/*This file is for prototype purpose*/ 
#include <Arduino.h>
#include <stdio.h>
#include <wifiHeader.h>

//These two bools are only used in setup and loop for easy debugging. Should be removed in none prototype code
bool Ap = false;      //If true esp acts as ap, if false esp act as client
bool server = false;  //If true esp acts as UDP server, if false esp act as UDP client

double tLatestUpdate;  //Timing

void setup(){
  //Start serial
  Serial.begin(115200);
  Serial.printf("\nESP32\nAP: %i\nserver: %i\n",Ap,server);

  if(Ap){ //If WiFi AP
    if(!startAp()){ 
      ESP.restart();
      }
  }
  else{ //else WiFi client
    if(!connectToWiFi()){ 
      ESP.restart();
    }
  }
  
  //replyBuffer = "hello"; //Uncomment for debugging

  Serial.printf("UDP begins at port:  %i\n",localPort);  
  Udp.begin(localPort);
}

void loop() {

  if(true/*millis() - tLatestUpdate > 1000*/) //Runs in intervals of xxx..x ms
  {

    if(server){ //If server: server always gets package before sending
      
      if(getUdpPacket())  //If there are a new package 
      {
        sendUdpPackage();  //Send a package back 
      }
    }
    else //else client: Client always sendsPackage before gettings
    {

      if(WiFi.status() == WL_CONNECTED) //If WiFi is connected
      {
        sendUdpPackage(replyBuffer,serverIP,localPort); //send a package
        getUdpPacket(packetBuffer,200); //Download a package into packetBuffer. Waits for a maximum of 200 ms before giving up   
      }
      else //wifi not connected
      {
        Serial.println("Connection Lost"); //print in serial
        connectToWiFi(); //try to reconnect
      }
    }


    tLatestUpdate = millis(); //resets tLatestUpdate
  }

}