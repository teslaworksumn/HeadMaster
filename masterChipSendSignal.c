/* Master DMX distribution microcontroller code
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		This microcontroller receives a DMX input signal, buffers and distributes the data to 4 
 *		servo control boards via I2C.
 */
 
/*Todo:
 * Verify pragmas
*/

#include <p18f2620.h>
#include DMXlib.h

//config from dmx code ******************************************************
/*
#pragma config CPUDIV = NOCLKDIV
#pragma config USBDIV = OFF
#pragma config FOSC = HS
#pragma config PLLEN = ON
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRTEN = OFF
#pragma config BOREN = OFF
#pragma config BORV = 30
#pragma config WDTEN = OFF
#pragma config WDTPS = 32768
#pragma config MCLRE = ON
#pragma config HFOFST = OFF
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config XINST = OFF
#pragma config BBSIZ = OFF
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CPB = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTRB = OFF
#pragma config DEBUG = OFF
 */

void high_isr(void);
void low_isr(void);

void sendI2C(int receiver);
void sendByte(char data);

#pragma code high_isr_entry=8
void high_isr_entry(void)
{
    _asm goto high_isr _endasm
};

#pragma code low_isr_entry=0x18
void low_isr_entry(void)
{
    _asm goto low_isr _endasm
};
#pragma code

#pragma interrupt high_isr
void high_isr(void)
{
}

#pragma interruptlow low_isr
void low_isr(void)
{
}
//*****************************************************************************
char slaveAddress[4]; //TODO: Need to hardcode slave addresses in this array
const int BYTES_PER_SLAVE = 8; //TODO: Change to number of DMX channels used per slave
//******************************************************************************

/**
 * Send BYTES_PER_SLAVE bytes of data from the DMXBuffer via I2C to the indicated receiver. 
 * Reciever 0: bytes 0 to (BYTES_PER_SLAVE - 1)
 * Reciever 1: bytes BYTES_PER_SLAVE to (2*BYTES_PER_SLAVE - 1)
 * etc.
**/
void sendI2C(int receiver)
{
    int i = 0;

    SSPCON2bits.SEN=1;
    while(!PIR1bits.SSPIF) ;
    sendByte(slaveAddress[receiver]);
    while(!PIR1bits.SSPIF) ;

    for (i = 0; i < 10; ++i)
    {
        sendByte(DMXBuffer[10*receiver + i]);
        while(!PIR1bits.SSPIF) ;
    };

    SSPCON2bits.PEN=1;
    PIR1bits.SSPIF = 0;
}
				 
/* Adds a single byte of data to the send buffer, does not wait for send completion to return
 */
void sendByte(char data)
{
    SSPBUF = data;
    PIR1bits.SSPIF = 0;
}

void setup(void)
{
    TRISBbits.RB4 = 1;//SDA
    TRISBbits.RB6 = 1;//SCL
	//LATBbits.LATB4 = 1;

    //set up MSSP for master mode
    SSPSTATbits.SMP = 1;
    SSPSTATbits.CKE = 1;

    SSPCON1 = 0b00001000;

    SSPCON2 = 0b00000000;
	
    //Baud = Fosc/(4*SSPADD+1) = ~114kHz when Fosc @ 12MHz
    SSPADD = 100;

    DMXBuffer[0]=7;
    slaveAddress[0] = 40;
    PIR1bits.SSPIF = 0;

    //INTCON |= 0xC0;

    SSPCON1bits.SSPEN = 1;
}

void main(void)
{
    int receiver = 0;

    setup();
    DMXSetup();
    while(1)
    {
		DMXReceive();
        for (receiver = 0; receiver < 4; ++receiver)

        {
            sendI2C(receiver);
        };
    }
}
