# Empacotar o PKG para PS4

Gera `build\pkg\IV0000-GNMS00001_00-GNMSCALEPS400001.pkg` com o OpenOrbis PS4 Toolchain.

## Requisitos
- [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain). O script usa `OO_PS4_TOOLCHAIN` ou, se ausente, `%USERPROFILE%\OpenOrbis\PS4Toolchain`.
- `clang++` e `ld.lld` no PATH (LLVM) e .NET para o `PkgTool.Core`.
- Opcional: `pkg\sce_sys\icon0.png` (512x512). Sem ele, usa o icone do exemplo hello_world.

## Uso
```powershell
powershell -ExecutionPolicy Bypass -File scripts\build-pkg.ps1
```

## Estrutura
- `src\ps4\main_ps4.cpp`: entrada do eboot (usa `gnm_core` e o `FrameLimiter` de 60 FPS).
- `scripts\build-pkg.ps1`: compila (`-D_GNU_SOURCE` e necessario para o libc++ do OpenOrbis), linka, gera `eboot.bin`, `param.sfo`, `.gp4` e o PKG. `right.sprx` e `sce_module` vem do exemplo hello_world do toolchain.

## Estado
- O PKG e gerado com sucesso, mas **nao foi testado em um PS4** (requer console com homebrew).
- O eboot lista os jogos de `/user/app` (via dirent) e roda o pipeline de demonstracao; a melhoria de imagem real dentro dos jogos nao esta implementada (exige captura de frames).

## Publicar release
```powershell
powershell -ExecutionPolicy Bypass -File scripts\publish-release.ps1 -Tag v0.1.0
```
Gera o PKG e o anexa a uma GitHub Release (via `gh`). O `.pkg` nao e versionado no git.

## Limitacoes
- Melhoria de imagem e 60 FPS dentro de jogos de terceiros exigem captura/injecao de frames (runtime), fora do escopo atual: hoje o projeto fornece o nucleo de processamento, o limitador de frames e a listagem de jogos.
