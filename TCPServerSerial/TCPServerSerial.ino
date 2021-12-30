#include <WiFi.h>

#define DEBUG_PRINTS 1

//For AP
const char *apSSID = "EngineControl";
WiFiServer server(23);

const char END_TOKEN = '\r';

void setup() {
  Serial.begin(115200);
  startAP();
}

void loop() {
  wifiHandler();
}

void wifiHandler(){
  WiFiClient client = server.available();
  if(client){
    Serial.println("New Client");
    String currentLine = "";
    while(client.connected()){
      if(client.available()){
        char c = client.read();

        #if DEBUG_PRINTS == 1
          Serial.write(c);
        #endif

        if(c == END_TOKEN){

          #if DEBUG_PRINTS == 1
            Serial.println("Current line: " + currentLine);
          #endif
          
          if (!handleCommands(client, currentLine)){
            break;                                         //Disconnect if return false
          }
          currentLine = "";
          while (client.available()){ c = client.read(); } //Flush remaining chars
        }else{
          currentLine += c;
        }
      }
      serialHandler();
    }
    client.stop();
    Serial.println("Client disconnected");
  }else{
    serialHandler();
  }
}

//For debugging
void serialHandler(){
  if(Serial.available()){
    String rxMessage = "";
    while(Serial.available()){
      rxMessage += (char)Serial.read();
    }

    #if DEBUG_PRINTS == 1
    Serial.println("serial echo: " + rxMessage);
    #endif
  }
}

//Start a access point
void startAP(){
  WiFi.mode(WIFI_AP);
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.softAP(apSSID);
  Serial.print("AP IP:");
  Serial.println(WiFi.softAPIP());
  server.begin();
  Serial.println("Server started");
}

bool handleCommands(WiFiClient client, String line){
  if(line == "exit"){
    return false;
  }else if(line == "42"){
    client.println("The meaning of life, the universe and everything");
  }
  return true;
}