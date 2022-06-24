/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL utilizando o SDK TivaWare no IAR
Lab4 - Comunicação Serial
##############################################################################*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

//variável do clock do sistema
uint32_t g_ui32SysClock;

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

void Setup_Inicial()
{
  /*
  --- PORTS ---
    LEDs - PN0,PN1,PF0,PF4
    UART -PA0(RX), PA1(TX)
  -------------
  */
  
  //Clock do sistema 25MHz
  g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 25000000);
  
  //Habilita os Ports
  //GPIO port N
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {/*Espera habilitar o port*/}
  
  //GPIO port F
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {/*Espera habilitar o port*/}    

  //GPIO port A
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {/*Espera habilitar o port*/}
  
  //Define como entrada ou saída
  //SAÍDAS
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); //lED PN0,PN1
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); //LED PF0,PF4
  
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
  
}


/*-----------------------MAIN ------------------*/
int main(void)
{
  //variaveis
  char msg[5]; //recebe msg na UART
  bool Led1=false,Led2=false,Led3=false,Led4=false; //estado dos leds
   
  //RODA AS INICIALIZAÇÕES
  SetupSystick();
  Setup_Inicial();
  
  //loop infinito
  while (1)
        {
          //Espera o comando ser enviado na UART
          UARTgets(msg,5);
          
          if(msg!= NULL) //verifica se tem algo na UART
          {
            //Validação do comando recebido
            if(msg[0]=='#')        
            {
              if (msg[1]=='R' || msg[1]=='T')
              {
                if (msg[2]=='1'||msg[2]=='2'||msg[2]=='3'||msg[2]=='4'||msg[2]=='X') 
                {
                  if(msg[3]=='0' || msg[3]=='1')
                  {
                    //Relé 1 
                    if(msg[2]=='1')
                    {
                      if(msg[3]=='1') //#R11 ligar 
                      {
                        Led1=true;
                        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
                      }
                      else // #R10 desligar 
                      {
                        Led1=false;
                        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
                      }              
                    }
                    
                    //Relé 2 
                    if(msg[2]=='2')
                    {
                      if(msg[3]=='1') //#R21 ligar 
                      {
                        Led2=true;
                        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
                      }
                      else // #R20 desligar 
                      {
                        Led2=false;
                        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
                      }              
                    }
                    
                    //Relé 3 
                    if(msg[2]=='3')
                    {
                      if(msg[3]=='1') //#R31 ligar 
                      {
                        Led3=true;
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
                      }
                      else // #R30 desligar 
                      {
                        Led3=false;
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
                      }              
                    }
                    
                    //Relé 4 
                    if(msg[2]=='4')
                    {
                      if(msg[3]=='1') //#R41 ligar 
                      {
                        Led4=true;
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
                      }
                      else // #R40 desligar 
                      {
                        Led4=false;
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
                      }              
                    }
                    
                    //TODOS JUNTOS
                    if(msg[1]=='T')
                    {
                      if(msg[2]=='X')
                      {
                        if(msg[3]=='1') //#TX1 ligar 
                        {
                          Led1=true;
                          Led2=true;
                          Led3=true;
                          Led4=true;
                          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
                          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
                          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
                          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
                        }
                        else // #TX0 desligar 
                        {
                          Led1=false;
                          Led2=false;
                          Led3=false;
                          Led4=false;
                          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, Led1<<1);
                          GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, Led2);
                          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, Led3<<4);
                          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led4);
                        } 
                      
                      }
                    }
                    //resposta
                    msg[0]='@';
                    UARTprintf(msg);
                    UARTprintf("\n"); 
                    
                  }//1 ou 0
                  else {UARTprintf("Comando Invalido \n");}
                }//1,2,3,4 ou X
                else {UARTprintf("Comando Invalido \n");}
              }//R ou T
              else {UARTprintf("Comando Invalido \n");}
            }//#
            else {UARTprintf("Comando Invalido \n");}
          }//tem algo?
 
        } //WHILE
} //MAIN