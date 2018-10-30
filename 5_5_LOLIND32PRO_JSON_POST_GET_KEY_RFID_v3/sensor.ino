#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 33

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float sensVal;

void setup_DS18B20() {
  sensors.begin();
}
void loop_DS18B20() {

  Serial.print("temp = ");
  sensors.requestTemperatures(); // Send the command to get temperatures
  sensVal = sensors.getTempCByIndex(0);
  Serial.println(sensVal);
  delay(50);

  //Json data generation ============================
  //  StaticJsonDocument<200> doc;
  //  JsonObject root = doc.to<JsonObject>();

  StaticJsonBuffer<150> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["sensorId"] = 1;
  root["values"] = sensVal;
  root["categoryId"] = 1;
  root["roomId"] = 1;

  root.printTo(jsonResult);
  
//  root.prettyPrintTo(Serial);
//  Serial.println("");

//  serializeJson(root, jsonResult);
//  serializeJson(root, Serial);
  //============================Json data generation
}


