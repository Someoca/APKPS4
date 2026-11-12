# Avaliacao da biblioteca OpenGNM

## Referencia

Repositorio: https://github.com/PS4-OpenGNM/opengnm

Revisao fixada: 4b295ca54c82c83acf308d1c646a2dfa9ae57350

Licenca declarada: MIT.

## Objetivo

Avaliar a biblioteca como backend GNM para o GNM SCALE PS4.

## Validacoes automatizadas

- Testes genericos da biblioteca.
- Compilacao da biblioteca para Orbis.
- Link smoke com libSceGnmDriver e libSceVideoOut.
- Compilacao do executavel de teste de hardware.

## Limites

A compilacao e o link nao comprovam execucao no console.

A integracao ao pipeline de imagem permanece pendente.

A compatibilidade com firmware 13.52 e 14.00 precisa de validacao
individual em hardware.

A biblioteca nao fornece automaticamente captura do framebuffer
de jogos externos nem frame generation.