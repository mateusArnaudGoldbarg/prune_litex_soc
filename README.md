# Aceleração de Inferência em FPGA por Meio de Poda Consciente e Execução Esparsa

Este repositório apresenta a implementação e avaliação de um modelo de Machine Learning comprimido, utilizando poda iterativa consciente e execução esparsa em um sistema embarcado baseado em FPGA, com SoC desenvolvido via LiteX.

O objetivo principal é reduzir o tempo de inferência e o consumo energético, mantendo desempenho satisfatório, explorando a esparsidade estrutural do modelo diretamente no hardware.

---

## Visão Geral

Modelos de Machine Learning modernos apresentam elevado desempenho, porém à custa de altas demandas computacionais, o que limita sua aplicação em sistemas embarcados.  

Neste trabalho, aplica-se poda iterativa consciente durante o treinamento, seguida de quantização em ponto fixo, e a inferência é executada em um acelerador dedicado, capaz de explorar operações esparsas.

Os resultados demonstram uma redução de aproximadamente 70% no número de ciclos de clock, com speedup superior a 3× em relação ao modelo denso.

---

## Técnicas Utilizadas

- **Poda Iterativa Consciente**
  - Aplicada durante o treinamento
  - Limiar baseado no desvio padrão dos pesos
  - Remoção gradual de pesos pouco relevantes
- **Quantização em Ponto Fixo**
  - Conversão FP32 → Q15
  - Eliminação de operações em ponto flutuante
- **Execução Esparsa**
  - Processamento apenas de pesos não nulos
  - Número de ciclos proporcional à esparsidade

---

## Arquitetura do Sistema

O sistema segue uma abordagem de co-design hardware–software, composta por:

- **Treinamento**
  - Realizado off-line em ambiente computacional
  - Aplicação da poda iterativa consciente
- **SoC LiteX**
  - CPU soft-core
  - Interface UART para interação
  - Registradores CSR
  - Acelerador dedicado de produto escalar
- **Acelerador em Hardware**
  - Implementado em SystemVerilog
  - 1 operação MAC por ciclo
  - Suporte explícito à esparsidade via registrador `nnz`

---

## Fluxo de Execução

1. Treinamento do modelo com poda iterativa consciente  
2. Quantização do modelo para ponto fixo (Q15)  
3. Embarque do modelo no SoC LiteX  
4. Execução da inferência:
   - **Modo denso**: todos os pesos processados  
   - **Modo esparso**: apenas pesos não nulos  
5. Medição do desempenho em ciclos de clock  

---

## Avaliação de Desempenho

- **FPGA**: Colorlight i9  
- **Frequência de operação**: 60 MHz  
- **Métrica**: Número de ciclos de clock por camada  
