#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include <Password.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define lamp1 26 //SSR ==> LAMP1

//========withoutDelay(mainLOOP) start
const long intervalMainLoop = 5000;

unsigned long nowMainLoop = 0;
unsigned long prevMainLoop = 0;

//========withoutDelay(mainLOOP) start


//http==============================
//char* gssid  = "ng-asus";
//char* gpassword = "12345678";
////char* gatewayip = "209.97.159.204";//server
//char* gatewayip = "192.168.43.100";//localhost

char* gssid  = "AP-19";
char* gpassword = "12345678";

//char* gssid  = "SOWOOL_IoT";
//char* gpassword = "12345678";

//char* gatewayip = "209.97.159.204";//server
char* gatewayip = "192.168.10.4";//localhost
//char* gatewayip = "192.168.1.102";//localhost

const uint16_t port = 8080;
IPAddress myIP;
//==============================http

//values********************
int brate = 115200;
String jsonResult = "";
//********************values

//GPIO********************
//int pinDHT = D4;
//********************GPIO

HTTPClient http;

void setup() {
  pinMode(lamp1, OUTPUT); //SSR ==> LAMP
  setup_DS18B20();
  setup_rfid_key();
  Serial.begin(brate);
  setup_http();
  //  keypad.addEventListener(keypadEvent);
}

void loop() {

  loop_rfid_key();

  nowMainLoop = millis();

  if (nowMainLoop - prevMainLoop >= intervalMainLoop) {
    prevMainLoop = nowMainLoop;
    loop_DS18B20();
    loop_http();
  }

  ////////////////////====================HTTP JSON GET
  Serial.println("/////////////////////////////---ARDUINO HTTP JSON GET <<<<<< LIGHT");
  String hosturlGET_LIGHT = "http://" + String(gatewayip) + ":" + String(port) + "/light?id=1";
  http.begin(hosturlGET_LIGHT);
  int httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) { //Check for the returning code

    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);

    StaticJsonBuffer<300> JSONBuffer;
    JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message

    if (!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(50);
      return;
    }

    boolean lampStat = parsed["statusOn"]; //Get sensor type value

    Serial.print("Lamp: ");
    Serial.println(lampStat);

    Serial.println();
    delay(50);

    if (lampStat == true) {
      digitalWrite(lamp1, HIGH);
    } else {
      digitalWrite(lamp1, LOW);
    }

  }

  //  delay(3000);
}


//isFloat=========================
//check whether tString is numeric or not ( to filter dnoise data  )
//====================================
boolean isFloat(String tString) {
  String tBuf;
  boolean decPt = false;

  if (tString.charAt(0) == '+' || tString.charAt(0) == '-') tBuf = &tString[1];
  else tBuf = tString;

  for (int x = 0; x < tBuf.length(); x++)
  {
    if (tBuf.charAt(x) == '.') {
      if (decPt) return false;
      else decPt = true;
    }
    else if (tBuf.charAt(x) < '0' || tBuf.charAt(x) > '9') return false;
  }
  return true;
}
//=========================isFloat
