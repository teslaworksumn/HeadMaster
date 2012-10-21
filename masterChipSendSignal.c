/* Master DMX distribution microcontroller code
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		This microcontroller receives a DMX input signal, buffers and distributes the data to 4 
 *		servo control boards via I2C.
 */
 
/*Todo:
 * Verify pragmas
*/

#include <p18f14k50.h>

//config from dmx code ******************************************************
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

void high_isr(void);
void low_isr(void);

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
char RxBuffer[512];
char CountL,CountH; //16-bit counter
char slaveAddress[4]; //TODO: Need to hardcode slave addresses in this array
//******************************************************************************
void setupDMX(void)
{
_asm
; Set PLL on
    bsf     OSCTUNE,PLLEN
    
; Clear the receive buffer
    lfsr    FSR0,RxBuffer
CRxLoop
    clrf    POSTINC0            ;Clear INDF register then increment pointer
    incf    CountL,F
    btfss   STATUS,C
    bra     CRxLoop
    incf    CountH,F

    btfss   CountH,1
    bra     CRxLoop  

; Setup EUSART
    bsf     TRISC,7         ;Allow the EUSART RX to control pin RC7
    bsf     TRISC,6         ;Allow the EUSART TX to control pin RC6

    bsf     BAUDCON,BRG16   ;Enable EUSART for 16-bit Asynchronous operation
    clrf    SPBRGH

    movlw   .31             ;Baud rate is 250KHz for 32MHz Osc. freq.
    movwf   SPBRG
    
    movlw   0x04            ;Disable transmission
    movwf   TXSTA           ;Enable transmission and CLEAR high baud rate

    movlw   0x90
    movwf   RCSTA           ;Enable serial port and reception
_endasm
}

void receiveDMX(void)
{
_asm
MainLoop
    bsf     PORTB,RB0
    bsf     PORTB,RB1
    
; First loop, synchronizing with the transmitter

WaitBreak
    btfsc   RCSTA,FERR
    bra     GotBreak
    btfss   RCSTA,OERR
    bra     WaitBreak
    bcf     RCSTA,CREN
    bsf     RCSTA,CREN

GotBreak
    movf    RCREG,W                 ;Read the Receive buffer to clear the error condition

;Second loop, waiting for the START code
WaitForStart
    btfss   PIR1,RCIF               ;Wait until a byte is correctly received
    bra     WaitForStart
    btfsc   RCSTA,FERR              ;Got a byte
    bra     GotBreak
    movf    RCREG,W

; Check for the START code value, if it is not 0, ignore the rest of the frame
    andlw   0xff
    bnz     MainLoop                ;Ignore the rest of the frame if not zero 
  
; Init receive counter and buffer pointer        
    clrf    CountL
    clrf    CountH
    lfsr    FSR0,RxBuffer

; Third loop, receiving 512 bytes of data
WaitForData
    btfsc   RCSTA,FERR          ;If a new framing error is detected (error or short frame)
    bra     MainLoop            ; the rest of the frame is ignored and a new synchronization
                                ; is attempted

    btfss   PIR1,RCIF           ;Wait until a byte is correctly received
    bra     WaitForData
    movf    RCREG,W

MoveData
    movwf   POSTINC0            ;Move the received data to the buffer 
                                ; (auto-incrementing pointer)
    incf    CountL,F            ;Increment 16-bit counter
    btfss   STATUS,C
    bra     WaitForData
    incf    CountH,F

    btfss   CountH,1            ;Check if 512 bytes of data received
    bra     WaitForData
    return
_endasm
}

/**
 * Send 10 bytes of data from the buffer via I2C to the indicated receiver. 
 * Reciever 0: bytes 0-9
 * Reciever 1: bytes 10-19
 * etc.
**/
void sendI2C(int receiver)
{
	SSPCON2bits.SEN=1;
	while(!PIR1bits.SSPIF) ;
	sendByte(slaveAddress[receiver]);
	while(!PIR1bits.SSPIF) ;
	
	for (int i = 0; i < 10; i++)
	{
		sendByte(RxBuffer[10*receiver + i]);
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

//For testing, remove for final code version
	RxBuffer[0]=7;
	slaveAddress[0] = 40;
//End test
	PIR1bits.SSPIF = 0;

	//INTCON |= 0xC0;

	SSPCON1bits.SSPEN = 1;
}

void main(void)
{
	setup();
	setupDMX();
	while(1)
	{
		receiveDMX();
		for (int receiver = 0; receiver < 4; receiver++)
		{
			sendI2C(receiver);
		};
	}	
}