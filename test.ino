#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <iostream>
#include <cstring>

using namespace std;

DNSServer dnsServer;
AsyncWebServer server(80);



String message;

int i = 0;
int network;


String ssid;
String password;
String rpi;

bool receivedSSID = false;
bool receivedPASSWORD = false;
bool receivedRPI = false;




const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>head
  <title>Captive Portal Demo</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>HotSpot Groupe B</h3>
  <h1> Bienvenue sur le menu de configuration de votre objet connecte ! </h1>
  <form action="/get">
    <br>
    Assurez vous d'être a la portee de votre reseaux.
    <br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";







class CaptiveRequestHandler : public AsyncWebHandler {
  public:CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }
  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); 
  }
};



void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", index_html); 
      Serial.println("Client Connected");
  });
    
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      request->send(200, "text/html","<form action='/end'><span>" + message + "</span><i></i><div> <label for='pass'>Password:</label><input id='pass' name='pass' type='password' minlength='8' maxlength='64' required></div><div><label for='pass'>Adresse IP de la Raspberry:</label><input id='pass' name='ip' type='text' maxlength='15' required></div> <input type='submit' value='Submit'></form>" );
  });

  server.on("/end", HTTP_GET, [] (AsyncWebServerRequest *request) {
      if (request->hasParam("SSID")) {
          ssid = request->getParam("SSID")->value();
          receivedSSID = true;
    }
      if (request->hasParam("password")) {
          password = request->getParam("password")->value();
          receivedPASSWORD = true;
    }
      if (request->hasParam("rpi")) {
            rpi = request->getParam("rpi")->value();
            receivedRPI = true;
    }
      Serial.println(ssid);
      Serial.println(password);
      Serial.println(rpi);
    
      request->send(200, "text/html", "<h1>Merci :) reboot</h1>");
  });
  
  
  
}

void SetListWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  network = WiFi.scanNetworks();
  Serial.println(network);
  
  for ( i ; i < network; ++i) {
      //Reason  for Post Increment is to Increment the value then Assign it to "i"
      //Print the SSID (name) and RSSI (Signal Strenght)
      Serial.print(i + 1);
      Serial.print(":");
      //prints the Individual SSID
      Serial.print(WiFi.SSID(i));
      message +="<option value='" + String (WiFi.SSID(i)) + "'>" + String (WiFi.SSID(i))+ "</option>"; 
  }
  Serial.println("**Scan fini**");
  Serial.println("    ");
  Serial.println(message);
  
}


void setup(){
  //your other setup stuff...
  Serial.begin(115200);
  SetListWiFi();
  delay(1000);
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("WIFI MAES");
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");
}

void SetupConnexion(){
  WiFi.mode(WIFI_STA);

  const char * ssid_ = ssid.c_str();
  const char * password_ = password.c_str();
  const char * rpi_ = rpi.c_str();
  Serial.println(ssid_);

  
  WiFi.begin(ssid_, password_);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }
    
      Serial.println("\nConnected to the WiFi network");
      Serial.print("Local ESP32 IP: ");
      Serial.println(WiFi.localIP());
  
}
void loop(){
  dnsServer.processNextRequest();
  if ((receivedPASSWORD == true) && (receivedSSID == true) && (receivedRPI == true)){
    SetupConnexion();
  }
}
