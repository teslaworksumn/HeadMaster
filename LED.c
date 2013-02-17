void ledOn(char led)
{
    led = led % 6;                      // limit: 0 <= led <= 5; only 6 LEDs on board
    PORTB |= (0x01 << led);             // use bit shifting to specify the LED to turn on
}

void ledOff(char led)
{
    led = led % 6;
    PORTB &= ~(0x01 << led);
}

void ledOut(char leds)
{
    leds = leds % ((1 << 6) - 1);       // 0b1000000 - 1 = 0b011111; limit ledOut to modifying 6 LSBs
    // TODO: make ledOut output the proper values to RB0 ... RB5
}

char getLeds(void)
{
    return PORTB && 0b00111111;         // return the values of the 6 LEDs on RB0 ... RB5
}

void getLed(char ledNum)
{
    ledNum = ledNum % 6;                // limit: 0 <= led <= 5; only 6 LEDs on board
    return PORTB && (0x01 <<  ledNum);  // 
}