// Valor de PWM se encuentra entre 0 y 79 como valores aceptables recibidos por el FAN

word salidaPwm = 3;
const byte interruptPin = 2;
//int potenciometro = A0;
//int valor = 0;
int valorPwm = 0;
double pulso = 0.0;
double frecuencia = 0;

volatile int RPMcont=0;
unsigned long tiempo;
unsigned int promedioTime = 0;
int cont = 0;
bool flag = false;

void cicloPWM(byte ocrb);
void configura25KhzPwm();
void conteo(void);

void setup() {
  pinMode(salidaPwm,OUTPUT);
  //pinMode(potenciometro,INPUT);
  configura25KhzPwm();
  cicloPwm(0);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(interruptPin), conteo, RISING );
  tiempo = millis();
}
void loop(){

  if (Serial.available() > 0) {
    // read the incoming byte:
    valorPwm = Serial.parseInt(SKIP_ALL,'\n');
    
    // say what you got:
    Serial.print("I received: ");
    Serial.println(valorPwm, DEC);
    cicloPwm(valorPwm);
  }
  
  if(millis() - tiempo >= 500){
    promedioTime += RPMcont;
    cont++;
    if( cont == 10 ){
      Serial.println(promedioTime*3); //(60/2)/10
      promedioTime = 0;
      cont = 0;
    }
    tiempo = millis();
    RPMcont = 0;
  }

}
void conteo(void){
  RPMcont++;
}
void configura25KhzPwm(){
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;
  TIFR2 = 0;
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B |= (1 << WGM22) | (1 << CS21);
  OCR2A = 79;
  OCR2B = 0;
}

void cicloPwm(byte ocrb){
  OCR2B = ocrb;
}
/*
void loop() {
  //valor = analogRead(potenciometro);
  //valorPwm = map(valor,0,1023,0,79);
  for(int i = 0; i <= 79; i+= 10){
    valorPwm = i;
    cicloPwm(valorPwm);  
    delay(1000);
  }

  for(int i = 79; i >= 0; i-=10){
    valorPwm = i;
    cicloPwm(valorPwm);  
    delay(1000);
  }
  
  //medirRPM();
}


void medirRPM(){
  pulso = pulseIn(11,HIGH);

  frecuencia = 1000000/(2*pulso);
  Serial.print("Frecuencia: ");
  Serial.println( (frecuencia*60)/2 );
}
*/
