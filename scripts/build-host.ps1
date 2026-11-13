$ErrorActionPreference = "Stop"
Set-Location (Split-Path $PSScriptRoot -Parent)

$Compiler = Get-Command clang++ -ErrorAction SilentlyContinue
if (-not $Compiler) {
    throw "clang++ nao encontrado no PATH."
}

New-Item -ItemType Directory -Force -Path "build" | Out-Null

$Sources = @(
    "src/core/image.cpp",
    "src/graphics/pipeline.cpp",
    "src/hardware/profile.cpp",
    "src/library/library.cpp"
)

$Common = @(
    "-std=c++17",
    "-O2",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Iinclude"
)

Write-Host "Compilando demonstracao..." -ForegroundColor Cyan
$DemoArgs = $Common + $Sources + @("src/main.cpp", "-o", "build/gnm_scale.exe")
& clang++ @DemoArgs
if ($LASTEXITCODE -ne 0) { throw "Falha ao compilar demonstracao." }

Write-Host "Compilando testes..." -ForegroundColor Cyan
$TestArgs = $Common + $Sources + @("tests/test_pipeline.cpp", "-o", "build/test_pipeline.exe")
& clang++ @TestArgs
if ($LASTEXITCODE -ne 0) { throw "Falha ao compilar testes." }

Write-Host "Executando testes..." -ForegroundColor Cyan
& ".\build\test_pipeline.exe"
if ($LASTEXITCODE -ne 0) { throw "Testes falharam." }

Write-Host "Executando demonstracao..." -ForegroundColor Cyan
& ".\build\gnm_scale.exe"
if ($LASTEXITCODE -ne 0) { throw "Demonstracao falhou." }

Write-Host "`nBUILD HOST E TESTES CONCLUIDOS." -ForegroundColor Green
