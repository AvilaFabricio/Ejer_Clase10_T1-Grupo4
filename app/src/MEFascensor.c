#include "sapi.h"
#include "MEFascensor.h"

typedef enum{ EN_ESPERA, SUBIENDO, BAJANDO, ABRIENDO_PUERTA, PUERTA_ABIERTA, CERRANDO_PUERTA, PUERTA_CERRADA, MODO_CONFIGURACION, LEER_PISO,ALARMA_P_ABIERTA } estado_ascensor;

estado_ascensor estadoActual;

// variables para la MEF del ascensor
uint8_t tiempoTranscurrido = 0;
uint8_t ascensorsubiendo = 0;
uint8_t pisoactual[2] = {0,0};
uint8_t pisoseteado [2] ={0,0};
//uint8_t k = 0;  //indice primer display 
//uint8_t i = 0;  //indice segundo display
//uint8_t m = 0;  //indice tercer display 
//uint8_t n = 0;  //indice cuarto display


void puertaAbierta( void ) {
   gpioWrite( LEDG, ON );
   gpioWrite( LED1, OFF );
   gpioWrite( LED2, OFF );
}

void ascensorMoviendose( void ) {
   gpioWrite( LEDG, ON );
   gpioWrite( LED1, OFF );
   gpioWrite( LED2, OFF );
}
void puertaAbriendose( void ) {
   gpioWrite( LEDG, OFF );
   gpioWrite( LED1, ON );
   gpioWrite( LED2, OFF );
}

void puertaCerrandose( void ) {
   gpioWrite( LEDG, OFF );
   gpioWrite( LED1, OFF );
   gpioWrite( LED2, ON );
}

void enEspera( void ) {
   gpioWrite( LED1, OFF );
   gpioWrite( LED3, ON );
   gpioWrite( LED2, OFF );
}

void InicializarMEF( void ) {
   estadoActual =EN_ESPERA;
}
void ActualizarMEF( void ) {
   switch(estadoActual){
      case EN_ESPERA:
      //uartWriteString(UART_USB, texto);
       if (!gpioRead(TEC2)){
         ascensorsubiendo=1;
         estadoActual = ABRIENDO_PUERTA;
       }
       if (!gpioRead(TEC3)){
         estadoActual = ABRIENDO_PUERTA;
         ascensorsubiendo=0;
       }
         
      break;
      case SUBIENDO:
       //uartWriteString(UART_USB, texto5);
       ascensorMoviendose();
       if( tiempoTranscurrido == 2 ){
          if ((pisoactual[1]==pisoseteado[1]) & (pisoactual[0]==pisoseteado[0])) {
             estadoActual = ABRIENDO_PUERTA;
             
          }
          else {
           //n++;
           //if (n==10){
            //n=0;
            //m++;
           //}
          
          }      
            tiempoTranscurrido = 0;
       }
      break;
      case BAJANDO:
       //uartWriteString(UART_USB, texto6);
       ascensorMoviendose();
       if( tiempoTranscurrido == 2 ){
          if ((pisoactual[1]==pisoseteado[1]) & (pisoactual[0]==pisoseteado[0])) {
             estadoActual = ABRIENDO_PUERTA;
             
          }
          else {
           //n--;
           //if (n==0){
            //n=9;
            //m--;
           //}
          
          }      
            tiempoTranscurrido = 0;
       }
      break;
      case ABRIENDO_PUERTA:
       //uartWriteString(UART_USB, texto1);
        puertaAbriendose();
        tiempoTranscurrido++;
        if( tiempoTranscurrido == 2 ){
            estadoActual = PUERTA_ABIERTA;
            tiempoTranscurrido = 0;}
      break;
      case PUERTA_ABIERTA:
       //uartWriteString(UART_USB, texto2);
        tiempoTranscurrido++;
        if( tiempoTranscurrido == 2 ){
            estadoActual = CERRANDO_PUERTA;
            tiempoTranscurrido = 0;
        }    
      break;
      case CERRANDO_PUERTA:
       //uartWriteString(UART_USB, texto3);
        puertaCerrandose();
        tiempoTranscurrido++;
        if( tiempoTranscurrido == 2 ){
           while (!gpioRead(TEC1)){} //simula persona u objeto en la puerta
           estadoActual = PUERTA_CERRADA;
           tiempoTranscurrido = 0;
        }
      break;
      case PUERTA_CERRADA:
       //uartWriteString(UART_USB, texto4);
       
       if (pisoseteado[1]<pisoactual[1]){
         estadoActual = SUBIENDO;
       }
       else if(pisoseteado[1]>pisoactual[1]){
         estadoActual = BAJANDO;
       }
       else if(pisoseteado[1]==pisoactual[1]){
         if (pisoseteado[0]<pisoactual[0]){
         estadoActual = SUBIENDO;
         }
         else if(pisoseteado[0]>pisoactual[0]){
         estadoActual = BAJANDO;
         }
       }
      break;
      case MODO_CONFIGURACION:
         
      break;
      case LEER_PISO:
       //pisoseteado = lectura del teclado matricial   
       if(pisoactual<pisoseteado){
       estadoActual = SUBIENDO;
       }
       else {estadoActual = BAJANDO;}
      break;
      case ALARMA_P_ABIERTA:
         
      break;
      default:
         InicializarMEF();
      break;
   } 
}