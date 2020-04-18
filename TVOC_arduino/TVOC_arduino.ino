#include <Wire.h>
#include "Adafruit_SGP30.h"
unsigned long time;
unsigned long last_time = 0;
unsigned long current_time = -1;

Adafruit_SGP30 sgp;

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void setup() {
  Serial.begin(9600);

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  Serial.print("Time");Serial.print(" \t ");
  Serial.print("TVOC (ppb)");Serial.print(" \t ");
  Serial.println("eCO2 (ppm)");

  // Set baseline values obtained before
  sgp.setIAQBaseline(0x96B1, 0x93CA);  
}

void loop() {
  time = millis();
  // If you have a temperature / humidity sensor, 
  //you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  float temperature = 23; // [°C]
  float humidity = 26; //[%RH]
  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print(time); Serial.print(" \t ");
  Serial.print(sgp.TVOC); Serial.print(" \t \t ");
  Serial.println(sgp.eCO2);

  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }

  delay(1000);
  
  current_time = time / (1000/0.000277777); 
  if (last_time != current_time ) {
    last_time = current_time;
    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
        Serial.println("Failed to get baseline readings");
        return;
    }

    sgp.setIAQBaseline(eCO2_base, TVOC_base ); //Sets the baseline value every 
    
  }
}
