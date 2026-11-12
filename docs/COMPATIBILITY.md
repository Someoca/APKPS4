# Compatibilidade GNM SCALE PS4

## Firmwares pretendidos

| Firmware | Objetivo | Estado |
|---|---|---|
| PS4 13.52 | Execucao homebrew | Pendente de teste |
| PS4 14.00 | Execucao homebrew | Pendente de teste |

## Modelos

| Modelo | Firmware 13.52 | Firmware 14.00 |
|---|---|---|
| PS4 FAT | Pendente | Pendente |
| PS4 Slim | Pendente | Pendente |
| PS4 Pro | Pendente | Pendente |

## Situacao tecnica

- Nucleo C++17 para Windows: compilacao e testes aprovados.
- Upscale bilinear: implementado em CPU.
- Filtro de nitidez: implementado em CPU.
- Mistura temporal: experimental.
- Geracao real de quadros: nao implementada.
- Backend GNM/GNMX integrado: pendente.
- OpenGNM: dependencia candidata em avaliacao.
- PKG PS4 do GNM SCALE: pendente.
- Testes em console: pendentes.

A presenca de suporte de firmware no GoldHEN nao comprova
compatibilidade do GNM SCALE.

Cada combinacao de modelo e firmware precisa ser validada
individualmente em hardware.