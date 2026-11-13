$ErrorActionPreference = "Stop"
$Root = Split-Path $PSScriptRoot -Parent
Set-Location $Root

# Requer o OpenOrbis PS4 Toolchain; detecta OO_PS4_TOOLCHAIN ou %USERPROFILE%\OpenOrbis\PS4Toolchain
$Tc = $env:OO_PS4_TOOLCHAIN
if (-not $Tc) { $Tc = Join-Path $env:USERPROFILE "OpenOrbis\PS4Toolchain" }
if (-not (Test-Path $Tc)) {
    throw "OpenOrbis PS4 Toolchain nao encontrado. Defina OO_PS4_TOOLCHAIN."
}
$Tc = (Resolve-Path $Tc).Path
$Bin = Join-Path $Tc "bin\windows"
foreach ($t in "create-fself.exe", "create-gp4.exe", "PkgTool.Core.exe") {
    if (-not (Test-Path (Join-Path $Bin $t))) { throw "Ferramenta nao encontrada: $t" }
}
foreach ($t in "clang++", "ld.lld") {
    if (-not (Get-Command $t -ErrorAction SilentlyContinue)) { throw "$t nao encontrado no PATH." }
}
$Sample = Join-Path $Tc "samples\hello_world"

$Title = "GNM Scale"
$TitleId = "GNMS00001"
$Version = "01.00"
$ContentId = "IV0000-GNMS00001_00-GNMSCALEPS400001"

$Out = Join-Path $Root "build\pkg"
$Stage = Join-Path $Out "stage"
if (Test-Path $Out) { Remove-Item -Recurse -Force $Out }
New-Item -ItemType Directory -Force -Path $Out, "$Stage\sce_sys\about", "$Stage\sce_module" | Out-Null

$Sources = "src/core/image.cpp", "src/graphics/pipeline.cpp", "src/graphics/render.cpp", "src/hardware/profile.cpp", "src/library/library.cpp", "src/library/game_profile.cpp", "src/ps4/main_ps4.cpp"
$Objs = @()
foreach ($s in $Sources) {
    $o = Join-Path $Out (([IO.Path]::GetFileNameWithoutExtension($s)) + ".o")
    & clang++ --target=x86_64-pc-freebsd12-elf -std=c++17 -D_GNU_SOURCE -O2 -fPIC -funwind-tables -c `
        -isysroot $Tc -isystem "$Tc\include" -isystem "$Tc\include\c++\v1" -Iinclude $s -o $o
    if ($LASTEXITCODE -ne 0) { throw "Falha ao compilar $s" }
    $Objs += $o
}

$Elf = Join-Path $Out "gnm_scale.elf"
& ld.lld -m elf_x86_64 -pie --script "$Tc\link.x" --eh-frame-hdr "-L$Tc\lib" @Objs -lc -lkernel -lc++ -lSceVideoOut "$Tc\lib\crt1.o" -o $Elf
if ($LASTEXITCODE -ne 0) { throw "Falha ao linkar." }

& "$Bin\create-fself.exe" "-in=$Elf" "-out=$Out\gnm_scale.oelf" --eboot "$Stage\eboot.bin" --paid 0x3800000000000011
if ($LASTEXITCODE -ne 0) { throw "Falha em create-fself." }

$Pkg = Join-Path $Bin "PkgTool.Core.exe"
$Sfo = "$Stage\sce_sys\param.sfo"
& $Pkg sfo_new $Sfo
$entries = @(
    @("APP_TYPE", "Integer", 4, "1"), @("APP_VER", "Utf8", 8, $Version),
    @("ATTRIBUTE", "Integer", 4, "0"), @("CATEGORY", "Utf8", 4, "gd"),
    @("CONTENT_ID", "Utf8", 48, $ContentId), @("DOWNLOAD_DATA_SIZE", "Integer", 4, "0"),
    @("SYSTEM_VER", "Integer", 4, "0"), @("TITLE", "Utf8", 128, $Title),
    @("TITLE_ID", "Utf8", 12, $TitleId), @("VERSION", "Utf8", 8, $Version)
)
foreach ($e in $entries) {
    & $Pkg sfo_setentry $Sfo $e[0] --type $e[1] --maxsize $e[2] --value $e[3]
    if ($LASTEXITCODE -ne 0) { throw "Falha ao definir $($e[0]) no param.sfo." }
}

# Icone do projeto, se existir; senao usa o do exemplo do OpenOrbis.
$Icon = Join-Path $Root "pkg\sce_sys\icon0.png"
if (-not (Test-Path $Icon)) { $Icon = Join-Path $Sample "sce_sys\icon0.png" }
Copy-Item $Icon "$Stage\sce_sys\icon0.png"
Copy-Item "$Sample\sce_sys\about\right.sprx" "$Stage\sce_sys\about\right.sprx"
Copy-Item "$Sample\sce_module\*" "$Stage\sce_module" -Force

Push-Location $Stage
try {
    $Files = (Get-ChildItem -Recurse -File | ForEach-Object { $_.FullName.Substring($Stage.Length + 1).Replace("\", "/") }) -join " "
    & "$Bin\create-gp4.exe" -out pkg.gp4 --content-id=$ContentId --files $Files
    if ($LASTEXITCODE -ne 0) { throw "Falha em create-gp4." }
    & $Pkg pkg_build pkg.gp4 $Out
    if ($LASTEXITCODE -ne 0) { throw "Falha em PkgTool pkg_build." }
} finally {
    Pop-Location
}

Write-Host "PKG gerado em $Out" -ForegroundColor Green
Get-ChildItem $Out -Filter *.pkg | ForEach-Object { Write-Host $_.FullName }
