# Guia de instalação e teste no PS4 (GoldHEN)

> **Experimental. Nunca testado em console real.** Alvo declarado: PS4 firmware 13.52 com GoldHEN 2.4b18.11 (ver `COMPATIBILIDADE.md` para as ressalvas de fonte).
> Use por sua conta e risco, apenas no seu console e com seus jogos. O projeto não contém exploits, hooks de kernel próprios nem bypass de proteção.

## O que o app faz sozinho
- Lista os jogos instalados (`/user/app`, se legível pelo app) e roda o pipeline demo (CAS/bicubic em CPU, limitador 60 FPS a 1x).
- No host (PC): gerencia perfis por TITLE_ID (`--profile`) e exporta JSON (`--export`).

## O que depende do plugin/GoldHEN
- O plugin `gnm_profile_plugin.prx` lê o JSON e, se o perfil do jogo tiver `fps` 60/30/20, ajusta o flip rate via hook de `sceVideoOutSetFlipRate`. **Não aplica sharpening/upscaling nem resolução dentro dos jogos** (isso exigiria patches por jogo, fora do escopo). O eboot ainda não lê os perfis.

## Instalação
1. Pendrive em **exFAT ou FAT32**; copie o `.pkg` (release do GitHub) para a raiz.
2. No PS4 com GoldHEN ativo: Configurações > Package Installer (ou instale o PKG pelo menu do GoldHEN) e instale **GNM Scale**.
3. Perfis (gerados no PC): `gnm_scale --profile perfis.ini CUSA00001 fps=60` e `gnm_scale --export perfis.ini profiles.json`. Copie `profiles.json` para `/data/GoldHEN/gnm/profiles.json` (via FTP do GoldHEN, porta 2121).
4. Plugin: copie `gnm_profile_plugin.prx` para `/data/GoldHEN/plugins/` e edite `/data/GoldHEN/plugins.ini`:
```
[CUSA00001]
/data/GoldHEN/plugins/gnm_profile_plugin.prx
```
(requer plugins habilitados nas opções do GoldHEN).

## O que verificar e logs
- Abrir o app: deve exibir a lista de jogos e rodar o demo. Anote o que aparecer.
- Com o plugin: o klog (GoldHEN, porta 3232 — `nc IP_DO_PS4 3232`) deve mostrar `[gnm] titleid=...` e `[gnm] fps do perfil=...`.
- Se o console travar, remova a entrada de `plugins.ini` e reinicie. Envie o klog ao abrir uma issue.

## Build
`scripts\build-pkg.ps1` (PKG), `scripts\build-plugin.ps1` (PRX; baixa o GoldHEN Plugins SDK, MIT), `scripts\publish-release.ps1 -Tag v0.2.0-rc1 -Prerelease -WithPlugin`.

## Créditos e licenças
GoldHEN Plugins SDK (MIT): bucanero, ctn123, jocover, nik, OSM, SiSTRo; OpenOrbis Team. Padrão de plugin inspirado em `force_30_fps` (illusion0001, GoldHEN_Plugins_Repository); nenhum código GPL foi copiado.
