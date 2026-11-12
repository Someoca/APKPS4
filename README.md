# GNM SCALE PS4 - Enhanced Edition

Núcleo experimental C++17 para pesquisa de processamento de imagem e
possíveis caminhos de integração homebrew no PlayStation 4.

## Estado atual

- Processamento RGBA em CPU.
- Ampliação bilinear.
- Filtro de nitidez espacial.
- Mistura temporal experimental.
- Perfis declarativos FAT, Slim e Pro.
- Testes automatizados do núcleo.

## Limitações

Este repositório ainda não contém backend GNM/GNMX para PS4, integração
com framebuffer de jogos externos, apresentação no console ou geração
de quadros compensada por movimento. Os perfis de hardware não certificam
compatibilidade com modelos, firmware ou versões GoldHEN.

A mistura temporal não é frame generation real.

## Compilar no Windows

Execute `scripts/build-host.ps1` no PowerShell 5.1.
É necessário ter Clang++ no PATH.

## Estrutura

- `include/gnm`: interfaces públicas.
- `src`: implementação do núcleo.
- `tests`: testes automatizados.
- `docs`: arquitetura, compatibilidade e roadmap.
- `scripts`: automação de compilação.
