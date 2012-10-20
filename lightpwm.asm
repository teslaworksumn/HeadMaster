;*********************************************************************************************
; Software License Agreement                                                                 *
; The software supplied herewith by Microchip Technology Incorporated                        *
; (the "Company") is intended and supplied to you, the Company's                             *
; customer, for use solely and exclusively on Microchip products.                            *
;                                                                                            *
; The software is owned by the Company and/or its supplier, and is                           *
; protected under applicable copyright laws. All rights are reserved.                        *
; Any use in violation of the foregoing restrictions may subject the                         *
; user to criminal sanctions under applicable laws, as well as to                            *
; civil liability for the breach of the terms and conditions of this                         *
; license.                                                                                   *
;                                                                                            *
; THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,                          *
; WHETHER EXPRESS, IMPLIED OR STATU-TORY, INCLUDING, BUT NOT LIMITED                         *
; TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                                *
; PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,                          *
; IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                                 *
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                                          *
;                                                                                            *
;*********************************************************************************************
; File:  DMX512ARec.asm
; DMX512 Receiver
; This file uses a PIC18F24J10 device to receive DMX-512 data and transport it to a PWM
; peripheral via I2C. 16 channels selectable via DIP switch.

    list        p=PIC18F24J10
    #include    <P18F24J10.inc>
    radix       decimal
    
; Configuration bits setup
    CONFIG  CCP2MX = ALTERNATE      ;Assign CCP2 output to pin RB3
    CONFIG  WDTEN = OFF             ;To use ICD2 as a debugger disable Watch Dog Timer 
    CONFIG  STVREN = ON             ;Reset on stack overflow/underflow enabled
    CONFIG  XINST = OFF             ;Instruction set extension and Indexed Addressing 
                                    ; mode disabled (Legacy mode)
    CONFIG  CP0 = OFF               ;Program memory is not code-protected
    CONFIG  FOSC = HSPLL            ;Use external crystlal oscillator
    CONFIG  FOSC2 = ON              ;Clock selected by FOSC as system clock is enabled
                                    ; when OSCCON<1:0> = 00
    CONFIG  FCMEN = OFF             ;Fail-Safe Clock Monitor disabled
    CONFIG  IESO = OFF              ;Two-Speed Start-up disabled
    CONFIG  WDTPS = 32768           ;1:32768

; Variables
    CBLOCK  0x8
        CountL,CountH               ;16-bit counter
        RxBuffer:512                ;512-byte buffer allocation
    ENDC
    
;******************************************************************************

    ORG     0x0

Main
    call    Setup                   ;Setup Serial port and buffers
    
; Initialize the PWM chips
    call    I2CStart
    movlw   0x00                    ;Write first chip
    call    I2CByte
    call    InitializePWM
    call    I2CStop
    
    call    I2CStart
    movlw   0x02                    ;Write second chip
    call    I2CByte
    call    InitializePWM
    call    I2CStop
    
    call    I2CStart
    movlw   0x00
    call    I2CByte
    call    EnablePWM
    call    I2CStop
    
    call    I2CStart
    movlw   0x02
    call    I2CByte
    call    EnablePWM
    call    I2CStop

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
    bra     RXend               ; the rest of the frame is ignored and a new synchronization
                                ; is attempted

    btfss   PIR1,RCIF           ;Wait until a byte is correctly received
    bra     WaitForData
    movf    RCREG,W
    bcf     PORTB,RB1           ;Green LED on when DMX is fully in


MoveData
    movwf   POSTINC0            ;Move the received data to the buffer 
                                ; (auto-incrementing pointer)
    incf    CountL,F            ;Increment 16-bit counter
    btfss   STATUS,C
    bra     WaitForData
    incf    CountH,F

    btfss   CountH,1            ;Check if 512 bytes of data received
    bra     WaitForData


;******************************************************************************

RXend
    lfsr    FSR0,RxBuffer       ;Use FSR0 to address the receiver buffer

GetData
; Calculate the starting DMX address
    movf    PORTA,W             ;Load raw pins
    rlncf   WREG,W              ;Shift bits left so DIP switch moves
    rlncf   WREG,W              ; in 16-ch increments
    rlncf   WREG,W
    rlncf   WREG,W
    andlw   b'11110000'         ;Mask the top 4 bits only
    addwf   FSR0L               ;Move the calculated DMX starting address
    movlw   0x00                ; to the low bits of the channel register
    addwfc  FSR0H
                                   
    movlw   0x00                ;Clear WREG for high channel
    btfsc   PORTA,RA5           ;Test if hightest (256) dip is on
    bsf     WREG,0              ; and if it is
    addwf   FSR0H               ; move it to the offset register
                                 
