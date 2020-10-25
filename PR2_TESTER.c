/*
 * File:   PR2_TESTER.c
 * Author: AXM
 *
 * Created on September 7, 2020, 6:31 PM
 */

// <editor-fold defaultstate="collapsed" desc="LIBRERIAS">
#include <xc.h>
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PALABRA DE CONFIGURACION">
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DEFINES RETARDOS">
#define _XTAL_FREQ 32000000     //FRECUENCIA DE OPERACION 32MHZ
#define retardo_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))  //DEFINICION RETARDO MILISEGUNDOS
#define retardo_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))  //DEFINICION RETARDO MICROSEGUNDOS
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DEFINES DE PINES">
#define LED LATCbits.LATC0  //LED DE TARJETA A PIN RC0
#define ENCODER PORTAbits.RA4   //SEÑAL DE ENCODER OPTICO A PIN RA4/T0CKI TIMER 0 CLOCK INPUT
#define VARIADOR PORTBbits.RB1  //SEÑAL DE POT LINEAL A PIN RB1/AN10 CANAL ANALOGICO NO.10
#define PWMA LATBbits.LATB5 //SEÑAL PWM DE SALIDA PARA VARIAR VELOCIDAD MOTOR A PIN RB5/CCP3
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="LIBRERIAS PROPIAS">
#include "ADC.h"        //LIBRERIA PROPIA ADC (ANALOG TO DIGITAL CONVERTER)
#include "COMUNICACION_SERIAL.h"    //LIBRERIA PROPIA PARA HACER COMUNICACION SERIAL (USB) CON COMPUTADOR O TELEFONO MOVIL
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROTOTIPADO DE VARIABLES">
//(INT -32,768 A 32,768)(CHAR -128 A 127)
//(UNSIGNED INT 0 A (2^16)-1 :65,535 COMBINACIONES)(UNSIGNED CHAR 0 A (2^8)-1 :255 COMBINACIONES)
//(UNSIGNED LONG 0 A (2^32)-1 :4,294,967,296 COMBINACIONES)
unsigned char dato;	//DATO SERIAL RECIBIDO
char contador_rx=0;	//CONTADOR RX RECEPTOR
char cadena_rx[7];	//CADENA/VECTOR CARACTERES RX RECEPTOR
unsigned long rpms=0;   //CONTADOR DE REVOLUCIONES POR MINUTO
unsigned int freqcomp=0;    //COMPARADOR DE FREQ
unsigned int freq=0;    //VARIABLE QUE ALOJA LA FREQ SELECCIONADA
unsigned char flag=0;   //BANDERA DE ESPERA USUARIO

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROTOTIPADO DE FUNCIONES">
void sistema_ini(void); //FUNCION PARA INICIALIZAR SISTEMA BASICO (ENTRADAS Y SALIDAS, PUERTOS, OSCILADOR, ETC)
void PWM_A(unsigned int duty, unsigned int fpwm);   //FUNCION PARA CONTROLAR CICLO DE TRABAJO % Y FRECUENCIA PWM PARA MOTOR
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROGRAMA PRINCIPAL">
void main(void) {
    sistema_ini();
    INICIALIZA_COMUNICACION_SERIAL9600();
    INICIALIZA_ADC('I',0,10);   //ADC 10 BITS, REFERENCIA INTERNA, REFERENCIA FIJA
    CONFIG_ADC(10); //USAMOS CANAL 10 AN10=RB1
    retardo_ms(500);
    
    ENVIAR_DATO(0x0D);
    ENVIAR_CADENA("TESTER PARA MOTORES");
    ENVIAR_DATO(0x0D);
    ENVIAR_CADENA("INTRODUCE FREQ DE PWM:");
    while(flag==0); //ESPERAMOS HASTA QUE USUARIO INGRESE FREQ (DELIMITADA ENTRE 500HZ Y 50KHZ)
    // <editor-fold defaultstate="collapsed" desc="CICLO INFINITO">
    while (1)
    {
        
        ENVIAR_CADENA("LECTURA_ADC:");
        PWM_A(LEER_ADC(10,10),freq);
        ENVIAR_NUMERO(LEER_ADC(10,10),10);
        ENVIAR_DATO(0x0D);              //SETEO DE LECTURA ADC
        ENVIAR_CADENA("CICLO DE TRABAJO:");
        ENVIAR_NUMERO(LEER_ADC(10,10)*0.097,10);
        ENVIAR_CADENA(" %");
        ENVIAR_DATO(0x0D);              //SETEO DE DUTY CYCLE %
        ENVIAR_CADENA("RPMS:");
        rpms=TMR0*60*2/20;              //FORMULA PARA OBTENER RPMS, RPMS=MUESTRA DE PULSOS POR SEG*60/LINEAS ENCODER
        TMR0=0;                         //RESET DE TMR0
        ENVIAR_NUMERO(rpms,10);
        ENVIAR_CADENA(" RPMS");
        ENVIAR_DATO(0x0D);              //SETEO DE RPMS
        retardo_ms(450);                
       
               
    }
     // </editor-fold>
    return;
}
   

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="INTERRUPCIÓN">
void __interrupt() my_isr(void){
     if (PIR1bits.RCIF)
    {   
      PIR1bits.RCIF=0;      //REESTABLECEMOS BANDERA INTERRUPCION POR RECEPCION SERIAL
      dato=RCREG;           //CARGAMOS EL CONTENIDO DE REGISTRO RECEPTOR A DATO
      if(dato==0x0D)
      {
          flag=1;           //BANDERA ESPERA USUARIO ACTIVADA
          freqcomp=atoi(cadena_rx);     //CONVERSION ARRAY TO INTEGER DE VECTOR DE DATOS INGRESADO
          if(freqcomp<=50000 && freqcomp>=500)  //DELIMITAMOS SOLO FRECUENCIAS ENTRE 500HZ Y 50KHZ
          {
            freq=freqcomp;
            ENVIAR_DATO(0x0d);
            ENVIAR_CADENA("FRECUENCIA:");
            ENVIAR_CADENA(cadena_rx);
            ENVIAR_CADENA("Hz");
            ENVIAR_DATO(0x0d);
            
            
          }
       
          else{         //FRECUENCIA FUERA DE INTERVALO O COMBINACION DE DATOS NO PERMITIDA
             
             ENVIAR_DATO(0x0d);
             ENVIAR_CADENA("FRECUENCIA FUERA DE RANGO");
             ENVIAR_DATO(0x0d);
     
          }
          for (char i=0;i<7;i++)
          {
              cadena_rx[i]=0;           //CORRIMIENTO DE LOS DATOS RECIBIDOS A LO LARGO DEL VECTOR RX
          }
          contador_rx=0;
      }
      else
      {
      cadena_rx[contador_rx]=dato;
      contador_rx++;                //INCREMENTO DE POSICION DE ESCRITURA DE DATO RECIBIDO A LO LARGO DEL VECTOR RX
      }
      retardo_ms(600);          //RETARDO POR DIGITO INGRESADO
    }
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="FUNCIONES">
void sistema_ini(void)
{
    OSCCON=0b01110000; /*COMBINACION PARA 8 MHZ O 32 MHZ DE FRECUENCIA YA CON PLL*/
    APFCON=0B01000000; //ALTERNATE PIN FUNCTION REG MODIFICAMOS PARA QUE CCP3 ESTE EN RB5
    LATA=0;             //ESTADOS BAJOS PUERTO A
    LATB=0;             //ESTADOS BAJOS PUERTO B
    LATC=0;             //ESTADOS BAJOS PUERTO C
    TRISA=0B00010000;   //T0CKI EN RA4 ES ENTRADA
    TRISB=0b00000010;   //RB1 ENTRADA ANALOGICA, RB5 SALIDA PWM 
    TRISC=0;   /*CONFIGURADO COMO SALIDA NO USADO*/
    ANSELA=0;
    ANSELB=0;  
    OPTION_REG=0xF8;    //OPTION REG CONFIGURADO PARA USAR RA4/T0CKI COMO ENTRADA DE RELOG TMR0 (CONTADOR DE EVENTOS EXTERNOS)
}

void PWM_A(unsigned int duty, unsigned int fpwm){
   
  
    unsigned int prescalador=1;
    unsigned long PR2cal=(_XTAL_FREQ) ;
    PR2cal=PR2cal/fpwm;
    PR2cal=PR2cal/4;  
    T2CON=0b00000000;
    while(PR2cal>256){
       prescalador=prescalador*4;
       T2CON++;
       PR2cal=PR2cal/4;
    }
    PR2cal--;
    PR2=PR2cal;
    
    unsigned long AuxCCPR3L=PR2cal;
    AuxCCPR3L++;
    AuxCCPR3L=AuxCCPR3L*duty;
    AuxCCPR3L=AuxCCPR3L*4;
    AuxCCPR3L=AuxCCPR3L/1024;
    unsigned int AuxCCP3CON= AuxCCPR3L;
    AuxCCP3CON= AuxCCP3CON & 0b00000011 ;
    AuxCCP3CON= AuxCCP3CON*16;//<< 4
    CCP3CON= 0b00001100;
    CCP3CON= CCP3CON|AuxCCP3CON;
    AuxCCPR3L=AuxCCPR3L/4; //>>2
    CCPR3L=AuxCCPR3L;
    T2CONbits.TMR2ON=1;
    TMR2=0;       
}


// </editor-fold>

