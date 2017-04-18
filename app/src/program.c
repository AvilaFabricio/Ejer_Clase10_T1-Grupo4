#include "program.h"   // <= Own header
#include "sapi.h"      // <= sAPI library

#include <math.h>         /* <= Funciones matematicas */
#define CATODO_COMUN 1
//#define ANODO_COMUN 1

// Configuraci?n de pines del display
uint8_t display7SegmentPins_[8] = {
   GPIO5, // a
   GPIO3, // b
   GPIO2, // c
   GPIO8, // d
   GPIO7, // e
   GPIO1, // f
   GPIO4, // g
   GPIO0  // h = dp
   //GPIO6,  // habilitacion 1er segmento
   //LCD1   //habilitaci?n 2 segmento
};

uint8_t display7SegmentOutputs_[25] = {
   0b00111111, // 0
   0b00000110, // 1
   0b01011011, // 2
   0b01001111, // 3
   0b01100110, // 4
   0b01101101, // 5
   0b01111101, // 6
   0b00000111, // 7
   0b01111111, // 8
   0b01101111, // 9

   0b01011111, // a
   0b01111100, // b
   0b01011000, // c
   0b01011110, // d
   0b01111011, // e
   0b01110001, // f

   0b01110111, // A
   0b00111001, // C
   0b01111001, // E
   0b01110110, // H
   0b00011110, // J
   0b00111000, // L
   0b01110011, // P
   0b00111110, // U

   0b10000000  // .
};

void display7SegmentWrite_( uint8_t symbolIndex ){

   uint8_t i = 0;

   for(i=0;i<=7;i++)
#ifdef ANODO_COMUN
        gpioWrite( display7SegmentPins_[i], !(display7SegmentOutputs_[symbolIndex] & (1<<i)) );
#elif defined(CATODO_COMUN)
        gpioWrite( display7SegmentPins_[i], (display7SegmentOutputs_[symbolIndex] & (1<<i)) );
#endif   
}

void display7SegmentPinConfig_( void ){
   gpioConfig( display7SegmentPins_[0], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[1], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[2], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[3], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[4], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[5], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[6], GPIO_OUTPUT );
   gpioConfig( display7SegmentPins_[7], GPIO_OUTPUT );
   //gpioConfig( display7SegmentPins_[9], GPIO_OUTPUT );
   //gpioConfig( display7SegmentPins_[10], GPIO_OUTPUT );
}

// Guarda la ultima tecla apretada
uint16_t key = 0;

/* Pines del teclado matricial */

// Pines conectados a las Filas --> Salidas (MODO = OUTPUT)
uint8_t keypadRowPins[4] = {
   RS232_TXD, // Row 0
   CAN_RD,    // R1
   CAN_TD,    // R2
   T_COL1     // R3
};

// Pines conectados a las Columnas --> Entradas con pull-up (MODO = INPUT_PULLUP)
uint8_t keypadColPins[4] = {
   T_FIL0,    // Column 0
   T_FIL3,    // C1
   T_FIL2,    // C2
   T_COL0     // C3
};


// Vector para mostrar tecla presionada por UART
uint16_t asciiKeypadKeys[16] = {
                                '1', '2', '3', 'A',
                                '4', '5', '6', 'B',
                                '7', '8', '9', 'C',
                                '*', '0', '#', 'D'
                               };

// Vector para mostrar tecla presionada en el display 7 segmentos
uint16_t keypadKeys[16] = {
                               1,    2,    3, 0x0a,
                               4,    5,    6, 0x0b,
                               7,    8,    9, 0x0c,
                            0x0e,    0, 0x0f, 0x0d
                          };
                          
                          

void configurarTecladoMatricial( void ){
   
   uint8_t i = 0;
   
   // Configure Rows as Outputs
   for( i=0; i<4; i++ ){
      gpioConfig( keypadRowPins[i], GPIO_OUTPUT );
   }

   // Configure Columns as Inputs with pull-up resistors enable
   for( i=0; i<4; i++ ){
      gpioConfig( keypadColPins[i], GPIO_INPUT_PULLUP );
   }
}


/* Devuelve TRUE si hay alguna tecla presionada o FALSE (0) en caso contrario.
 * Si hay tecla presionada guarda el valor en la variable key.
 * El valor es un numero de indice entre 0 y 15 */
