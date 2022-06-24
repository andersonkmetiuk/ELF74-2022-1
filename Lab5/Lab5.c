/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL.
Lab5 - RTOS ThreadX - Blink Led1 e Led2
##############################################################################*/
#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100

/* Define as Threads utilizadas e as estruturas de dados do S.O. ... */
TX_THREAD tled1;
TX_THREAD tled2;
TX_BYTE_POOL byte_pool_0;

/* Define o byte pool memory. */
UCHAR byte_pool_memory[DEMO_BYTE_POOL_SIZE];

/* Definição dos prototipos das funções de execução das Threads. */
void TLed1();
void TLed2();

/* função main. */
int main()
{
  //habilitar gpio port N
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  //aguardar o periférico ficar pronto para uso
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {/*Espera habilitar o port*/}
  //configura o pin_0 e pin_1 como saída
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  /* Executar o ThreadX kernel.  */
  tx_kernel_enter();
}

/* Define o sistema embarcado inicial. */
void tx_application_define(void *first_unused_memory)
{
  CHAR *pointer = TX_NULL;
  #ifdef TX_ENABLE_EVENT_TRACE
    tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
  #endif
  /* Cria o bloco de memoria para alocação das tarefas */
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);
  /* Aloca a pilha para a tarefa1 */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa1 com time slice. */
  tx_thread_create(&tled1, "thread 1", TLed1, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  /* Aloca a pilha para a tarefa2. */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa2 com time slice. */
  tx_thread_create(&tled2, "thread 2", TLed2, 2, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  /* devolve o bloco para o pool */
  tx_block_release(pointer);
}

/* função de execução da Thread1. */
void TLed1()
{
UINT Led1=0;
/* Esta tarefa controla o Blink do Led1 do kit. */
while (1)
      {
        //Colocar a tarefa para dormir por 100 ticks do SO => 100 Ticks * 10ms = 1s
        tx_thread_sleep(100);
        //altera o estado do led ligado / desligado
        Led1=!Led1;
        //escreve o estado do led no pino
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
      }
}

/* função de execução da Thread2. */
void TLed2()
{
UINT Led2=0;
/* Esta tarefa controla o Blink do Led2 do kit. */
while (1)
      {
        //Colocar a tarefa para dormir por 200 ticks do S.O. => 200 Ticks * 10ms = 2s
        tx_thread_sleep(200);
        //altera o estado do led ligado / desligado
        Led2=!Led2;
        //escreve o estado do led no pino
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
    }
}
