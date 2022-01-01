#include <CircularBuffer.h>
#include <Wire.h>
#include <WiFi.h>

#define DEBUG_PRINTS 0
#define SERIAL_ECHO 0

//For AP
const char *apSSID = "EngineControl";
WiFiServer server(23);

//For I2C
const int slaveADDR = 4;

const char END_TOKEN = '\r';

CircularBuffer<char, 1000> txCommsBuffer; //Buffer for incoming commands from PC

void setup() {
  Serial.begin(115200);
  Wire.begin(slaveADDR);
  Wire.onReceive(receiveEventI2C);
  startAP();
}

void loop() {
  commsHandler();
}

//TODO implement I2C receive event
void receiveEventI2C(int howMany){

}

void commsHandler(){
  WiFiClient client = server.available();
  if(client){
    Serial.println("New Client");
    // String currentLine = "";
    while(client.connected()){
      if(client.available()){
        if(!readWifi(client)){ break; } //Disconnect if false
      }
      serialHandler();
    }
    client.stop();
    Serial.println("Client disconnected");
  }else{
    serialHandler();
  }
}

//Read wifi for commands
bool readWifi(WiFiClient client){
  static String currentLine = "";
  char c = client.read();

  #if DEBUG_PRINTS == 1
    Serial.write(c);
  #endif

  currentLine += c;
  if(c == END_TOKEN){

    #if DEBUG_PRINTS == 1
      Serial.println("Current line: " + currentLine);
    #endif
    
    if (!handleCommands(client, currentLine)){
      return false;                                         //Disconnect if return false
    }
    currentLine = "";
  }
  return true;
}

//TODO Send data back over wifi
void sendWifi(WiFiClient client){
  
}

//TODO Implement I2C send
void sendI2C(){

}
//For debugging
void serialHandler(){
  if(Serial.available()){
    String rxMessage = "";
    while(Serial.available()){
      rxMessage += (char)Serial.read();

    }
    if(rxMessage == "shift"){
      Serial.print(txCommsBuffer.shift());

    }else if(rxMessage == "shiftComm"){
      while((txCommsBuffer.first() != END_TOKEN) && (txCommsBuffer.first() != NULL)){
        Serial.print(txCommsBuffer.shift());
      }
      txCommsBuffer.shift();
    }else if(rxMessage == "size"){
      Serial.println("\nCommands in buffer:" + (String)txCommsBuffer.size());
      
    }

    #if SERIAL_ECHO == 1
    Serial.println("\necho: " + rxMessage);
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
  }else{
    for (int i = 0; i < line.length(); i++)
    {
      txCommsBuffer.push(line[i]);
    }
  }
  return true;
}