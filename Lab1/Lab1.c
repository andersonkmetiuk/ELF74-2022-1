/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL utilizando o SDK TivaWare no IAR
Lab1 - Introdução
##############################################################################*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void main(void)
{
  float n,a=15.25;
  time_t tempo;
  time(&tempo);
  printf("\t \t ELF74 LAB 1 \n \n");  
  printf("Data e hora do sistema: %s \n \n", ctime(&tempo));
  printf("Digite um numero: ");
  scanf("%f", &n);
  printf("%.3f \n \n \n",n);
  printf("%.3f + %.3f = %.3f \n \n \n",n,a ,(n+a));
  printf("\t \t --- FIM --- \n");
  
}