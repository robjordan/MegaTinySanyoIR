#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

#include <avr/sleep.h>

unsigned int Period=0xFFFF;
unsigned int interruptPB2=1;  // Send one burst at power-on, then go to sleep awaiting button push.

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER=Period; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~305Hz PWM
  TCA0.SINGLE.CMP0=0;
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
  setFrequency(38000);
  setDutyCycle(0);
  
  // Configure pin change interrupts to wake on button presses
  pinMode(PIN_PB2,INPUT_PULLUP);
  PORTB.PIN2CTRL=0b00001010; //PULLUPEN=1, ISC=2 trigger rising

  // prepare for sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  
}

void SendSanyoHeader() {
  // Serial.println("sending header");
  setDutyCycle(64); //~25%
  delay(9);
  setDutyCycle(0);
  delay(4);
  delayMicroseconds(500);
}

void SendSanyoBit(bool value) {
  // Serial.print("sending bit: ");
  // Serial.println(value ? '1' : '0');
  setDutyCycle(64); //~25%
  delayMicroseconds(560);
  setDutyCycle(0);
  delayMicroseconds(value ? 1690 : 565);
}

void Send (char IRtype, unsigned int Address, unsigned int Command) {
  
  // Sanyo LC7461
  // Protocol documented here: http://pdf.datasheetcatalog.com/datasheet/sanyo/LC7461.pdf
  if (IRtype == 'S') {
      SendSanyoHeader();
      // First send address (13-bits)
      unsigned int a = Address;
      for (unsigned int bit=0; bit < 13; bit++) {
        SendSanyoBit(a & 0x1000 ? true : false);
        a <<= 1;
      }
      // Then inverted address (13-bits)
      a = ~Address;
      for (unsigned int bit=0; bit < 13; bit++) {
        SendSanyoBit(a & 0x1000 ? true : false);
        a <<= 1;
      }
      // Then send command (8-bits)
      unsigned int c = Command;
      for (unsigned int bit=0; bit < 8; bit++) {
        SendSanyoBit(c & 0x80 ? true : false);
        c <<= 1;
      }
      // Then inverted address (13-bits)
      c = ~Command;
      for (unsigned int bit=0; bit < 8; bit++) {
        SendSanyoBit(c & 0x80 ? true : false);
        c <<= 1;
      }  
      // Final closing tone (same as a 1-bit)
      SendSanyoBit(true);    
  }
  else {
    Serial.print("Unsupported controller type: ");
    Serial.println(IRtype);
  }
}

void loop() {
  
  setDutyCycle(0);
  
  if (interruptPB2){
    interruptPB2=0;
    Serial.println("Button pressed");
    Send('S', 0x1000, 0xE0); // Sanyo code for sleep
  }

  // now sleep
  delay(1000);
  sleep_enable();
  sleep_cpu();


}

ISR(PORTB_PORT_vect) {
  byte flags=PORTB.INTFLAGS;
  PORTB.INTFLAGS=flags; //clear flags
  if (flags&0x04) {
    interruptPB2=1;
  }
}



void setDutyCycle(byte duty) {
  TCA0.SINGLE.CMP0=map(duty,0,255,0,Period);
}

void setFrequency(unsigned long freqInHz) {
  unsigned long tempperiod=(F_CPU/freqInHz);
  byte presc=0;
  while (tempperiod>65536 && presc<7) {
    presc++;
    tempperiod=tempperiod>>(presc>4?2:1);
  }
  Period=tempperiod;
  TCA0.SINGLE.CTRLA=(presc<<1)|TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.PER=Period;
}
