/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL.
Projeto - RTOS ThreadX - Elevador
##############################################################################*/
#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "system_TM4C1294.h" 
#include "driverlib/uart.h"

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100

/* Define as Threads utilizadas e as estruturas de dados do S.O. ... */
TX_THREAD tid_elevC;                  
TX_THREAD tid_elevD;               
TX_THREAD tid_elevE;                         
TX_THREAD tid_UART_READ;
TX_THREAD tid_UART_WRITE;
  
TX_TIMER timerc_id;
TX_TIMER timerd_id;
TX_TIMER timere_id;

TX_QUEUE fila_e_id;
TX_QUEUE fila_d_id;
TX_QUEUE fila_c_id;
TX_QUEUE fila_escrita_id;

TX_BYTE_POOL byte_pool_0;

/* Define o byte pool memory. */
UCHAR byte_pool_memory[DEMO_BYTE_POOL_SIZE];

/* Definição dos prototipos das funções de execução das Threads. */
void elevC (ULONG thread_input);
void elevD (ULONG thread_input);
void elevE (ULONG thread_input);
void UART_READ(ULONG thread_input);
void UART_WRITE(ULONG thread_input);


void UART0_Handler(void);

//variável do clock do sistema
uint32_t g_ui32SysClock;



void callbackc(ULONG timer_input){
     //ULONG my_old_time_slicec;
     //tx_thread_time_slice_change(&tid_elevC,0x0001,&my_old_time_slicec);
} // callback

void callbacke(ULONG timer_input){
//     ULONG my_old_time_slicee;
//     tx_thread_time_slice_change(&tid_elevE,0x0002,&my_old_time_slicee);

} // callback

void callbackd(ULONG timer_input){
//     ULONG my_old_time_slicec;
//     tx_thread_time_slice_change(&tid_elevE,0x0003,&my_old_time_slicec);
} // callback


/*----------------------------------------------------------------------------
 *      Thread 1 'elevC': Elevador Central
 *---------------------------------------------------------------------------*/
