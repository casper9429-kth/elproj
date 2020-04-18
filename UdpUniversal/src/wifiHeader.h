#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

WiFiUDP Udp; //Declares an object of type WiFiUDP

//Buffers to save udp data, not needed neccesarily needed but good to have as default buffers
String packetBuffer; //Save incoming data in this buffer
String replyBuffer;  //Save outgoing data in this buffer

//Hardcoded default settings: 

String ssid = "ESP32";  
String password = "ESP32"; 
unsigned int localPort = 8888; //For udp
IPAddress serverIP(192,168,4,1); //Only used by client, 192.168.4.1 is default localIp for an ESP32 in AP mode

//Fördefinerade funktioner
bool getUdpPacket(String &buffer = packetBuffer,int timeOut = 100); //maximum allowed delay waiting for a new UDP paket is 100 ms, need to take in regard that a package might be lost in air 
void sendUdpPackage(String content = replyBuffer, IPAddress address = Udp.remoteIP(),int port = Udp.remotePort());
bool startAp(String networkName = ssid,String networkPassword = password,bool hidden=false);
bool connectToWiFi(String networkName=ssid,String networkPassword=password,int timeOut = 3000);

bool startAp(String networkName,String networkPassword,bool hidden){ //Return true if AP started successfully, else false
  Serial.println("Start Ap");
  if(WiFi.softAP(networkName.c_str(), networkPassword.c_str(),1,int(hidden))){ //If ap started successfully
    Serial.println("- AP started successfully");
    Serial.printf("- ssid: %s\n", networkName.c_str());
    Serial.printf("- password: %s\n",networkPassword.c_str());
    Serial.printf("- IP: %s\n",WiFi.softAPIP().toString().c_str());
    return true;
  }
  Serial.println("- AP failed");
  return false;
}

bool connectToWiFi(String networkName,String networkPassword,int timeOut){
  //Ansluter till WiFi
  Serial.printf("Starts connection to wifi\n ssid: %s\n  password: %s\n",networkName.c_str(),networkPassword.c_str());
  WiFi.begin(networkName.c_str(),networkPassword.c_str());
  
  int latestMillis = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    if(millis() > latestMillis + timeOut){
      Serial.println("- WiFi connection failed, timeout.");
      WiFi.disconnect();
      return false;
    }
    Serial.print(".");
    delay(100);
  }

  Serial.printf(": connected \n- IP: %s\n",WiFi.localIP().toString().c_str());
  return true;
}


bool getUdpPacket(String &buffer, int timeOut){ //gets incoming paket into buffer and writes content to serial, return true if there was a new package, false if not

  int packetSize = Udp.parsePacket(); //packetSize = sizeOfNextPacketInStack, 0 if there is not next package
  int latestMillis = millis(); //Start timer

  //Wait to receive paket
  while (packetSize == 0 && millis() < timeOut + latestMillis){ // timeOut after 300 ms
    packetSize = Udp.parsePacket();
  }

  //if a new package wass found in privious step
  if(packetSize != 0) { //if paketSize != 0

    //Reads package into buffer(package into tempBuffer, tempBuffer into buffer)
    char tempBuffer[3999]; //might be to much, 800 works for sure
    Udp.read(tempBuffer,sizeof(tempBuffer));
    buffer = tempBuffer;

    Serial.printf("package recived:\n- IP: %s\n- port: %i\n",Udp.remoteIP().toString().c_str(),Udp.remotePort()); 
    //Serial.printf("\n%s\n",buffer.c_str()); //uncomment for debugging
    return true;
  }
  else{

    return false;

  }
}

void sendUdpPackage(String content, IPAddress address,int port){ //(packet, ip, port): should check that esp is connect if esp32 acts as wifiClient
  
    Udp.beginPacket(address,port);
    Udp.print(content.c_str());
    Udp.endPacket();

    Serial.printf("Package send:\n- IP: %s\n- port: %i\n", address.toString().c_str(),port);
    //Serial.printf("\n%s\n",content.c_str()); //Uncomment for debugging

}