I2CTransmit
    call    I2CStart            ;Send the I2C start signal
    
    movlw   0x00                ;Load address of first PWM driver (ch1-8)
    call    I2CByte             ;Transmit the address of the first PWM driver
    
    movlw   b'10100010'         ;Load mode data for PWM LED driver
    call    I2CByte             ;Then transmit byte over I2C
   
; Send 8 bytes of DMX channel values
    call    I2CData             ;1
    call    I2CData             ;2
    call    I2CData             ;3
    call    I2CData             ;4
    call    I2CData             ;5
    call    I2CData             ;6
    call    I2CData             ;7
    call    I2CData             ;8

    call    I2CStop             ; send stop signal
    bcf     PORTB,RB0           ;Green LED on when DMX fully in
   

; Second PWM
    call    I2CStart            ;Send second start signal for second PWM driver
    
    movlw   0x02                ;Address of second PWM LED driver (ch9-16)
    call    I2CByte
    
    movlw   b'10100010'         ;Load mode data for PWM LED driver
    call    I2CByte             ;Then transmit byte over I2C
     
; Send 8 bytes of DMX channel values
    call    I2CData             ;1
    call    I2CData             ;2
    call    I2CData             ;3
    call    I2CData             ;4
    call    I2CData             ;5
    call    I2CData             ;6
    call    I2CData             ;7
    call    I2CData             ;8
    
    call    I2CStop             ;Send stop signal

;Return to Main Loop
    bra     MainLoop            ;Return to main loop 

WaitForI2C
    btfss   PIR1,SSP1IF         ;Check if the I2C interrupt flag is set
    bra     WaitForI2C          ;Loop back if it is not set
    bcf     PIR1,SSP1IF         ;Clear the flag if it is set
    
    return

I2CStart
    bsf     SSP1CON2,SEN        ;Set start enable flag to send I2C start signal
    call    WaitForI2C          ;Wait for I2C operation to complete
    
    return

I2CByte
    movwf   SSP1BUF             ;Transmit byte in WREG over I2C
    call    WaitForI2C          ;Then wait for operation to complete
    
    btfsc   SSPCON2,ACKSTAT     ;Check ACK status bit
    goto    I2CFail             ;Go to to failure routine
                                ; if the transmission failed

    return

I2CData
    movf    POSTINC0,W          ;Load channel value
    bra     I2CByte             ;Write send it over the bus

I2CStop
    bsf     SSP1CON2,PEN        ;Set start enable flag to send I2C start signal
    call    WaitForI2C          ;Wait for I2C operation to complete
    
    return

I2CFail
    call    I2CStop             ; End transmission
    bsf     PORTB,1             ;Turn off blue LED
    goto    MainLoop            ;Start over

;******************************************************************************

; Setup Serial port and buffers
Setup

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
    
; Init I/O for LEDs
    movlw   b'11111100'
    movwf   TRISB           ;Make pin RB3 (CCP2) output

; Init I2C I/O
    movlw   b'11111111'
    movwf   TRISC

; Configure LEDs
    movlw   0x03
    movwf   PORTB           ;LEDs off

; Setup I2C (MSSP) module
    movlw   0x08
    movwf   SSP1CON1        ;Configure MSSP for I2C Master Mode
    movlw   0x0F
    movwf   SSP1ADD         ;Set I2C baud rate to approximately 100kHz
    bsf     SSP1CON1,SSPEN  ;Enable MSSP module

; Configure DIP switch input
    clrf    PORTA           ;Initialize PORTA by clearing output
    movlw   0Fh
    movwf   ADCON1          ;Configure A/D for digital inputs
    movlw   b'11111111'
    movwf   TRISA           ;Configure all PORTA pins for input

    return
    
InitializePWM    

    movlw   b'10000000'
    call    I2CByte         ;Auto increment write all registers
    movlw   b'00000000'
    call    I2CByte         ;Normal mode
    movlw   b'00010100'
    call    I2CByte         ;Inverted for external drivers, totem pole,output 0 when /OE = 1
    
    return
    
EnablePWM
    movlw   b'10001100'
    call    I2CByte         ;Auto increment starting at LED output state 0
    movlw   b'10101010'
    call    I2CByte         ;First four LEDs controlled by PWM
    movlw   b'10101010'
    call    I2CByte         ;Second four LEDs controlled PWM
    
    return


    END
