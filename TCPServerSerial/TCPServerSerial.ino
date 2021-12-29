#include <WiFi.h>
#include <arduino-timer.h>

const char *ssid = "EngineControl";

WiFiServer server(23);
auto sendTimer = timer_create_default();

void setup() {
  Serial.begin(115200);
  startAP();
  // sendTimer.every(1000, sendData);  
}

void loop() {
  WiFiClient client = server.available();
  if(client){
    Serial.println("New Client");
    String currentLine = "";
    Serial.println(currentLine);
    while(client.connected()){
      if(client.available()){
        char c = client.read();
        Serial.write(c);
        if(c == '\n' || c == '\r'){
          currentLine = "";
        }else{
          currentLine += c;
        }
        if(currentLine == "exit"){
          break;
        }else if(currentLine == "42"){
          client.print("The meaning of life, the universe and everything");
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }

  // sendTimer.tick();
}

void startAP(){
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  Serial.print("AP IP:");
  Serial.println(WiFi.softAPIP());
  server.begin();
  Serial.println("Server started");
}

bool sendData(void *){
  Serial.println("Working");
  return true;
}