param([string]$Tag = "v0.1.0")
$ErrorActionPreference = "Stop"
Set-Location (Split-Path $PSScriptRoot -Parent)

# Gera o PKG e publica como asset de uma GitHub Release (o .pkg nunca vai para o git).
& "$PSScriptRoot\build-pkg.ps1"
if ($LASTEXITCODE -ne 0) { throw "Falha ao gerar o PKG." }

$Pkg = Get-ChildItem build\pkg -Filter *.pkg | Select-Object -First 1
if (-not $Pkg) { throw "PKG nao encontrado em build\pkg." }

$ErrorActionPreference = "Continue"
gh release view $Tag *> $null
$Exists = ($LASTEXITCODE -eq 0)
$ErrorActionPreference = "Stop"
if ($Exists) {
    gh release upload $Tag $Pkg.FullName --clobber
} else {
    gh release create $Tag $Pkg.FullName --title "GNM Scale $Tag" --notes "PKG experimental para PS4 (OpenOrbis). Nao testado em console real. Veja docs/PKG.md."
}
if ($LASTEXITCODE -ne 0) { throw "Falha ao publicar a release." }
Write-Host "Release $Tag publicada." -ForegroundColor Green
