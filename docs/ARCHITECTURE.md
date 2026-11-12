# Arquitetura

O núcleo é dividido em três áreas:

1. Core: armazenamento e acesso a imagens RGBA.
2. Graphics: upscale bilinear, nitidez espacial e mistura temporal.
3. Hardware: perfis declarativos para FAT, Slim e Pro.

O pipeline atual opera em CPU e recebe imagens fornecidas pela aplicação.
Não intercepta nem captura o framebuffer de jogos do PS4.