bool_t leerTecladoMatricial( void ){

   bool_t retVal = FALSE;

   uint16_t r = 0; // Rows
   uint16_t c = 0; // Columns

   // Poner todas las filas en estado BAJO
   for( r=0; r<4; r++ ){
	  gpioWrite( keypadRowPins[r], LOW );
   }

   // Chequear todas las columnas buscando si hay alguna tecla presionada
   for( c=0; c<4; c++ ){

      // Si leo un estado BAJO en una columna entonces puede haber una tecla presionada
      if( !gpioRead( keypadColPins[c] ) ){

         delay( 50 ); // Anti-rebotes de 50 ms

         // Poner todas las filas en estado ALTO excepto la primera
         for( r=1; r<4; r++ ){
            gpioWrite( keypadRowPins[r], HIGH );
         }

         // Buscar que tecla esta presionada
         for( r=0; r<4; r++ ){

            // Poner la Fila[r-1] en estado ALTO y la Fila[r] en estado BAJO
            if( r>0 ){ // Exceptua el indice negativo en el array
               gpioWrite( keypadRowPins[r-1], HIGH );
            }
            gpioWrite( keypadRowPins[r], LOW );

            // Chequear la Columna[c] en Fila[r] para buscar si la tecla esta presionada
            // Si dicha tecla esta oresionada (en estado BAJO) entonces retorna
            // graba la tecla en key y retorna TRUE
            if( !gpioRead( keypadColPins[c] ) ){
               retVal = TRUE;
               key = r * 4 + c;
               /*
                  Formula de las teclas de Teclado Matricial (Keypad)
                  de 4 filas (rows) * 5 columnas (columns)

                     c0 c1 c2 c3 c4
                  r0  0  1  2  3  4
                  r1  5  6  7  8  9   Si se presiona la tecla r[i] c[j]:
                  r2 10 11 12 13 14   valor = (i) * cantidadDeColumnas + (j)
                  r3 15 16 17 18 19
               */
               return retVal;
            }
         }

      }
   }
   return retVal;
}
   

int main(void){

   boardConfig();
   gpioConfig(GPIO6, GPIO_OUTPUT);   // catodo comun del primer digito
   gpioConfig(LCD1, GPIO_OUTPUT);    // " del segundo digito
   gpioConfig(LCD2, GPIO_OUTPUT);    // " del tercer digito
   gpioConfig(LCD3, GPIO_OUTPUT);   // " del cuarto digito
   display7SegmentPinConfig_();    
   configurarTecladoMatricial(); // Configurar teclado matricial
   
   uint8_t k = 0;  //indice primer display 
   uint8_t i = 0;  //indice segundo display
   uint8_t m = 0;  //indice tercer display 
   uint8_t n = 0;  //indice cuarto display
   uint8_t j = 0;  // indice tiempo de cuenta

   while(1) {
        
  //---     if(k==10)k=0;
        
  //---      if(j==24){  i++;
  //---                  j=0;}
        
        display7SegmentWrite_(k);    // primer display
        gpioWrite(GPIO6,OFF);
        gpioWrite(LCD1,ON);
        gpioWrite(LCD2,ON);
        gpioWrite(LCD3,ON);
        delay(5);
        
        display7SegmentWrite_(i);    // segundo display
        gpioWrite(GPIO6, ON);
        gpioWrite(LCD1,OFF);
        gpioWrite(LCD2,ON);
        gpioWrite(LCD3,ON);
        delay(5);
        
        display7SegmentWrite_(m);    // tercer display
        gpioWrite(GPIO6,ON);
        gpioWrite(LCD1,ON);
        gpioWrite(LCD2,OFF);
        gpioWrite(LCD3,ON);
        delay(5);

        display7SegmentWrite_(n);    // cuarto display
        gpioWrite(GPIO6,ON);
        gpioWrite(LCD1,ON);
        gpioWrite(LCD2,ON);
        gpioWrite(LCD3,OFF);
        delay(5);        

       if( leerTecladoMatricial() ){
         k = i;
         i = m;
         m = n;
         n = keypadKeys[key];
         //uartWriteByte( UART_USB, asciiKeypadKeys[key] );
         
      }



   }
   return 0 ;
}