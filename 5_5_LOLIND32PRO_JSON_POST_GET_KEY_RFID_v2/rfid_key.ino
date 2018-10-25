

#define SS_PIN 21  //
#define RST_PIN 22 //
#define SELENOID 25


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
byte statRFID = 0;
byte statKey = 0;

int out = 0;

const long intervalDelay = 3000;
unsigned long nowMilKeyPad = 0;
unsigned long nowMilRFID = 0;
unsigned long trigMilKeyPad = 0;
unsigned long trigMilRFID = 0;

unsigned long deltaMilKeyPad = 0;
unsigned long deltaMilRFID = 0;

Password passwordKEYPAD = Password( "1234" ); //Ubah password

const byte baris = 4; //Baris
const byte kolom = 4; //Kolom

char keys[baris][kolom] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte barisPin[baris] = {15, 2, 0, 4};
byte kolomPin[kolom] = {13, 12, 14, 27};

// Buat objek keypad
Keypad keypad = Keypad( makeKeymap(keys), barisPin, kolomPin, baris, kolom );

void setup_rfid_key() {
  //  Serial.begin(115200);
  Serial.print("Enter: ");
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  delay(100);
  pinMode(SELENOID, OUTPUT);
  keypad.addEventListener(keypadEvent);
  digitalWrite(SELENOID, HIGH);
}

void loop_rfid_key() {

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++withoutDELAY start

  nowMilKeyPad = millis();
  nowMilRFID   = millis();

  deltaMilKeyPad = nowMilKeyPad - trigMilKeyPad;
  deltaMilRFID = nowMilRFID - trigMilRFID;

  if ((statRFID == 1) && (deltaMilRFID >= intervalDelay)) {
    if (deltaMilRFID >= intervalDelay) {
      Serial.println("=====deltaMilRFID >= intervalDelay =  ============================================ RFID");
      delay(50);
      digitalWrite(SELENOID, HIGH);
      delay(10);
      statuss = 1;
    }
    statRFID = 0;
  }
  if ((statKey == 1) && (deltaMilKeyPad >= intervalDelay)) {
    if (deltaMilKeyPad >= intervalDelay) {
      Serial.println("=====deltaMilKeyPad >= intervalDelay ============================================KEYPAD ");
      delay(50);
      digitalWrite(SELENOID, HIGH);
      delay(10);
      statuss = 1;
    }
    statKey = 0;
  }

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++withoutDELAY end

  keypad.getKey();
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == "D3 4A 2E 24") //change UID of the card that you want to give access
  {
    delay(40);
    statRFID = 1;
    trigMilRFID = millis();
    Serial.println(" AKSES DITERIMA ");
    digitalWrite(SELENOID, LOW);
    delay(10);
  }
  else   {
    Serial.println(" AKSES DITOLAK ");
  }
}


//keypad

void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:
      Serial.println(eKey);
      delay(10);

//      Serial.write(254);

      switch (eKey) {
        case '*': Login(); delay(1); break;

        case '#': passwordKEYPAD.reset(); delay(1); break;

        default: passwordKEYPAD.append(eKey); delay(1);
      }
  }
}

void Login() {
  if (passwordKEYPAD.evaluate()) {
    trigMilKeyPad = millis();
    Serial.println("AKSES DITERIMA");
    digitalWrite(SELENOID, LOW);
    statKey = 1;
    delay(50);
  }
  else {
    Serial.println("AKSES DITOLAK");
    Serial.write(254);
    delay(10);
  }
}
