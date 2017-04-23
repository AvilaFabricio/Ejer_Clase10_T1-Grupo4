#include "program.h"   // <= Own header
#include "sapi.h"      // <= sAPI library
#include "display7segment.h"
#include "keypad4x4.h"
#include <math.h>         /* <= Funciones matematicas */

delay_t digitChange;
delay_t delayRefresh;

uint8_t display4S[5]={11,18,25,23};

uint16_t tecl_pres = 0;
uint16_t keypadKeys[16] = {
                               1,    2,    3, 0x0a,
                               4,    5,    6, 0x0b,
                               7,    8,    9, 0x0c,
                            0x0e,    0, 0x0f, 0x0d
                          };     
       

int main(void){

   boardConfig();
   display7SegmentPinConfig_();
   delayConfig(&delayRefresh, 3);
   delayConfig(&digitChange, 500); 
   configurarTecladoMatricial(); // Configurar teclado matricial
   
   
  

   while(1) {
        
   

   
       if(delayRead(&delayRefresh))
            selectDigit(&display4S[0]);
   
  
       tecl_pres =leerTecladoMatricial();
       if(tecl_pres!=99){
    
            display4S[0]=display4S[1];
            display4S[1]=display4S[2];
            display4S[2]=display4S[3];
            display4S[3] = keypadKeys[tecl_pres];
         
       }


        
  
   }
   return 0 ;
}
