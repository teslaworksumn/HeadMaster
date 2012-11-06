/*
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		Functions for use with PIC18 microcontrollers to receive DMX data
 *
 * Author: Kevan Ahlquist (@aterlumen)
 */

// Variables
const int DMXBufferSize = 512;
char DMXBuffer[DMXBufferSize];
int DMXBytesReceived; //16-bit counter
char DMXInputBuffer; //used to read RCREG to clear error conditions
const int DMXStartCode = 0x00;
enum {
    DMXMainLoop, DMXWaitBreak, DMXGotBreak, DMXWaitForStart, DMXWaitForData, DMXMoveData, DMXDone
} DMXState;

void DMXSetup(void)
{
    OSCTUNEbits.PLLEN = 1; //Set PLL on. Not sure why this isn't in the main setup function
    for (int i = 0; i < DMXBufferSize; i++) { //Clear the receive buffer 
    	DMXBuffer[i] = 0;
    }

	TRISCbits.TRISC7 = 1;	//Allow the EUSART RX to control pin RC7
	TRISCbits.TRISC6 = 1;	//Allow the EUSART RX to control pin RC6

    BAUDCONbits.BRG16 = 1; 	//Enable EUSART for 16-bit Asynchronous operation
	SPBRGH = 0;
	
    SPBRG = .31; 			//Baud rate is 250KHz for 32MHz Osc. freq.

    TXSTA = 0x04;			//Enable transmission and CLEAR high baud rate

    RCSTA = 0x90;			//Enable serial port and reception
}

void receiveDMX(void)
{
    while (DMXState != DMXDone) {
        switch (DMXState) {
            case DMXMainLoop:
                PORTBbits.RB0 = 1;
                PORTBbits.RB1 = 1;
                DMXState = DMXWaitBreak;
                break;
            case DMXWaitBreak:
                if (RCSTAbits.FERR){
                    DMXState = DMXGotBreak;
                } else {
                    if (RCSTAbits.OERR) {
            	        RCSTAbits.CREN = 0;		//Toggle CREN to clear OERR flag
            	        RCSTAbits.CREN = 1;
            	        break;
            	    }
                }//end else
                break;
            case DMXGotBreak:
                DMXInputBuffer = RCREG; //Read the Receive buffer to clear the error condition
                DMXState = DMXWaitForStart;
                break;
            case DMXWaitForStart:
                while (!PIR1bits.RCIF) ; //Wait until a byte is correctly received
                if (RCSTAbits.FERR) {		//Framing error
			        DMXInputBuffer = RCREG; //Read the Receive buffer to clear the error condition
		        } else {					//Got a byte
			        DMXInputBuffer = RCREG; //Read the Receive buffer to clear the error condition
		        }
		            if (DMXInputBuffer != DMXStartCode) { //if current byte isn't START code, ignore the frame
                    DMXState = DMXMainLoop;
                }
                DMXBytesReceived = 0;	//initialize counter
                break;
            case DMXWaitForData:
                if (RCSTAbits.FERR) {	//If a new framing error is detected (error or short frame)
        	        DMXState = DMXMainLoop;		// the rest of the frame is ignored and a new synchronization
    	        }						//is attempted
    	        if (PIR1bits.RCIF) {	//Wait until a byte is correctly received
    	            DMXInputBuffer = RCREG;		//read received byte from RCREG
    	            DMXState = DMXMoveData;
        	        break;
    	        }
                break;
            case DMXMoveData:
                DMXBuffer[DMXBytesReceived++] = DMXInputBuffer;
                if (DMXBytesReceived < DMXBufferSize) {
                    DMXState = DMXWaitForData;
                    break;
                } else DMXState = DMXDone;
                break;
            case DMXDone:
                return;
            default:
                // You're in no man's land now.
                break;
        }
    }
}