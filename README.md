# ELF74 - Sistemas Embarcados
- Aluno: Anderson Luiz de Souza Kmetiuk
- Email: andersonkmetiuk@alunos.utfpr.edu.br

- Professor: Prof. Paulo Denis Garcez da Luz
- Email: garcez@professores.utfpr.edu.br

- Disciplina: ELF74 - Sistemas Embarcados
- Ano: 2022
- Semestre: 1

## Documentações
- Tiva C Series &rarr; TM4C1294NCPDT
  - [Datasheet](https://www.ti.com/lit/ds/symlink/tm4c1294ncpdt.pdf?ts=1687451818296)
  - [Cortex M3/M4F Instruction Set](https://www.ti.com/lit/ug/spmu159a/spmu159a.pdf)
  - [ARM and Thumb-2 Instruction Set - Quick Reference Card](https://developer.arm.com/documentation/qrc0001/latest/)
- IAR Embedded Workbench for ARM v9.20 &rarr; https://www.iar.com/products/architectures/arm/iar-embedded-workbench-for-arm/
- TivaWare C Series v2.2.0.295
  -  [Download](https://www.ti.com/tool/SW-TM4C)
  -  [TivaWare Peripheral Driver Library](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf?ts=1687399770541&ref_url=https%253A%252F%252Fwww.ti.com%252Ftool%252FSW-TM4C)
- ThreadX 
  - [Github Repo](https://github.com/azure-rtos/threadx)
  - [Documentation](https://docs.microsoft.com/en-us/azure/rtos/threadx/)
- Elevador
  - [Projeto SimSE32](https://pessoal.dainf.ct.utfpr.edu.br/douglasrenaux/index_files/Page392.htm)

## DevKit
- LEDs
  - PN1 &rarr; LED 1
  - PN0 &rarr; LED 2
  - PF0 &rarr; LED 3
  - PF4 &rarr; LED 4
- Switchs
  - PJ0 &rarr; SW1
  - PJ1 &rarr; SW2
 
## Lab 1
- Configuração inicial da placa

## Lab 2
- Deve-se ler uma tecla: Tecla_1 (PJ0/USR_SW1) para iniciar a contagem do tempo e enquanto a Tecla_1 não for pressionada nada acontece.
- O disparo do “tempo de jogo” deve ser feito pela Tecla_2 (PJ1/USR_SW2) somente após ter sido dado início do jogo na Tecla_1.
- Caso o tempo exceda de 3s, travar o uso da segunda tecla: Tecla_2 e indicar que o jogo acabou acendendo o LED:D4 (PF0).
- O programa pode deve reiniciar todo o processo caso a Tecla_1 (PJ0/USR_SW1) seja pressionada novamente, mesmo estando no estado do “tempo de jogo”.
- A reposta do “tempo de jogo” deve ser fornecida em "ms".

### Fluxograma
<p align="center"><img alt="Lab2-Fluxogram" src= "Lab2/Fluxograma/Lab2%20.jpg" /> </p>

## Lab 3 - Histograma
- Integração de Assembly com C.
### Definição do problema:
- Desenvolva uma função em assembly que constrói o histograma de uma imagem em tons de cinza com 8-bits por pixel.
- Parâmetros de entrada:
  - image width - número de pixels em uma linha da imagem.
  - image height - altura da imagem em pixels.
  - starting address - endereço do primeiro pixel da imagem.
  - histogram - endereço inicial de um vetor de tamanho 256. Cada posição do vetor armazena um inteiro
sem sinal de 16-bits. Este vetor não possui dados válidos quando a função é chamada. Ele é usado
apenas para o retorno da função.
- Retorno:
  - inteiro sem sinal de 16 bits indicando o número de pixels processados.
- Restrições:
  - O tamanho máximo da imagem é de 64K (65.536) pixels.
  - A função retorna o valor 0 se o tamanho da imagem for superior a 64K.

## Prototipo da função
```
uint16_t EightBitHistogram(uint16_t width, uint16_t height, uint8_t * p_image, uint16_t * p_histogram);
```

## Exemplo de imagem (arquivo images.c)
```
const uint8_t image0[HEIGTH0][WIDTH0] = {
    { 20, 16, 16, 18}, {255, 255, 0, 0}, {32,32,32,32}
};
```

### Domínio do problema:
- Um bitmap é composto por pixels dispostos na forma de uma matriz. Numa imagem em tons de cinza, cada pixel é representado por um valor numérico indicando o nível de luminosidade daquele pixel.
- Numa imagem em tons de cinza de 8-bits, cada pixel é representado por um valor de 8-bits, portanto de 0 (preto) até 255 (branco).
- Um histograma é uma representação gráfica da distribuição de tons de uma imagem. O eixo horizontal apresenta os possíveis valores dos pixels (neste caso de 0 a 255) e o eixo vertical indica quantos pixels da imagem tem aquele valor.

### Para realizar a integração do C com o Assembly utilizamos o seguinte trecho de código:
```
; -------------------------------------------------------------------------------
        PUBLIC EightBitHistogram
        SECTION .text : CODE (2) 
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
        EXPORT EightBitHistogram ;exportar a função pro c
; -------------------------------------------------------------------------------
```
### Os parâmetros são armazenados nos seguintes registradores:
- R0 &rarr; width
- R1 &rarr; height
- R2 &rarr; endereço/ponteiro da imagem
- R3 &rarr; endereço/ponteiro do histograma
    
### A Tiva C armazena o endereço de retorno em R4, por isso temos:
```
  MOV R4, R3  
  BX LR
```
