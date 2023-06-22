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
