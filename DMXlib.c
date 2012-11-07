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
    DMXMainLoop, DMXWaitBreak, DMXGotBreak, DMXWaitForStart, DMXWaitForData, DMXDone
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

void DMXReceive(void)
{
    while (DMXState != DMXDone) {
        switch (DMXState) {
            case DMXMainLoop:
                PORTBbits.RB0 = 1;
                PORTBbits.RB1 = 1;
                DMXState = DMXWaitBreak;
                break;
            case DMXWaitBreak:
                if (RCSTAbits.FERR){            //Framing error
                    DMXState = DMXGotBreak;
                    break;
                } else {
                    if (RCSTAbits.OERR) {
            	        RCSTAbits.CREN = 0;		//Toggling CREN clears OERR flag
            	        RCSTAbits.CREN = 1;
            	    }
                }
                break;
            case DMXGotBreak:
                DMXInputBuffer = RCREG;         //Read the Receive buffer to clear FERR
                DMXState = DMXWaitForStart;
                break;
            case DMXWaitForStart:
                while (!PIR1bits.RCIF) ;        //Wait until a byte has been received
                if (RCSTAbits.FERR) {
			        DMXState = DMXGotBreak;
			        break;
		        } else {
			        DMXInputBuffer = RCREG;     //Read the Receive buffer
		        }
		        if (DMXInputBuffer != DMXStartCode) { //if current byte isn't START code, ignore the frame
                    DMXState = DMXMainLoop;
                    break;
                }
                DMXBytesReceived = 0;	        //initialize counter
                DMXState = DMXWaitForData;
                break;
            case DMXWaitForData:
                if (RCSTAbits.FERR) {	        //If a new framing error is detected (error or short frame)
        	        DMXState = DMXMainLoop;		// the rest of the frame is ignored and a new synchronization
        	        break;                      //is attempted
    	        }
    	        if (PIR1bits.RCIF) {	        //Wait until a byte is correctly received
    	            DMXBuffer[DMXBytesReceived++] = RCREG;
        	        if (DMXBytesReceived < DMXBufferSize) {
                        DMXState = DMXWaitForData;
                        break;
                    } else {
                        DMXState = DMXDone;
                        break;
                    }
                }
                break;
            case DMXDone:
                return;
            default:
                // You're in no man's land now.
                break;
        }
    }
}
