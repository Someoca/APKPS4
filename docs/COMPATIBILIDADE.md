# Matriz de compatibilidade (verificÃ¡vel)

Nada deste projeto foi testado em PS4 real. Esta pÃ¡gina sÃ³ registra o que as fontes oficiais afirmam.

## Alvo: firmware 13.52 + GoldHEN 2.4b18.11

Alvo declarado pelo usuário: PS4 com firmware **13.52** (14.00: não verificado / fora de suporte declarado até haver fonte).

- Fontes **secundárias** (não verificadas por mim; o site bloqueou acesso automatizado): https://onejailbreak.com/blog/ps4-13-52-jailbreak-adds-goldhen-2-4b18-11-support/ e https://onejailbreak.com/blog/goldhen-ps4/ — indicam GoldHEN 2.4b18.11 com suporte a 13.52 (exploit cobrindo 13.02–13.52).
- Fonte **primária** consultada: release 2.4b18 (28/10/2024) em github.com/GoldHEN/GoldHEN lista firmwares até 11.00 (5.05, 6.71/6.72, 9.00, 9.60, 10.00/10.01, 10.50, 10.70/10.71, 11.00). A 2.4b18.11 e o 13.52 **não** aparecem nas releases do GitHub; não foram confirmados em fonte primária.
- Nada foi testado em hardware. O app não garante compatibilidade.
## O que as fontes dizem

| Componente | InformaÃ§Ã£o verificada | Fonte |
|---|---|---|
| GoldHEN (release no GitHub) | Ãšltima release publicada no repositÃ³rio oficial: v2.3 (dez/2022). CÃ³digo-fonte privado; builds mais novos (2.4bXX) sÃ£o distribuÃ­dos pelo projeto, fora do repositÃ³rio. | https://github.com/GoldHEN/GoldHEN , https://api.github.com/repos/GoldHEN/GoldHEN/releases/latest |
| GoldHEN â€“ plugins | "Plugins support" Ã© recurso do GoldHEN; SDK de plugins Ã© MIT e usa o OpenOrbis. | https://github.com/GoldHEN/GoldHEN , https://github.com/GoldHEN/GoldHEN_Plugins_SDK |
| ps4debug (projeto GoldHEN) | Firmwares citados (v1.1.19): 5.05 a 12.02 (lista na release). SÃ³ como indÃ­cio do alcance do ecossistema; nÃ£o vale para o GoldHEN. | https://github.com/GoldHEN/ps4debug/releases |
| OpenOrbis PS4 Toolchain | Gera homebrew sem o SDK da Sony; o README nÃ£o define matriz de firmware. | https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain |

Os firmwares suportados pelo GoldHEN variam por build; consulte as notas da release que vocÃª instalar. Este repositÃ³rio **nÃ£o** comprova compatibilidade com nenhum firmware especÃ­fico.

## Status do app

| Item | Status |
|---|---|
| Build host + testes | Validado (clang++) |
| PKG com OpenOrbis | Gerado; nunca instalado/executado em PS4 |
| Plugin GoldHEN | NÃ£o criado/testado |
| Firmware/GoldHEN especÃ­fico | NÃ£o verificado |

