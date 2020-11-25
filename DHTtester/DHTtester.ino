#include "DHT.h"

#define DHTPIN 2     

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
  else{
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C -> "));

    unsigned int valor =  ( t <= 25 ) ? 00 :
                          ( t <= 26 ) ? 25 :
                          ( t <= 27 ) ? 50 :
                          ( t <= 28 ) ? 75 : 100;
    Serial.println(valor);
  }
  
}
