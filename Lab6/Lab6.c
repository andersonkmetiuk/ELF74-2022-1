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
#define N_Programa 4 //selecionar o número do programa para as questões "a)" até "e)"
#define Thread_Loop 10
#define Thread_Sleep 100 //1 tick SO = 10ms

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

//variáveis globais para monitorar o tempo
UINT tempo_antes_global=0;
UINT tempo_depois_global=0;
UINT tempo_total_global=0;

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
void main()
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
  UINT Prioridade_Thread[3];  
  UINT Preempcao_Thread[3];
  UINT Time_Slice[3];  
  UINT i;
    
    for(i=0;i<3;i++) //preenche os vetores com as configs das threads
    { 
      if(N_Programa==0)//questão a
      {        
        Prioridade_Thread[i]=0;
        Preempcao_Thread[i]=0;
        Time_Slice[i]=50;
      }
      else if (N_Programa==1)//questão b
      {
        Prioridade_Thread[i]=i;
        Preempcao_Thread[i]=0;
        Time_Slice[i]= TX_NO_TIME_SLICE; 
      }  
      else //questões c,d,e
      {         
        Prioridade_Thread[i]=i;
        Preempcao_Thread[i]=i;
        Time_Slice[i]= TX_NO_TIME_SLICE;
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
  
  /*----------MUTEX----------*/
  if(N_Programa==3)//questão d
  {
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT); //sem herança
  }
  if(N_Programa==4)//questão e
  { 
    tx_mutex_create(&mutex_0, "mutex 0", TX_INHERIT); //herança
  }  

  /* devolve o bloco para o pool */
  tx_block_release(pointer);
}

/*----------THREAD 1----------*/
void thread_1_entry(ULONG thread_input)
{
  UINT Led1=0;
  UINT t1,t1_antes,t1_depois,t1_total;
  
  t1_antes = tx_time_get();  
  
  if(N_Programa==3 || N_Programa==4) //Mutex - questão d,e - Control
  {
    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER); //fica no controle do Mutex
  }
  
  /* Esta tarefa controla o Blink do Led1 do kit. */
  for(t1=0; t1 <= Thread_Loop ; t1++)
  {
      //Colocar a tarefa para dormir por 25 ticks do S.O.
      tx_thread_sleep(Thread_Sleep);
      //altera o estado do led ligado / desligado
      Led1=!Led1;
      //escreve o estado do led no pino
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
  }
  
  if(N_Programa==3 || N_Programa==4) //Mutex - questão d,e - Release
  {
     tx_mutex_put(&mutex_0); //libera o Mutex
  }
  
  t1_depois = tx_time_get();
  t1_total = t1_depois - t1_antes;
  
  /*---tempo global---*/
  tempo_antes_global = t1_antes;
  tempo_total_global += t1_total;
  tempo_depois_global = tempo_total_global;  
  
  printf("Thread 1 = %u ms \nTempo Antes = %u ms \nTempo Depois = %u ms \n\n", t1_total,t1_antes,t1_depois);
  
  /*---tempo global---*/
  printf("TAG = %u ms \nTDG = %u ms \nTempo Total = %u ms \n\n", tempo_antes_global,tempo_depois_global,tempo_total_global);
}

/*----------THREAD 2----------*/
void thread_2_entry(ULONG thread_input)
{
  UINT Led2=0;
  UINT t2,t2_antes,t2_depois,t2_total;
  
  t2_antes = tx_time_get();
  
  /* Esta tarefa controla o Blink do Led2 do kit. */
  for(t2=0; t2 <= Thread_Loop ; t2++)
  {
      //Colocar a tarefa para dormir por 25 ticks do S.O.
      tx_thread_sleep(Thread_Sleep);
      //altera o estado do led ligado / desligado
      Led2=!Led2;
      //escreve o estado do led no pino
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
  }
  
  t2_depois = tx_time_get();
  t2_total = t2_depois - t2_antes;
  
  /*---tempo global---*/
  tempo_antes_global = tempo_depois_global;
  tempo_total_global += t2_total; 
  tempo_depois_global = tempo_total_global;
  
  printf("Thread 2 = %u ms \nTempo Antes = %u ms \nTempo Depois = %u ms \n\n", t2_total,t2_antes,t2_depois);
  
  /*---tempo global---*/
  printf("TAG = %u ms \nTDG = %u ms \nTempo Total = %u ms \n\n", tempo_antes_global,tempo_depois_global,tempo_total_global);
}

/*----------THREAD 3----------*/
void thread_3_entry(ULONG thread_input)
{
  UINT Led3=0;
  UINT t3,t3_antes,t3_depois,t3_total;
  
  t3_antes = tx_time_get();
  
  if(N_Programa==3 || N_Programa==4) //Mutex - questão d,e - Control
  {
    tx_mutex_get(&mutex_0, TX_WAIT_FOREVER); //fica no controle do Mutex
  }
  
  /* Esta tarefa controla o Blink do Led3 do kit. */
  for(t3=0; t3 <= Thread_Loop ; t3++)
  {
      //Colocar a tarefa para dormir por 25 ticks do S.O.
      tx_thread_sleep(Thread_Sleep);
      //altera o estado do led ligado / desligado
      Led3=!Led3;
      //escreve o estado do led no pino
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
  }
  
  if(N_Programa==3 || N_Programa==4) //Mutex - questão d,e - Release
  {
    tx_mutex_put(&mutex_0); //libera o Mutex    
  }
  
  t3_depois = tx_time_get();
  t3_total = t3_depois - t3_antes;
  
  /*---tempo global---*/
  tempo_antes_global = tempo_depois_global;
  tempo_total_global += t3_total; 
  tempo_depois_global = tempo_total_global;
  
  printf("Thread 3 = %u ms \nTempo Antes = %u ms \nTempo Depois = %u ms \n\n", t3_total,t3_antes,t3_depois);
  
  /*---tempo global---*/
  printf("TAG = %u ms \nTDG = %u ms \nTempo Total = %u ms \n\n", tempo_antes_global,tempo_depois_global,tempo_total_global);
}