void elevC (ULONG thread_input) {
  static uint8_t andar_atual=0;
  static uint8_t proximo_andar=20; //proximo andar jamais recebera valor 20 pois são apenas 15 andares, portanto 20 significa só o momento de iniciar o elevador
  static uint8_t vetor_destino[16];
  
  static uint8_t i;
  static uint8_t botao_externo_andar = 0;
  static uint8_t sinal_feedback=0;  
  static uint8_t flag_vetor_zero=0;  
  volatile bool  flag_mudou_andar=false;  
  
  
  char comando[2];
  char mensagem[10];    
  char andares_letras[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};
 
  //ENVIA RESET
  comando[0]='c'; //Qual elevador
  comando[1]='r';//resetar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
  tx_thread_sleep(1000);        //osDelay(1000);

  //FECHAR AS PORTAS
  comando[0]='c';
  comando[1]='f';//fechar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
    
  for (;;) {
    tx_queue_receive(&fila_c_id, mensagem, TX_WAIT_FOREVER);
    //TRATAMENTO DOS BOTÕES EXTERNOS 
    if(mensagem[1]=='E'){//botao externo
      sscanf(mensagem,"%*c%*c%d%*c", &botao_externo_andar);  //botao_externo_andar: Auxiliar que recebe o valor do andar do botão externo pressionado 
      vetor_destino[botao_externo_andar]=1;                  //Atualiza o vetor de destino do elevador     
      
      //Caso seja a primeira vez
      if((proximo_andar == 20) ||((flag_vetor_zero == 0) && (flag_mudou_andar))){  //se for a primeira vez (proximo_andar = 20), se o vetor de destino tiver somente zeros, ou seja, só foi pedido um andar (flag_vetor_zero), se ja mudou de andar (flag_mudou_andar)
        flag_mudou_andar = false;
        proximo_andar = botao_externo_andar;
      }
      
      //manda subir caso andar_atual for menor que o próximo andar
       if(andar_atual < proximo_andar){
         comando[0]='c'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
      //manda descer caso andar_atual for maior que o próximo andar
      else if(andar_atual > proximo_andar){
             comando[0]='c'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
   }//if do botão externo    
   else if(mensagem[1]=='I'){ //botao interno     
      //Tem um vetor de ketras, pois para os botões internos, ao invés de ter números indicando os andares são letras, então aqui ja converte pra números e ja salva no vetor destino, só existe um vetor de destino tanto para subir quanto para descer
      for(i=0;i<16;i++){
        if(andares_letras[i]==mensagem[2]){
          vetor_destino[i]=1;
        }
      }
      
      //Atualiza o próximo andar
      for(i=0;i<16;i++){
          if(vetor_destino[i]==1){
             proximo_andar = i;
          }      
      }
      
      //manda subir caso andar_atual for menor que o próximo andar
      if(andar_atual < proximo_andar){
         comando[0]='c'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
      //manda descer caso andar_atual for maior que o próximo andar
      else if(andar_atual > proximo_andar){
             comando[0]='c'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
    }//if do botão interno
    
    else if((mensagem[1]>='0' && mensagem[0]=='c' && mensagem[1]<='9') || (mensagem[2]>='0' && mensagem[1]<='2'))//NÃO FUNCIONOU if(!(mensagem[1]=='A' || mensagem[1]=='F')) // If para verificar se é feedback pois A e F é apenas feedback de porta aberta e fechada
    {
  //RESOLVIDO: PENSAR SOBRE ISSO AQUI PROBLEMA QUE EU ATUALIZO O ANDAR QUANDO PASSA SENSOR, PORÉM EU POSSO APERTAR BOTÃO ANTES DE TERMINAR O DESTINO              
      flag_mudou_andar = true; //Flag que vai indicar que teve mudança de andar
      
         sscanf(mensagem,"%*c%d", &sinal_feedback);  
               andar_atual = sinal_feedback;
          if(andar_atual == proximo_andar){
           
           vetor_destino[andar_atual]=0; //Zera a posição que ja foi atendida no vetor de destino
           
           comando[0]='c'; //Qual elevador
           comando[1]='p';//parar
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           
           comando[0]='c'; //Qual elevador
           comando[1]='a';//abrir porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           tx_timer_activate(&timerc_id); //Da start no timer do sistema para que a porta fique aberta 3 segundos
          //** osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever); //Bloqueia a Thread esperando uma flag de sinal, só vai receber a flag quando for chamada a função de callback

          //RESOLVIDO: PAREI AQUI LÓGICA ERRADO
          for(i=0;i<16;i++){
              if(vetor_destino[i]==1){
                 proximo_andar = i;
                 flag_vetor_zero = 1;
                 break;
                }else
                  flag_vetor_zero =0;
          }     
         }  
    }//Fim else do feedback    
    else{// ultimo else são os feeback de porta aberta e porta fechada
        if(mensagem[1]=='A'){
           comando[0]='c'; //Qual elevador
           comando[1]='f';//fechar porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
        }        
    }
    
    //Aqui lógica para comandar a subida e descida do elevador, mesmo se os botões não forem mais apertados, porém o vetor de destino ainda tem elementos
    if(flag_vetor_zero == 1){
      
      if(andar_atual < proximo_andar){
         comando[0]='c'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='c'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }      
      
      
    }
  }//for infinito
}//Thread elevC
/*----------------------------------------------------------------------------
 *      Thread 2 'elevD': Elevador Direito
 *---------------------------------------------------------------------------*/
void elevD (ULONG thread_input) {
  static uint8_t andar_atual=0;
  static uint8_t proximo_andar=20;
  static uint8_t vetor_destino[16];
  
  static uint8_t i;
  static uint8_t botao_externo_andar = 0;
  static uint8_t sinal_feedback=0;  
  static uint8_t flag_vetor_zero=0;  
  volatile bool flag_mudou_andar=false;  
  
  
  char comando[2];
  char mensagem[10];    
  char andares_letras[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};
  
  //ENVIA RESET
  comando[0]='d'; //Qual elevador
  comando[1]='r';//resetar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
  tx_thread_sleep(1000);

  //FECHAR AS PORTAS
  comando[0]='d';
  comando[1]='f';//fechar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
 

         
  for (;;) {
    tx_queue_receive(&fila_d_id, mensagem, TX_WAIT_FOREVER);
    //TRATAMENTO DOS BOTÕES EXTERNOS 
    if(mensagem[1]=='E'){//botao externo
      sscanf(mensagem,"%*c%*c%d%*c", &botao_externo_andar);  //Auxiliar que recebe o valor do andar do botão externo pressionado 
      vetor_destino[botao_externo_andar]=1;               //Atualiza o vetor de destino do elevador     
      
      //Caso seja a primeira vez
      if((proximo_andar == 20) ||((flag_vetor_zero == 0) && (flag_mudou_andar))){
        flag_mudou_andar = false;
        proximo_andar = botao_externo_andar;
      }          
        if(andar_atual < proximo_andar){
         comando[0]='d'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
      else if(andar_atual > proximo_andar){
         comando[0]='d'; //Qual elevador
         comando[1]='d';//descer
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
   }//if do botão externo    
   else if(mensagem[1]=='I'){ //botao interno     
      for(i=0;i<16;i++){
        if(andares_letras[i]==mensagem[2]){
          vetor_destino[i]=1;
        }
      }
      
      for(i=0;i<16;i++){
          if(vetor_destino[i]==1){
             proximo_andar = i;
          }      
      }
      
      if(andar_atual < proximo_andar){
         comando[0]='d'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='d'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
      
    }//if do botão interno
    
    else if((mensagem[1]>='0' && mensagem[1]<='9') || (mensagem[2]>='0' && mensagem[1]<='2'))//NÃO FUNCIONOU if(!(mensagem[1]=='A' || mensagem[1]=='F')) // If para verificar se é feedback pois A e F é apenas feedback de porta aberta e fechada
    {
      flag_mudou_andar = true;
      
         sscanf(mensagem,"%*c%d", &sinal_feedback);  
         andar_atual = sinal_feedback;

          if(andar_atual == proximo_andar){
           
           vetor_destino[andar_atual]=0; 
           comando[0]='d'; //Qual elevador
           comando[1]='p';//parar
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           
           comando[0]='d'; //Qual elevador
           comando[1]='a';//abrir porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           tx_timer_activate(&timerd_id);
           //**osThreadFlagsWait(0x0003, osFlagsWaitAny, osWaitForever);

          for(i=0;i<16;i++){
              if(vetor_destino[i]==1){
                 proximo_andar = i;
                 flag_vetor_zero = 1;
                 break;
                }else
                  flag_vetor_zero =0;
          }
         }
    }//Fim else do feedback    
    else{
        if(mensagem[1]=='A'){
           comando[0]='d'; //Qual elevador
           comando[1]='f';//fechar porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
        }        
    }
    
    if(flag_vetor_zero == 1){
      if(andar_atual < proximo_andar){
         comando[0]='d'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='d'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }      
    }
  }//for infinito
}

/*----------------------------------------------------------------------------
 *      Thread 3 'elevE': Elevador Esquerdo
 *---------------------------------------------------------------------------*/
void elevE (ULONG thread_input) {
  static uint8_t andar_atual=0;
  static uint8_t proximo_andar=20;
  static uint8_t vetor_destino[16];
  
  static uint8_t i;
  static uint8_t botao_externo_andar = 0;
  static uint8_t sinal_feedback=0;  
  static uint8_t flag_vetor_zero=0;  
  volatile bool flag_mudou_andar=false;  
  
  
  char comando[2];
  char mensagem[10];    
  char andares_letras[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};

  //ENVIA RESET
  comando[0]='e'; //Qual elevador
  comando[1]='r';//resetar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
  tx_thread_sleep(1000);

  //FECHAR AS PORTAS
  comando[0]='e';
  comando[1]='f';//fechar
  tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);

  for (;;) {
    tx_queue_receive(&fila_e_id, mensagem, TX_WAIT_FOREVER);
    //TRATAMENTO DOS BOTÕES EXTERNOS 
    if(mensagem[1]=='E'){//botao externo
      sscanf(mensagem,"%*c%*c%d%*c", &botao_externo_andar);  //Auxiliar que recebe o valor do andar do botão externo pressionado 
      vetor_destino[botao_externo_andar]=1;               //Atualiza o vetor de destino do elevador     
      
      //Caso seja a primeira vez
      if((proximo_andar == 20) ||((flag_vetor_zero == 0) && (flag_mudou_andar))){
        flag_mudou_andar = false;
        proximo_andar = botao_externo_andar;
      }
      
       if(andar_atual < proximo_andar){
         comando[0]='e'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='e'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
   }//if do botão externo    
   else if(mensagem[1]=='I'){ //botao interno     
      for(i=0;i<16;i++){
        if(andares_letras[i]==mensagem[2]){
          vetor_destino[i]=1;
        }
      }
      
      for(i=0;i<16;i++){
          if(vetor_destino[i]==1){
             proximo_andar = i;
          }      
      }
      
      if(andar_atual < proximo_andar){
         comando[0]='e'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='e'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }
      
    }//if do botão interno
    
    else if((mensagem[1]>='0' && mensagem[0]=='c' && mensagem[1]<='9') || (mensagem[2]>='0' && mensagem[1]<='2'))//NÃO FUNCIONOU if(!(mensagem[1]=='A' || mensagem[1]=='F')) // If para verificar se é feedback pois A e F é apenas feedback de porta aberta e fechada
    {
      flag_mudou_andar = true;
      
         sscanf(mensagem,"%*c%d", &sinal_feedback);  
               andar_atual = sinal_feedback;
          if(andar_atual == proximo_andar){
           
           vetor_destino[andar_atual]=0; 
           comando[0]='e'; //Qual elevador
           comando[1]='p';//parar
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           
           comando[0]='e'; //Qual elevador
           comando[1]='a';//abrir porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
           tx_timer_activate(&timere_id);
        //**   osThreadFlagsWait(0x0002, osFlagsWaitAny, osWaitForever);

          for(i=0;i<16;i++){
              if(vetor_destino[i]==1){
                 proximo_andar = i;
                 flag_vetor_zero = 1;
                 break;
                }else
                  flag_vetor_zero =0;
          }
         }
    }//Fim else do feedback    
    else{
        if(mensagem[1]=='A'){
           comando[0]='e'; //Qual elevador
           comando[1]='f';//fechar porta
           tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
        }        
    }
    
    if(flag_vetor_zero == 1){
      
      if(andar_atual < proximo_andar){
         comando[0]='e'; //Qual elevador
         comando[1]='s';//subir
         tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }     
      else if(andar_atual > proximo_andar){
             comando[0]='e'; //Qual elevador
             comando[1]='d';//descer
             tx_queue_send(&fila_escrita_id, comando, TX_WAIT_FOREVER);
      }      
      
      
    }
  }//for infinito
}


/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
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
  tx_thread_create(&tid_elevC, "thread 1", elevC, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
 
  /* Aloca a pilha para a tarefa2. */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa2 com time slice. */
  tx_thread_create(&tid_elevD, "thread 2", elevD, 2, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  
  /* Aloca a pilha para a tarefa3 */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa3 com time slice. */
  tx_thread_create(&tid_elevE, "thread 3", elevE, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  
  /* Aloca a pilha para a tarefa4 */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa4 com time slice. */
  tx_thread_create(&tid_UART_READ, "thread 4", UART_READ, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  
  /* Aloca a pilha para a tarefa5 */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
  /* Cria a tarefa5 com time slice. */
  tx_thread_create(&tid_UART_WRITE, "thread 5", UART_WRITE, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);
  
  /* Cria a fila de mensagens 1*/
  tx_queue_create(&fila_e_id, "queue 1", sizeof(char)*6, pointer, 15);
  
  /* Cria a fila de mensagens 2*/
  tx_queue_create(&fila_d_id, "queue 2", sizeof(char)*6, pointer, 15);
  
  /* Cria a fila de mensagens 3*/
  tx_queue_create(&fila_c_id, "queue 3", sizeof(char)*6, pointer, 15);
  
  /* Cria a fila de mensagens 4*/
  tx_queue_create(&fila_escrita_id, "queue 4", sizeof(char)*6, pointer, 15);
  
  /* Cria o timer 1 */
  tx_timer_create(&timerc_id, "timer 1", callbackc, 0x0001, 300, 0, TX_AUTO_ACTIVATE);
  
  /* Cria o timer 2 */
  tx_timer_create(&timerd_id, "timer 2", callbackd, 0x0002, 300, 0, TX_AUTO_ACTIVATE);
  
  /* Cria o timer 3 */
  tx_timer_create(&timere_id, "timer 3", callbacke, 0x0003, 300, 0, TX_AUTO_ACTIVATE);

  /* devolve o bloco para o pool */
  tx_block_release(pointer);
}



extern void UARTStdioIntHandler(void);

void UARTInit(void){
  
  //Clock do sistema 25MHz
  g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 25000000);
  
    /*-----UART----*/
  // Habilitando a UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))  {/*Espera habilitar a uart0*/}
  
  //Configurando os pinos para o modo UART
  GPIOPinConfigure(GPIO_PA0_U0RX); //pino RX
  GPIOPinConfigure(GPIO_PA1_U0TX); //pino TX
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //seta PA0 e PA1  

  //Número da porta, Baud Rate, Freq da UART
  UARTStdioConfig(0, 115200, g_ui32SysClock);  
  
  
} // UARTInit


void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

void UART_READ(ULONG thread_input){
  
  char msg[6];
  while(1){
    UARTgets(msg,20);
    if(msg[0]=='e'){
      tx_queue_send(&fila_e_id, msg, 0);
    }
    else if(msg[0]=='c'){
      tx_queue_send(&fila_c_id, msg, 0);
    }
    else if(msg[0]=='d'){
      tx_queue_send(&fila_d_id, msg, 0);
    }  
    //osThreadYield();
  } 
}
void UART_WRITE(ULONG thread_input){
  
  
  char msg[2];
  while(1){
    tx_queue_receive(&fila_escrita_id, msg, TX_WAIT_FOREVER);
    UARTprintf("%c%c\n\r",msg[0],msg[1]); 
  } 
} 

void main(void){
  //UART initialization
  UARTInit();
  /* Executar o ThreadX kernel.  */
  tx_kernel_enter();
  
  
} // main