#include <CircularBuffer.h>
#include <Wire.h>
#include <WiFi.h>

#define DEBUG_PRINTS 1
#define SERIAL_ECHO 0
//For I2C
#define slaveADDR 0x04

//For AP
const char *apSSID = "EngineControl";
WiFiServer server(23);
WiFiClient client;

const char END_TOKEN = (char)0xC0;

CircularBuffer<char, 1024> txCommsBuffer; //Buffer for incoming commands from PC

void setup() {
  Serial.begin(115200);
  Wire.setClock(400000);
  Wire.onReceive(receiveEventI2C);
  Wire.onRequest(requestEventI2C);
  Wire.begin((uint8_t)slaveADDR);
  startAP();
}

void loop() {
  commsHandler();
}

void receiveEventI2C(int numBytes){
  // String line = "";
  while (Wire.available()) {
    // line += (char)Wire.read();
    if(client){
      char c = (char)Wire.read();
      client.write(c);
        #if DEBUG_PRINTS == 1
        Serial.print("I2C receive: ");
        Serial.write(c);
        #endif
    }else{
      #if DEBUG_PRINTS == 1
      Serial.println("No client connected");
      #endif
    }
  }
}

void requestEventI2C(){
  #if DEBUG_PRINTS == 1
  Serial.print("request");
  #endif
  while(!txCommsBuffer.isEmpty()){
    if(txCommsBuffer.first() == END_TOKEN){
      #if DEBUG_PRINTS == 1
      Serial.println("end token");
      #endif
      Wire.write(txCommsBuffer.shift());
      break;
    }
    #if DEBUG_PRINTS == 1
    Serial.println("writing");
    #endif
    Wire.write(txCommsBuffer.shift());
  }
}

void commsHandler(){
  client = server.available();
  if(client){
    Serial.println("New Client");
    // String currentLine = "";
    while(client.connected()){
      if(client.available()){
        if(!readWifi(client)){ break; } //Disconnect if false
      }
      // serialHandler();
    }
    client.stop();
    Serial.println("Client disconnected");
  }else{
    // serialHandler();
  }
}

//Read wifi for commands
bool readWifi(WiFiClient client){
  // static char currentLine[1024];
  txCommsBuffer.push(client.read());

  #if DEBUG_PRINTS == 1
    Serial.write(txCommsBuffer.last());
  #endif

  // currentLine += c;
  // if(c == END_TOKEN){
  //   #if DEBUG_PRINTS == 1
  //     Serial.println("Current line: " + currentLine);
  //   #endif
    
  //   if (!handleCommands(client, currentLine)){
  //     return false;                                         //Disconnect if return false
  //   }
  //   currentLine = "";
  // }
  return true;
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
      while(!txCommsBuffer.isEmpty()){
        if(txCommsBuffer.first() == END_TOKEN){
          Serial.print(txCommsBuffer.shift());
          break;
        }
        Serial.print(txCommsBuffer.shift());
      }
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
  if(line == "exit\r\n"){
    return false;
  }else if(line == "shiftComm\r\n"){
    while(!txCommsBuffer.isEmpty()){
        if(txCommsBuffer.first() == END_TOKEN){
          Serial.print(txCommsBuffer.shift());
          break;
        }
        Serial.print(txCommsBuffer.shift());
      }
  }else{
    for (int i = 0; i < line.length(); i++)
    {
      txCommsBuffer.push(line[i]);
    }
  }
  return true;
}