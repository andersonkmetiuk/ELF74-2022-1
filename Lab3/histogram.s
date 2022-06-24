; Desenvolvido para a placa EK-TM4C1294XL
; Anderson Kmetiuk
;15/06/2022
; -------------------------------------------------------------------------------
        PUBLIC EightBitHistogram
        SECTION .text : CODE (2) 
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
        EXPORT EightBitHistogram ;exportar a função pro c
; -------------------------------------------------------------------------------
;parâmetros do C
;uint16_t EightBitHistogram (uint16_t width, uint16_t height, uint8_t * p_image, uint16_t * p_histogram);
;parâmetros em R0 a R3

EightBitHistogram  
    MUL R0, R1 ;R0=width*height -> tamanho
    ;verifica se o tamanho passa de 65.536 pixels
    MOV R5, #65536
    MOV R9, #0
    MOV R6,#0
    CMP R0,R5
    BLE Zera ;LE = less than or equal (<=64K)
    BGT Erro ;GT=greater than (>64K)  

Zera
    STR R9,[R3,R6]
    ADD R6,#1
    CMP R6, #512 
    BEQ Programa
    BNE Zera
   
;varredura do histograma
;Registradores reservados: R0 -> Tamanho, R2 -> imagem, R3 -> histograma 
;R1:pos ref img , R4: pos ref histograma, R5: contador de pixels, R6:pixel atual , R7:variável para cópia
Programa
    MOV R1, #0 ;pos ref img
    MOV R4, #0 ;pos ref histograma
    MOV R5, #0 ;contador de pixels
    MOV R6, #0 ;pixel atual
  
Loop
    LDRB R7,[R2,R1] ;copia o conteúdo do vetor imagem
    CMP R6, R7 ;compara para ver se o pixel atual é igual
    BEQ Soma ;se for igual soma 1
    ADD R1, #1 ;Próximo valor da lista img
    CMP R1, R0 ;ve se chegou ao fim do tamanho do vetor img
    BEQ ProximoPixel
    B Loop
    
ProximoPixel
    STR R5, [R3,R4] ;guarda a qtd do pixel em questão 
    MOV R5, #0 ;zera a contagem de pixel
    ADD R6, #1 ;proximo pixel a ser analisado
    MOV R1, #0 ;zera pos do vetor img
    ADD R4, #2 ;proximo end
    CMP R4, #512 ;1 valor a cada 2 posições
    BEQ Exit
    B Loop
    
Soma
    ADD R5, #1 ;Soma 1 na contagem do pixel
    ADD R1,#1 ;Próximo valor da lista img
    CMP R1, R0 ;ve se chegou ao fim do tamanho do vetor img
    BEQ ProximoPixel
    B Loop
  
Erro
    MOV R0, #0
    BAL Exit ;AL=always

Exit
    MOV R4, R3 ;****por algum motivo o R4 armazena o end do ponteiro pro retorno em C
    BX LR
; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------  
  END