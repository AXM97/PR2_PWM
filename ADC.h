/* 
 * File:   ADC.h
 * Author: AXM
 *
 * Created on 28 de agosto de 2020 10:51 AM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
//INICIALIZA_ADC REQUIERE VALORES E DE EXTERNO O I DE INTERNO VSS, 0 PARA VDD E PARA EXTERNO 4 PARA 4V 2 PARA 2V 1 PARA 1V
//LEER_ADC REQUIERE VALORES DE CANAL 0 A 13 OMITIENDO 5 5 6 Y 7, Y CUANTOS BITS DE RESOLUCION 8 O 10
//CONFIG_ADC REQUIERE VALORES DE CANAL 0 A 13 OMITIENDO 5 5 6 Y 7 PARA CONFIGURAR COMO ENTRADAS ANALOGICAS
void INICIALIZA_ADC(char ref_neg,char ref_pos,char just);
unsigned int LEER_ADC(char canal,char bits);
void CONFIG_ADC(char canal);
void CONFIG_ADC(char canal)
{
    switch (canal)
    {
        case 0:
            TRISAbits.TRISA0=1;
            ANSELAbits.ANSA0=1;
            break;
        case 1:
            TRISAbits.TRISA1=1;
            ANSELAbits.ANSA1=1;
            break;
        case 2:
            TRISAbits.TRISA2=1;
            ANSELAbits.ANSA2=1;
            break;
        case 3:
            TRISAbits.TRISA3=1;
            ANSELAbits.ANSA3=1;
            break;
        case 4:
            TRISAbits.TRISA5=1;
            ANSELAbits.ANSA5=1;
            break;
        case 8:
            TRISBbits.TRISB2=1;
            ANSELBbits.ANSB2=1;
            break;
        case 9:
            TRISBbits.TRISB3=1;
            ANSELBbits.ANSB3=1;
            break;
        case 10:
            TRISBbits.TRISB1=1;
            ANSELBbits.ANSB1=1;
            break;
        case 11:
            TRISBbits.TRISB4=1;
            ANSELBbits.ANSB4=1;
            break;
        case 12:
            TRISBbits.TRISB0=1;
            ANSELBbits.ANSB0=1;
            break;
        case 13:
            TRISBbits.TRISB5=1;
            ANSELBbits.ANSB5=1;
            break;
       
    }
            
}
unsigned int LEER_ADC(char canal,char bits)
{
   canal=canal<<2;
   canal=canal+1;
   ADCON0=canal;
   retardo_us(100);
   ADCON0bits.GO=1;
   while (ADCON0bits.GO);
   if (bits==10)
       return ADRES;
   else if(bits==8)
       return ADRESH;
   else
       return 0xFFFF;
}
void INICIALIZA_ADC(char ref_neg,char ref_pos,char just)
    {
        ADCON0=0;
        ADCON1=0;
        if(just==8) ADCON1bits.ADFM=0;
        if(just==10) ADCON1bits.ADFM=1;
        ADCON1|=0b00100000;
        if(ref_neg=='E') ADCON1bits.ADNREF=1;
        else if(ref_neg=='I') ADCON1bits.ADNREF=0;
        switch (ref_pos)
        {
            case 0:
                FVRCON=0;
                break;
            case 'E':
                ADCON1|=0b00000010;
                FVRCON=0;
                
                break;
            case 4:
                ADCON1|=0b00000011;
                FVRCON=0b11000011;
                break;
            case 2:
                ADCON1|=0b00000011;
                FVRCON=0b11000010;
                break;
            case 1:
                ADCON1|=0b00000011;
                FVRCON=0b11000001;
                break;
                
        }
        ADCON0bits.ADON=1;
    }


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

