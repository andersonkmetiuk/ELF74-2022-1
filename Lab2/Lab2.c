/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL utilizando o SDK TivaWare no IAR
Lab2 - Tempo de Reação
##############################################################################*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

//variável que conta os ticks(1ms) = (25k clocks)
unsigned int SysTicks1ms;

//função de tratamento da interrupção do SysTick
void SysTickIntHandler(void)
{
  SysTicks1ms++;
}

//função para configurar e inicializar o periférico Systick a 1ms
void SetupSystick()
{
  SysTicks1ms=0;
  //desliga o SysTick para poder configurar
  SysTickDisable();
  //clock 25MHz <=> SysTick deve contar 25k=1ms  
  SysTickPeriodSet(25000);
  //registra a função de atendimento da interrupção
  SysTickIntRegister(SysTickIntHandler);
  //liga o atendimento via interrupção
  SysTickIntEnable();
  //liga novamente o SysTick
  SysTickEnable();
}

int main(void)
{
  //estado do led
  bool Led1=false,Led4=false;
  //controle via programação de anti-debouncing
  bool bt1flag=false,bt2flag=false;
  //controle de tempo para cada botão
  unsigned int bt1time=0,bt2time=0;
  //variáveis auxiliares
  unsigned int tempo_inicial=0, tempo_final=0;
  bool button1=false,button2=false, resultado_print=false, mudou_led1=false, reset=false;
  
  //executa configuração e inicialização do SysTick
  SetupSystick();  
  //habilitar gpio port N
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  //aguardar o periférico ficar pronto para uso
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {/*Espera habilitar o port*/}
  //habilitar gpio port F
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  //aguardar o periférico ficar pronto para uso
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {/*Espera habilitar o port*/}
  //configura o pin_0 e pin_1 como saída
  //habilitar gpio port J
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
  //aguardar o periférico ficar pronto para uso
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {/*Espera habilitar o port*/}
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  //configura o pin_0 e pin_4 como saída
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
  //configura o pin_0 e pin_1 como entrada
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  //configura os pinos para 2mA como limite de corrente e com week pull-up
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  //loop infinito
  while (1)
        {
          //tempo estourado quando passa mais que 3s
          if( ((SysTicks1ms - tempo_inicial) >= 3000) && (button1==true)) 
          {
              printf("Tempo estourado!\n");
              //altera o estado do led ligado / desligado
              Led4=!Led4;
              //escreve o estado do led no pino
              GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
              button1=false;
              reset=true;
              resultado_print=true;
          }
          
          //ant-debouncig do botão 1
          if (bt1flag) 
	     {
	       //Botão1 liberado !!!
	       if (GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)&&SysTicks1ms>=bt1time)
	          {
	            //botão liberado
                    bt1flag=false;
	            //55ms para liberar estado do botão ... tempo anti-debouncing
                    bt1time=SysTicks1ms+55;				 
	          }
	     }
          else
	     {
	       //botão1 pressionado !!!
	       if ((GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)==0)&&(SysTicks1ms>=bt1time)&&(button1==false))
	          {
                    //contador de tempo
                    tempo_inicial= SysTicks1ms;
                    button1=true;                    
	            //botão pressionado
	            bt1flag=true;
	            //55ms para liberar estado do botão ... tempo anti-debouncing
	            bt1time=SysTicks1ms+55;	
                    
                    if(mudou_led1==false)
                    {
                      //altera o estado do led ligado / desligado
                      Led1=!Led1;
                      //escreve o estado do led no pino
                      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1); 
                      mudou_led1=true;
                    }
                    
                    if(reset==true) //RESET
                    {
                       //altera o estado do led para desligado
                       Led4= false;
                       //escreve o estado do led no pino
                       GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
                       //altera o estado do led para desligado
                       Led1= false;
                       //escreve o estado do led no pino
                       GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1); 
                       
                       printf("RESET\n");
                       tempo_inicial=0;
                       tempo_final=0;
                       button1=false;
                       button2=false;                       
                       reset=false;
                       mudou_led1=false;
                       resultado_print=false;
                    }
	          }
	     }
          
            //ant-debouncig do botão 2
          if (bt2flag) 
	     {
	       //botão2 liberado !!!
	       if (GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1)&&(SysTicks1ms>=bt2time))
	          {
	            //botão liberado
                    bt2flag=false;
	            //55ms para liberar estado do botão ... tempo anti-debouncing
                    bt2time=SysTicks1ms+55;				 
	          }
	     }
          else
	     {
	       //botão2 pressionado !!!
	       if ((GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1)==0)&&(SysTicks1ms>=bt2time)&&(button2==false))
	          {
                    //cálculo do tempo final
                    tempo_final= SysTicks1ms - tempo_inicial;
                    
	            //botão pressionado
	            bt2flag=true;
	            //55ms para liberar estado do botão ... tempo anti-debouncing
	            bt2time=SysTicks1ms+55;				 
                                   
                    if(resultado_print==false)
                    {
                      printf("Tempo total= %d ms \n",tempo_final);
                      //altera o estado do led para desligado
                      Led1= false;
                      //escreve o estado do led no pino
                      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);    
                      tempo_inicial=0;
                      tempo_final=0;
                      button1=false;
                      button2=false;                       
                      reset=false;
                      mudou_led1=false;
                      resultado_print=false;
                    }
                    
                    
	          }
	     }
          
          
        } 
}