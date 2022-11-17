#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(80);

String authFailResponse = "Authentication Failed";

void wifiapSetup() {
  Serial.println("Configuring access point...");
  Serial.println("APssid");
  Serial.println(storageGetString("APssid"));

  Serial.println("APpassword");
  Serial.println(storageGetString("APpassword"));

  WiFi.softAP(storageGetString("APssid").c_str(), storageGetString("APpassword").c_str());

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  server.on("/", configForm);
  server.on("/saveConfig", saveConfig);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("AP mode started");
}

unsigned long previousMillis = 0;
unsigned long interval = 30000;
void webserverSetup() {
  unsigned long currentMillis = millis();
  Serial.println("WiFissid");
  Serial.println(storageGetString("WiFissid"));
  Serial.println("WiFipassword");
  Serial.println(storageGetString("WiFipassword"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(storageGetString("WiFissid").c_str(), storageGetString("WiFipassword").c_str());
  if (WiFi.status() != WL_CONNECTED) {
    blinkWiFiLoss();
    delay(1000);
  }
  //Wait for connection
  while (WiFi.status() != WL_CONNECTED && (currentMillis - previousMillis >= interval)) {

    //Serial.println(WiFi.status());
    blinkWiFiLoss();
    delay(1000);
    previousMillis = currentMillis;
  }

  Serial.println("");
  if (MDNS.begin(storageGetString("deviceName").c_str())) {
    Serial.println("MDNS responder started");
    Serial.println(storageGetString("deviceName").c_str());
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void webserverLoop() {

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  while ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    //Serial.println(WiFi.status());
    blinkWiFiLoss();
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(2000);
    previousMillis = currentMillis;
  }

  if (WiFi.status() == WL_CONNECTED) {
    blinkWiFiConnect();
    WiFiRSSI = WiFi.RSSI();
    myIP = WiFi.localIP().toString();

    Serial.print("Connected to ");
    Serial.println(storageGetString("WiFissid"));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void clientLoop() {

  server.handleClient();
  // Serial.println("Client loop");
  delay(10);  // allow the cpu to switch to other tasks
}

void handleRoot() {

  char html[3600];
  String icon;
  icon = "&#127780;";

  snprintf(html, 3600,
           "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv='refresh' content='5'/> <style>.max-val span, h3{text-align: center}#pm25-num sup, body{font-family: Arial, Helvetica, sans-serif}html{width: 100vw; height: 100vh}body{margin: 0 30px; background: #001213; background: linear-gradient(126deg, #001213 0, #005157 100%%); color: #eee;}.val-l{font-size: 80px; display: block; margin-bottom: 10px}.pm-other .val{text-align: right}.max-val span{display: inline-block; background-color: #ffffff21; border-radius: 5px; padding: 5px; font-size: 10px; margin: 3px;}.alert{background-color: #00be8e!important}h1{font-size: 16px; margin: 0 0 3px; padding: 0; color: #89ffae}h3{font-size: 22px; margin-top: 0}*{box-sizing: border-box}.flex-container{display: flex; flex-direction: row; text-align: left}.flex-item-left{padding: 10px 20px 10px 0; flex: 50%%; border-right: 1px solid #ffffff52; position: relative;}.flex-item-right{padding: 10px 0 10px 20px; flex: 50%%;}.flex-item-right sup{font-size: 15px}#pm25-num{font-size: 100px; line-height: 100px}#pm25-num sup{font-size: 20px}.emoticon{font-size: 10rem; filter: grayscale(1); opacity: .2; position: absolute; right: 0}@media (max-width:660px){.emoticon{top: 0}.flex-container{flex-direction: column}.flex-item-left{padding: 10px 0; border-right: 0; border-bottom: 1px solid #ffffff52}.flex-item-right{padding: 10px 0}}.container{max-width: 800px; margin: auto;}</style></head><body> <div class=\"container\"> <h3 style=\"display: flex;align-items: center;column-gap: 10px;justify-content: center;\"><span style='font-size:50px;filter: grayscale(1);'>&#9728;</span> %s</h3> <div class=\"flex-container\"> <div class=\"flex-item-left\"> <div class=\"emoticon\">%s</div><div class=\"pm25\" style=\"display: flex;flex-direction: row;justify-content: space-around;flex-wrap: nowrap;\"> <div class=\"\"> <h1>&#127777; Temperature °C</h1> <div class=\"val-l\">%.1f </div><h1>&#9983; Wind speed (km/h)</h1> <span class=\"val-l\">%d</span> </div><div class=\"\"> <h1>HUMI (&percnt;)</h1> <div class=\"val-l\">%.1f </div><h1>&#9954; Wind direction (°)</h1> <span class=\"val-l\">%d</span> </div></div></div><div class=\"flex-item-right\"> <div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY1</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">TEMP > %d</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY2</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">HUMI > %d</span> </div></div></div></div></div><div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY3</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">WINSPD > %d</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY4</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">WINDIR > %d</span> </div></div></div></div></div></div></div><p style=\"text-align: center;\"><b>Domain:</b> %s.local</p></div></body></html>",

           storageGetString("webTitle"), icon, TEMP,WINSPD,HUMI, WINDIR, R1info.stateText, R1info.cssClass, setVar1, R2info.stateText, R2info.cssClass, setVar2, R3info.stateText, R3info.cssClass, setVar3, R4info.stateText, R4info.cssClass, setVar4, storageGetString("deviceName"));
  server.send(200, "text/html", html);
}


void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void configForm() {
  char html[3000];
  snprintf(html, 3000,

           "<html>\
<head>\
<meta charset=\"UTF-8\" />\
<title>System configuration.</title>\
<style>\
   html{width:100%%;height:100%%}body{margin:0 30px;background:#005157;font-family:Arial,Helvetica,sans-serif;font-size:1.8rem;color:#eee}input:not([type=checkbox]){padding:20px;font-size:20px;width:100%%;margin:10px 0;font-weight:700;font-size:2rem;border:0;border-radius:5px;}legend h1{font-size:2.5rem;;color:#7fffd4}input[type=submit]{background-color:#32ab4e;}\
</style>\
</head>\
<body>\
<div style=\"width:95vw;margin:auto;\">\
<h1>System configuration.</h1>\
<form action=\"/saveConfig\" method=\"POST\">\
<fieldset>\
<legend>\
<h1>WiFi Router credential:</h1>\
</legend>\
<h3>SSID:</h3>\
<div><input type=\"text\" name=\"WiFissid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"WiFipassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>Set max values</h1>\
Set value = 0 to disable relay.\
</legend>\
<h3>Relay1:</h3>\
<div><label>Temp (°c)></label><input type=\"number\" min=\"0\" max=\"999\" name=\"set1\" value=\"%d\" size=\"5\"></div>\
<h3>Relay2:</h3>\
<div><label>Humi (&percnt;) ></label><input type=\"number\" min=\"0\" max=\"999\" name=\"set2\" value=\"%d\" size=\"5\"></div>\
<h3>Relay3:</h3>\
<div><label>WindSpd (km/h) ></label><input type=\"number\" min=\"0\" max=\"999\" name=\"set3\" value=\"%d\" size=\"5\"></div>\
<h3>Relay4:</h3>\
<div><label>WindDir (°) > </label><input type=\"number\" min=\"0\" max=\"360\" name=\"set4\" value=\"%d\" size=\"5\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>System setting:</h1>\
</legend>\
<h3>Web title:(No space)</h3>\
<div><input type=\"text\" name=\"web-title\" value=\"%s\" maxlength=\"10\"></div>\
<h3>Device name:</h3>\
<div><input type=\"text\" name=\"device-name\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>WiFi AP mode credential:</h1>\
</legend>\
<h3>SSID:</h3>\
<div><input type=\"text\" name=\"APssid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"APpassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<div><input type=\"submit\" value=\"SAVE\"></div>\
</form>\
<div style=\"text-align: center;\">\
<hr> Ready to use Weather Station Devkit.</div>\
</div>\
</body>\
</html>",

           storageGetString("WiFissid"), storageGetString("WiFipassword"), setVar1, setVar2, setVar3, setVar4, storageGetString("webTitle"), storageGetString("deviceName"), storageGetString("APssid"), storageGetString("APpassword"));
  server.send(200, "text/html", html);
}

void saveConfig() {

  char set1, set2, set3, set4, webserveron;
  String message = "<h3>Data saved.<br></h3><hr>";
  String WIFIssid, WIFIpassword, APssid, APpassword, _webTitle, _deviceName;
  // message += (server.method() == HTTP_GET) ? "GET" : "POST";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " <b>" + server.argName(i) + ":</b> ______" + server.arg(i) + "______<br>\n";
    if (server.argName(i) == "APssid") {
      APssid = server.arg(i);
    } else if (server.argName(i) == "APpassword") {
      APpassword = server.arg(i);
    } else if (server.argName(i) == "WiFissid") {
      WIFIssid = server.arg(i);
    } else if (server.argName(i) == "WiFipassword") {
      WIFIpassword = server.arg(i);
    } else if (server.argName(i) == "set1") {
      set1 = server.arg(i).toInt();
    } else if (server.argName(i) == "set2") {
      set2 = server.arg(i).toInt();
    } else if (server.argName(i) == "set3") {
      set3 = server.arg(i).toInt();
    } else if (server.argName(i) == "set4") {
      set4 = server.arg(i).toInt();
    } else if (server.argName(i) == "web-title") {
      _webTitle = server.arg(i);
    } else if (server.argName(i) == "device-name") {
      _deviceName = server.arg(i);
    }
  }
  message += "<html><head><meta charset=\"UTF-8\" />\<style>\
   html{width:100%%;height:100%%}body{margin:30px;background:#005157;font-family:Arial,Helvetica,sans-serif;color:#eee}\
</style></head><body><script>alert('Saving data please wait until device restarted.');</script><br><br></body></html>";
  server.send(200, "text/html", message);

  storagePutString("APssid", APssid);
  storagePutString("APpassword", APpassword);
  storagePutString("WiFissid", WIFIssid);
  storagePutString("WiFipassword", WIFIpassword);
  storagePutString("webTitle", _webTitle);
  storagePutString("deviceName", _deviceName);
  storagePutInt("setVar1", set1);
  storagePutInt("setVar2", set2);
  storagePutInt("setVar3", set3);
  storagePutInt("setVar4", set4);
  ESP.restart();
}