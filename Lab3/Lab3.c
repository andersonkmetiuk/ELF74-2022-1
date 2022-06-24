/*##############################################################################
Anderson Luiz de Souza Kmetiuk
Desenvolvido para a placa EK-TM4C1294XL utilizando o SDK TivaWare no IAR
Lab3 - Histograma
##############################################################################*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "images.c"


uint16_t EightBitHistogram (uint16_t width, uint16_t height, uint8_t * p_image, uint16_t * p_histogram);


int main(void)
{
 
   uint16_t *histograma=(uint16_t*)malloc(sizeof(uint16_t)*256);
   uint16_t a,i;

   //a= EightBitHistogram (width0, height0, (uint8_t*) p_start_image0, histograma); //Image0
   a= EightBitHistogram (width1, height1, (uint8_t*) p_start_image1, histograma); //Image1

  if(a==0) //erro
  {
    printf("VALOR MAIOR QUE 64K\n");
    printf("Imagem invalida!\n");
  }
  else
  {
    printf("Tamanho: %d \n",a);   
    printf("\n\tArquivo CSV\n\n");
    printf("Pixel;Pixel Count\n");
    for(i=0; i<256;i++)
    {
      printf("%u;%u\n",i,histograma[i]);
    }
  }
}