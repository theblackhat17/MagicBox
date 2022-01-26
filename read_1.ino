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
<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>MagicBox</title>
  

</head>
<body>
<!-- partial:index.partial.html -->
<!DOCTYPE html>
<html>

<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <title>IDPay Q</title>
  <style>
    html {
      background: #f4f4f4;
      font-family: Arial, Helvetica, sans-serif;
      font-size: 16px;
    }

    body {
      margin: 0 auto;
      font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
    }

    .list_wifi_available
    {
      max-width: 400px;
      margin: 0 auto;
      padding: 1em;
    }

    input,
    button,
    #cancel,
    #wifi-list > div {
      border: 1px solid #d3d3d3;
      outline: 0;
      border-radius: 0.6em;
      display: block;
      width: 100%;
      padding: 0.8em;
      margin: 0.4em auto;
      box-sizing: border-box;
      font-size: 16px;
    }

    input:focus {
      border-color: #00cc99;
    }

    button {
      border: 0;
      color: #fff;
      background: #00cc99;
    }

    #submit {
      opacity: 0.7;
      background:#00cc99;
    }

    #cancel {
      border: 0;
      text-align: center;
      cursor: pointer;
    }

    form > div {
      margin-bottom: 1em;
    }

    #scan-form {
      float: right;
      margin-top: 0.95em;
    }

    #scan-form button {
      margin: 0;
      cursor: pointer;
    }

    #wifi-list > div {
      background: #fff;
      padding: 1.2em;
      margin: 1em auto;
    }

    #wifi-list > div:active {
      color: #fff;
      background: #1e90ff;
      border-color: #1e90ff;
    }

    #wifi-list > div:hover {
      background: #00cc99;
      color:white;
      transition: all 0.3s ease-out;
    }

    i {
      float: right;
      background: #000;
      width: 16px;
      height: 10px;
      margin-top: 8px;
      position: relative;
      border-radius: 2px;
    }

    i:before {
      content: '';
      border: 2px solid #000;
      width: 6px;
      height: 6px;
      top: -8px;
      left: 3px;
      position: absolute;
      border-radius: 50% 50% 0 0;
    }

    .hidden {
      display: none;
    }
  </style>
</head>

<body>
  <img src="logo_size.jpg" class="img_logo"/>
  <div class="list_wifi_available">       
    <form id='scan-form' method='GET' action='/scan'>
      <button type='submit' form='scan-form'>Scan WIFI</button>
    </form>
    <h2>Networks</h2>

    <div id='wifi-list'>

      <div onclick='ssidClick(this)'>
        <span>Yncrea_Lab</span>
        <i></i>
      </div>

    </div>)rawliteral";







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
      request->send(200, "text/html","<form action='/end'><select name='SSID'>" + message + "</select>Password: <input type='text' name='password'>Raspberry:<input type='text' name='rpi'><input type='submit' value='Submit'></form>" );
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
      //Serial.println(rpi);

      const char index_html[] PROGMEM = R"rawliteral(
            <script>
        var $ = function (id) {
          return document.getElementById(id);
        };

        function ssidClick(e) {
          $('ssid').value = e.getElementsByTagName('span')[0].innerText;

          if (e.getElementsByTagName('i').length) {
            hideWifi();
          } else {
            $('pass').value = ' ';
            $('submit').click();
          }
        }

        function showWifi() {
          $('wifi-form').classList.add('hidden');
          $('scan-form').classList.remove('hidden');
          $('wifi-list').classList.remove('hidden');
        }

        function hideWifi() {
          $('wifi-list').classList.add('hidden');
          $('scan-form').classList.add('hidden');
          $('wifi-form').classList.remove('hidden');
          $('pass').focus();
        }
      </script>
      </body>
      </html>
      <!-- partial -->
        
      </body>
      </html>)rawliteral";
      
      request->send(200, "text/html", "<h1>Merci :) reboot</h1>");

  });
    
}

/**     
 * 
 * const char index_html[] PROGMEM = R"rawliteral(
      <!DOCTYPE HTML><html><head>
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

      **/

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
