/*
 * Organization: Tesla Works
 * Project: Animatronic heads
 *		Functions for use with PIC18 microcontrollers to receive DMX data
 *
 */

// Variables
char RxBuffer[512];
static int numReceivedBytes; //16-bit counter
char inputBuffer; //used to read RCREG to clear error conditions
const int DMXStartCode = 0x00;
enum {
    MainLoop, WaitBreak, GotBreak, WaitForStart, WaitForData, MoveData, Done
} state;

void setupDMX(void)
{
    OSCTUNEbits.PLLEN = 1; //Set PLL on. Not sure why this isn't in the main setup function
    for (int i = 0; i < 512; i++) { //Clear the receive buffer 
    	RxBuffer[i] = 0;
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
    while (state != Done) {
        switch (state) {
            case MainLoop:
                PORTBbits.RB0 = 1;
                PORTBbits.RB1 = 1;
                state = WaitBreak;
                break;
            case WaitBreak:
                if (RCSTAbits.FERR){
                    state = GotBreak;
                } else {
                    if (RCSTAbits.OERR) {
            	        RCSTAbits.CREN = 0;		//Toggle CREN to clear OERR flag
            	        RCSTAbits.CREN = 1;
            	        break;
            	    }
                }//end else
                break;
            case GotBreak:
                inputBuffer = RCREG; //Read the Receive buffer to clear the error condition
                state = WaitForStart;
                break;
            case WaitForStart:
                while (!PIR1bits.RCIF) ; //Wait until a byte is correctly received
                if (RCSTAbits.FERR) {		//Framing error
			        inputBuffer = RCREG; //Read the Receive buffer to clear the error condition
		        } else {					//Got a byte
			        inputBuffer = RCREG; //Read the Receive buffer to clear the error condition
		        }
		            if (inputBuffer != DMXStartCode) { //if current byte isn't START code, ignore the frame
                    state = MainLoop;
                }
                numReceivedBytes = 0;	//initialize counter
                break;
            case WaitForData:
                if (RCSTAbits.FERR) {	//If a new framing error is detected (error or short frame)
        	        state = MainLoop;		// the rest of the frame is ignored and a new synchronization
    	        }						//is attempted
    	        if (PIR1bits.RCIF) {	//Wait until a byte is correctly received
    	            inputBuffer = RCREG;		//read received byte from RCREG
    	            state = MoveData;
        	        break;
    	        }
                break;
            case MoveData:
                RxBuffer[numReceivedBytes++] = inputBuffer;
                if (numReceivedBytes < 512) {
                    state = WaitForData;
                    break;
                } else state = Done;
                break;
            case Done:
                return;
            default:
                // You're in no man's land now.
                break;
        }
    }
}
