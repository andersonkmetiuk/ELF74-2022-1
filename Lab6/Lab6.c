/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL.
Lab6 - RTOS ThreadX - Escalonamento
##############################################################################*/
#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include <stdio.h>

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100

/*
Programas 
  N=0 -> a) Escalonamento por time-slice de 50 ms. Todas as tarefas com mesma prioridade.
  N=1 -> b) Escalonamento sem time-slice e sem preempção. Prioridades estabelecidas no passo 3. A preempção pode ser evitada com o “ 
  preemption threshold” do ThreadX.
  N=2 -> c) Escalonamento preemptivo por prioridade.
  N=3 -> d) Implemente um Mutex compartilhado entre T1 e T3. No início de cada job estas tarefas devem solicitar este mutex e liberá-lo no 
  final. Use mutex sem herança de prioridade. Observe o efeito na temporização das tarefas.
  N=4 -> e) Idem acima, mas com herança de prioridade.
*/
#define N_Programa 0 //selecionar o número do programa para as letras "a)" até "e)"

/* Define as Threads utilizadas e as estruturas de dados do S.O. ... */
TX_THREAD thread_1;
TX_THREAD thread_2;
TX_THREAD thread_3;
TX_BYTE_POOL byte_pool_0;
TX_MUTEX mutex_0;

/* Define o byte pool memory. */
UCHAR byte_pool_memory[DEMO_BYTE_POOL_SIZE];

/* Definição dos prototipos das funções de execução das Threads. */
void thread_1_entry(ULONG thread_input);
void thread_2_entry(ULONG thread_input);
void thread_3_entry(ULONG thread_input);

void Setup_Leds()
{
  /*
  --- PORTS ---
    LEDs - PN0,PN1,PF0,PF4
    Led1=PN1,Led2=PN0,Led3=PF4,Led4=PF0
  -------------
  */
    
  //Habilita os Ports
  //GPIO port N
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {/*Espera habilitar o port*/}
  
  //GPIO port F
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {/*Espera habilitar o port*/}    
  
  //Define como entrada ou saída
  //SAÍDAS
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); //lED PN0,PN1
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); //LED PF0,PF4
  
}

/* função main. */
int main()
{
  Setup_Leds();
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
    
  //Variáveis
  UINT Time_Slice[3];
  UINT Prioridade_Thread[3];
  UINT Preempcao_Thread[3];
  int i;
    
    for(i=0;i<3;i++) //preenche os vetores com as configs das threads
    { 
      if(N_Programa==0)//letra a
      {
        Time_Slice[i]=50;
        Prioridade_Thread[i]=0;
        Preempcao_Thread[i]=0;
      }
      else if (N_Programa==1)//letra b
      {
        Time_Slice[i]= TX_NO_TIME_SLICE; 
        Prioridade_Thread[i]=i;
        Preempcao_Thread[i]=0;
      }  
      else //letra c,d,e
      {
        Time_Slice[i]= TX_NO_TIME_SLICE; 
        Prioridade_Thread[i]=i;
        Preempcao_Thread[i]=i;
      }
    }   
    

  /* Cria o bloco de memoria para alocação das tarefas */
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);
  
  
/* 
*****  CRIAÇÃO DE THREADS******
UINT tx_thread_create(
    TX_THREAD *thread_ptr,
    CHAR *name_ptr, 
    VOID (*entry_function)(ULONG),
    ULONG entry_input, 
    VOID *stack_start,
    ULONG stack_size, 
    UINT priority, <--- prioridade
    UINT preempt_threshold,  <---- preempção
    ULONG time_slice, <--- time slice
    UINT auto_start);  
*/
  
  /* Aloca a pilha para a tarefa1 */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa1 com time slice. */
  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1, pointer, DEMO_STACK_SIZE, Prioridade_Thread[0], Preempcao_Thread[0], Time_Slice[0], TX_AUTO_START);

  /* Aloca a pilha para a tarefa2. */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa2 com time slice. */
  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2, pointer, DEMO_STACK_SIZE, Prioridade_Thread[1], Preempcao_Thread[1], Time_Slice[1], TX_AUTO_START);

  /* Aloca a pilha para a tarefa3. */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa3 com time slice. */
  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 2, pointer, DEMO_STACK_SIZE, Prioridade_Thread[2], Preempcao_Thread[2], Time_Slice[2], TX_AUTO_START);
  
  //****MUTEX****
  if(N_Programa==3)//letra d
  {
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT); //sem herança
  }
  if(N_Programa==4)//letra e
  { 
    tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT); //herança
  }  

  /* devolve o bloco para o pool */
  tx_block_release(pointer);
}

/* função de execução da Thread1. */
void thread_1_entry(ULONG thread_input)
{
  UINT Led1=0;
  /* Esta tarefa controla o Blink do Led1 do kit. */
  while (1)
        {
          //Colocar a tarefa para dormir por 25 ticks do S.O.
          tx_thread_sleep(25);
          //altera o estado do led ligado / desligado
          Led1=!Led1;
          //escreve o estado do led no pino
          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
        }
}

/* função de execução da Thread2. */
void thread_2_entry(ULONG thread_input)
{
  UINT Led2=0;
  /* Esta tarefa controla o Blink do Led2 do kit. */
  while (1)
        {
          //Colocar a tarefa para dormir por 50 ticks do S.O.
          tx_thread_sleep(50);
          //altera o estado do led ligado / desligado
          Led2=!Led2;
          //escreve o estado do led no pino
          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
      }
}

/* função de execução da Thread2. */
void thread_3_entry(ULONG thread_input)
{
  UINT Led3=0;
  /* Esta tarefa controla o Blink do Led2 do kit. */
  while (1)
        {
          //Colocar a tarefa para dormir por 50 ticks do S.O.
          tx_thread_sleep(75);
          //altera o estado do led ligado / desligado
          Led3=!Led3;
          //escreve o estado do led no pino
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
        }
}
