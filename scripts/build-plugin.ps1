# Compila o plugin PRX do GNM Scale com o GoldHEN Plugins SDK (MIT) + OpenOrbis.
# Requer: OpenOrbis (OO_PS4_TOOLCHAIN ou %USERPROFILE%\OpenOrbis\PS4Toolchain), clang, ld.lld, llvm-ar, git.
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$tc = $env:OO_PS4_TOOLCHAIN
if (-not $tc) { $tc = Join-Path $env:USERPROFILE 'OpenOrbis\PS4Toolchain' }
if (-not (Test-Path $tc)) { throw "OpenOrbis nao encontrado: $tc" }
$sdk = $env:GOLDHEN_SDK
if (-not $sdk) {
    $sdk = Join-Path $root 'build\ghsdk'
    if (-not (Test-Path $sdk)) { git clone --depth 1 https://github.com/GoldHEN/GoldHEN_Plugins_SDK.git $sdk; if ($LASTEXITCODE) { throw 'clone do SDK falhou' } }
}
$out = Join-Path $root 'build\plugin'
$obj = Join-Path $out 'obj'
Remove-Item -Recurse -Force $out -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $obj | Out-Null

$defs = '-D__ORBIS__=1','-DORBIS=1','-DPS4=1','-DOO=1','-D__PS4__=1','-DOOPS4=1','-D__OOPS4__=1','-D__BSD_VISIBLE=1','-D_BSD_SOURCE=1','-D__OPENORBIS__=1'
$common = @('--target=x86_64-pc-freebsd12-elf','-fPIC','-funwind-tables','-isysroot',$tc,'-isystem',"$tc\include","-I$sdk\include",'-Wno-c99-designator') + $defs

# libGoldHEN_Hook (estatica) + crtprx.o
$sdkObjs = @()
foreach ($c in Get-ChildItem "$sdk\source\*.c") {
    $o = Join-Path $obj ("sdk_" + $c.BaseName + '.o')
    & clang @common '-march=btver2' '-std=c11' -c $c.FullName -o $o; if ($LASTEXITCODE) { throw "falha: $($c.Name)" }
    $sdkObjs += $o
}
$crt = Join-Path $obj 'crtprx.o'
& clang -target x86_64-pc-linux-gnu -ffreestanding -nostdlib -fno-builtin -fPIC -isysroot $tc -isystem "$tc\include" "-I$sdk\include" -c "$sdk\crt\crtprx.c" -o $crt
if ($LASTEXITCODE) { throw 'falha crtprx' }
$lib = Join-Path $out 'libGoldHEN_Hook.a'
& llvm-ar --format=bsd rcs $lib @sdkObjs $crt; if ($LASTEXITCODE) { throw 'llvm-ar falhou' }

# plugin
$src = Join-Path $root 'plugin\gnm_profile_plugin\source'
$po = Join-Path $obj 'main.o'
& clang @common -D__FINAL__=1 -c "$src\main.c" -o $po; if ($LASTEXITCODE) { throw 'falha main.c' }
$elf = Join-Path $out 'gnm_profile_plugin.elf'
& ld.lld -m elf_x86_64 -pie -e _init --script "$tc\link.x" --eh-frame-hdr "-L$tc\lib" "-L$out" $crt $po -lGoldHEN_Hook -lkernel -lc -lSceVideoOut -lSceSysmodule -o $elf
if ($LASTEXITCODE) { throw 'link falhou' }
& "$tc\bin\windows\create-fself.exe" "-in=$elf" "-out=$out\gnm_profile_plugin.oelf" "-lib=$out\gnm_profile_plugin.prx" --paid 0x3800000000000011
if ($LASTEXITCODE) { throw 'create-fself falhou' }
Get-Item "$out\gnm_profile_plugin.prx" | Select-Object FullName, Length
