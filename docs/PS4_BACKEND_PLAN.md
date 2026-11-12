# Plano de integracao OpenGNM

Repositorio:
https://github.com/PS4-OpenGNM/opengnm

Revisao fixada:
4b295ca54c82c83acf308d1c646a2dfa9ae57350

## Etapas

1. Validar testes genericos da biblioteca.
2. Identificar os alvos reais do Makefile.
3. Validar compilador e linker Orbis no Windows.
4. Compilar os testes de ligacao.
5. Validar executavel minimo no console.
6. Criar camada de adaptacao do pipeline GNM SCALE.
7. Implementar apresentacao de imagem.
8. Medir desempenho por modelo.
9. Avaliar super-resolucao na GPU.
10. Pesquisar interpolacao com estimativa de movimento.

## Regras

Nao inventar simbolos GNM/GNMX.
Nao assumir acesso ao framebuffer de jogos externos.
Nao tratar mistura temporal como frame generation.
Nao afirmar compatibilidade sem testes reais.