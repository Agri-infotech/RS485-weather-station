// กำหนดตัวแปรสำหรับการสื่อสารผ่านโปรโตคอล Modbus RTU
#define RXD2 16  // กำหนดหมายเลข GPIO 16 สำหรับขา Rx
#define TXD2 17  // กำหนดหมายเลข GPIO 17 สำหรับขา Tx

#define SW 2  // Custom swithch

// OUTPUTS
#define R1 23
#define R2 19
#define R3 18
#define R4 32

String APssid = "WSTDevkit";
String APpassword = "dddddddd";

int SETMODE = 0;

String WiFissid = "";
String WiFipassword = "";

String WiFiRSSI;
String myIP;
String webTitle = "RS485 Weather Station";
String device_name = "485wst";

int32_t setVar1, setVar2, setVar3, setVar4, WINSPD, WINDIR;
float_t TEMP, HUMI;

struct WEBINFO {
  String stateText;
  String icon;
  String cssClass;
};
struct WEBINFO R1info = { "OFF", "", "normal" };
struct WEBINFO R2info = { "OFF", "", "normal" };
struct WEBINFO R3info = { "OFF", "", "normal" };
struct WEBINFO R4info = { "OFF", "", "normal" };

void controlRelay() {
  // Serial.println("Controll run");
  // Serial.println(TEMP);
  // Serial.println(setVar1);
  // Serial.println(setVar2);
  if (round(TEMP) > setVar1 && setVar1 > 0) {
    relayHi(R1);
    R2info.stateText = "ON";
    R1info.cssClass = "alert";
  } else {
    relayLo(R1);
    R1info.stateText = "OFF";
    R1info.cssClass = "normal";
  }
  if (round(HUMI) > setVar2 && setVar2 > 0) {
    relayHi(R2);
    R2info.stateText = "ON";
    R2info.cssClass = "alert";
  } else {
    relayLo(R2);
    R2info.stateText = "OFF";
    R2info.cssClass = "normal";
  }
  if (WINSPD > setVar3 && setVar3 > 0) {
    relayHi(R3);
    R3info.stateText = "ON";
    R3info.cssClass = "alert";
  } else {
    relayLo(R3);
    R3info.stateText = "OFF";
    R3info.cssClass = "normal";
  }
  if (WINDIR > setVar4 && setVar4 > 0) {
    relayHi(R4);
    R4info.stateText = "ON";
    R4info.cssClass = "alert";
  } else {
    relayLo(R4);
    R4info.stateText = "OFF";
    R4info.cssClass = "normal";
  }
}

void setup() {
  TEMP = 0;
  HUMI = 0;
  WINSPD = 0;
  WINDIR = 0;
  Serial.begin(115200);  // For debug
  Serial.println("ESP start.");
  serialSetup();
  relaySetup();
  resetbuttonSetup();
  storageSetup();

  if (SETMODE == 1)  // SET
  {
    SETMODE = true;
    wifiapSetup();

  } else if (SETMODE == 2) {  // RESET FACTORY
    storageClear();
    // ESP.restart();
  } else {
    // RUN
    webserverSetup();
  }
}

void loop() {
  if (SETMODE == 0) {

    webserverLoop();
    clientLoop();
    serialLoop();
    controlRelay();
  } else if (SETMODE == 1)  // SET
  {
    Serial.println("SET MODE.");
    clientLoop();
    blinkSet();
  } else if (SETMODE == 2) {  //RESET
    blinkReset();
  } else {
    Serial.println("CLIENT MODE.");
    clientLoop();
  }
}