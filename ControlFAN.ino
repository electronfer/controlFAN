//======================//
//=     LIBREARIAS     =//
//======================//
#include "DHT.h"

//=======================================//
//=     MACROS Y VARIABLES GLOBALES     =//
//=======================================//
#define TIMER 1
#define MAX_PWM 75
#define NUM_FANS 6
#define TIMESTEP 7e3
#define DHTPIN 2     
#define DHTTYPE DHT22

const word PWM_FREQ_HZ = 20000;
const word TCNT1_TOP = 16000000/(2*PWM_FREQ_HZ);


//================================================//
//=     OBJETO TIPO DHT (SENSOR TEMPERATURA)     =//
//================================================//
DHT dht(DHTPIN, DHTTYPE);

unsigned short int pinesFAN[NUM_FANS] = { 
  3 , //OC2B
  5 , //OC0B 
  6 , //OC0A
  9 , //OC1A
  10, //OC1B
  11  //OC2A
};

unsigned int valorPwm = 0;

void cicloPWM(byte ocrb);
void configura25KhzPwm();

void setup() {
  dht.begin();
#if TIMER == 0
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  for(unsigned short int indx = 0 ; indx < NUM_FANS ; indx++ )
    pinMode( pinesFAN[indx] , OUTPUT );
  
  configura25KhzPwm();
  cicloPwm(0);
#else 
  for(unsigned short int indx = 0 ; indx < NUM_FANS ; indx++ )
    pinMode(pinesFAN[indx], OUTPUT);

/*
 *   TCCR2A = 0;

  TCCR2B = 0;

  TIMSK2 = 0;

  TIFR2 = 0;

  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

  TCCR2B |= (1 << WGM22) | (1 << CS21);

  OCR2A = 79;
  OCR2B = 0;
*/
  // Clear Timer0 control and count registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  
  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Clear Timer1 control and count registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  // Set Timer1 configuration
  // COM1A(1:0) = 0b10   (Output A clear rising/set falling)
  // COM1B(1:0) = 0b00   (Output B normal operation)
  // WGM(13:10) = 0b1010 (Phase correct PWM)
  // ICNC1      = 0b0    (Input capture noise canceler disabled)
  // ICES1      = 0b0    (Input capture edge select disabled)
  // CS(12:10)  = 0b001  (Input clock select = clock/1)

  TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
  TCCR0B |= (1 << CS01);
  
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11) ;
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;

  TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B |= (1 << CS21);
  
#endif
}
void loop(){
#if TIMER == 0
  for(valorPwm = 0; valorPwm < 50; valorPwm += 10){
       
    cicloPwm(valorPwm);
    for(unsigned long int i = 0 ; i < 5e4; i++)
      digitalWrite(LED_BUILTIN, HIGH);
    for(unsigned long int i = 0 ; i < 5e4; i++)
      digitalWrite(LED_BUILTIN, LOW); 
  }
  
  for( ; valorPwm > 0; valorPwm -= 10){
    
    cicloPwm(valorPwm);
    for(unsigned long int i = 0 ; i < 5e4; i++)
      digitalWrite(LED_BUILTIN, HIGH);
    for(unsigned long int i = 0 ; i < 5e4; i++)
      digitalWrite(LED_BUILTIN, LOW);
  }

#else
    float t = dht.readTemperature();
    unsigned int valor =  ( t <= 25 ) ? 00 :
                          ( t <= 26 ) ? 25 :
                          ( t <= 27 ) ? 50 :
                          ( t <= 28 ) ? 75 : 100;
    setPwmDuty(valor);
    delay(2*TIMESTEP);
    /*
    setPwmDuty(25);
    delay(TIMESTEP);
    setPwmDuty(50);
    delay(TIMESTEP);
    setPwmDuty(75);
    delay(TIMESTEP);
    setPwmDuty(100);
    delay(2*TIMESTEP);
    setPwmDuty(75);
    delay(TIMESTEP);
    setPwmDuty(50);
    delay(TIMESTEP);
    setPwmDuty(25);
    delay(TIMESTEP);
    */
#endif
}

#if TIMER == 0

void configura25KhzPwm(){
  TCCR0A = 0;
  TCCR2A = 0;

  TCCR0B = 0;
  TCCR2B = 0;

  TIMSK0 = 0;
  TIMSK2 = 0;

  TIFR0 = 0;
  TIFR2 = 0;

  TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

  TCCR0B |= (1 << WGM02) | (1 << CS01);
  TCCR2B |= (1 << WGM22) | (1 << CS21);

  OCR0A = 79;
  OCR2B = 0;
  
  OCR2A = 79;
  OCR2B = 0;
}

void cicloPwm(byte ocrb){
  OCR2B = ocrb;
}

#else

void setPwmDuty(byte duty) {
  OCR0A = (word) (duty*0XFF)/100;
  OCR0B = (word) (duty*0XFF)/100;
  OCR1A = (word) (duty*TCNT1_TOP)/100;
  OCR1B = (word) (duty*TCNT1_TOP)/100;
  OCR2A = (word) (duty*0XFF)/100;
  OCR2B = (word) (duty*0XFF)/100;
}
#endif

// Programa 3
/*
void setup()
{
    // Set the main system clock to 8 MHz.
    noInterrupts();
    CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
    CLKPR = _BV(CLKPS0);  // divide frequency by 2
    interrupts();

    // Configure Timer 0 for phase correct PWM @ 25 kHz.
    TCCR0A = 0;           // undo the configuration done by...
    TCCR0B = 0;           // ...the Arduino core library
    TCNT0  = 0;           // reset timer
    TCCR0A = _BV(COM0B1)  // non-inverted PWM on ch. B
        | _BV(WGM00);  // mode 5: ph. correct PWM, TOP = OCR0A
    TCCR0B = _BV(WGM02)   // ditto
        | _BV(CS00);   // prescaler = 1
    OCR0A  = 160;         // TOP = 160

    // Same for Timer 1.
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
        | _BV(COM1B1)  // same on ch. B
        | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
        | _BV(CS10);   // prescaler = 1
    ICR1   = 160;

    // Same for Timer 2.
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2  = 0;
    TCCR2A = _BV(COM2B1)  // non-inverted PWM on ch. B
        | _BV(WGM20);  // mode 5: ph. correct PWM, TOP = OCR2A
    TCCR2B = _BV(WGM22)   // ditto
        | _BV(CS20);   // prescaler = 1
    OCR2A  = 160;
}

void loop()
{
    analogWrite( 3,   1);  // duty cycle = 1/160
    analogWrite( 5,  53);  // ~ 1/3
    analogWrite( 9, 107);  // ~ 2/3
    analogWrite(10, 159);  // 159/160
}
*/
