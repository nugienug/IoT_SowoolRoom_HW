#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include <Password.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define lamp1 26 //SSR ==> LAMP1
#define SOLENOID 25


Password passwordKEYPAD = Password( "1234" ); //Ubah password

//========withoutDelay(mainPOST) start
const long intervalMainPost = 5000;

unsigned long nowMainPost = 0;
unsigned long prevMainPost = 0;

//========withoutDelay(mainPOST) end

//========withoutDelay(mainGET) start
const long intervalMainGet = 3000;

unsigned long nowMainGet = 0;
unsigned long prevMainGet = 0;

//========withoutDelay(mainGET) end


//http==============================
//char* gssid  = "ng-asus";
//char* gpassword = "12345678";
////char* gatewayip = "209.97.159.204";//server
//char* gatewayip = "192.168.43.100";//localhost



char* gssid  = "AP-02";
char* gpassword = "12345678";

//char* gssid  = "SOWOOL_IoT";
//char* gpassword = "12345678";

char* gatewayip = "209.97.159.204";//server
//char* gatewayip = "192.168.10.4";//localhost
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
int httpCode;
HTTPClient http;

void setup() {
  pinMode(lamp1, OUTPUT); //SSR ==> LAMP
  setup_DS18B20();
  setup_rfid_key();
  Serial.begin(brate);
  setup_http();
}

void loop() {

  loop_rfid_key();

  nowMainPost = millis();
  nowMainGet  = millis();

  if (nowMainPost - prevMainPost >= intervalMainPost) {
    prevMainPost = nowMainPost;
    loop_DS18B20();
    loop_post();
  }


  if (nowMainGet - prevMainGet >= intervalMainGet) {
    prevMainGet = nowMainGet;
    loop_get();
  }
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

//-------------------------------------------------------------------------------------GET START
void loop_get() {
  //  StaticJsonBuffer<300> JSONBuffer;
  ////////////////////====================HTTP JSON GET <<---LAMP
  Serial.println("ARDUINO HTTP JSON GET <<< LAMP");
  //  String hosturlGET_LIGHT = "http://" + String(gatewayip) + ":" + String(port) + "/light?id=1";
  String hosturlGET_LIGHT = "http://" + String(gatewayip) + ":" + String(port) + "/sowool/light?id=1";
  http.begin(hosturlGET_LIGHT);
  httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) { //Check for the returning code

    String payload = http.getString();
    Serial.println(httpCode);
//    Serial.println(payload);

    StaticJsonBuffer<200> JSONBuffer;
    JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message

    if (!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(10);
      return;
    }

    boolean lampStat = parsed["statusOn"]; //Get sensor type value

    Serial.print("Lamp: ");
    Serial.println(lampStat);

    //    delay(50);
    //    JSONBuffer.clear();

    if (lampStat == true) {
      digitalWrite(lamp1, HIGH);
    } else {
      digitalWrite(lamp1, LOW);
    }

  }

  Serial.println("ARDUINO HTTP JSON GET <<< DOOR");
  //  String hosturlGET_DOOR = "http://" + String(gatewayip) + ":" + String(port) + "/door?id=1";
  String hosturlGET_DOOR = "http://" + String(gatewayip) + ":" + String(port) + "/sowool/door?id=1";
  http.begin(hosturlGET_DOOR);
  httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) { //Check for the returning code

    String payload = http.getString();
    Serial.println(httpCode);
//    Serial.println(payload);

    StaticJsonBuffer<250> JSONBuffer;
    JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message

    if (!parsed.success()) {   //Check for errors in parsing
      Serial.println("Parsing failed");
      delay(10);
      return;
    }

    boolean doorStat = parsed["statusOpen"]; //Get sensor type value
    //    String pinVal = parsed["pin"];

    Serial.print("Door: ");
    Serial.println(doorStat);
    //
    //    Serial.print("pin: ");
    //    Serial.println(pinVal);
    //    Serial.println();

    //    int lenPinVal = pinVal.length();
    //    char pinValArray[lenPinVal];
    //    pinVal.toCharArray(pinValArray, lenPinVal);
    //    passwordKEYPAD.set(pinValArray);

    Serial.println();
    //    delay(50);

    if (doorStat == true) {
      digitalWrite(SOLENOID, LOW);
    } else {
      digitalWrite(SOLENOID, HIGH);
    }

  }
}
//-------------------------------------------------------------------------------------GET END
