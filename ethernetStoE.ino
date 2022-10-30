#include <Ethernet.h>
#include <SPI.h>
#include "./src/Artila-Matrix310.h"
#include "./src/ethernetStoE.h"
#include "./src/StoE.h"
// #include "./src/connection.h"

bool printWebData = true; // set to false for better speed measurement
EthernetClient client;
IPAddress server(SERVER);//要連的SERVER 
byte mac[] = {MAC}; // mac
IPAddress ip(IP);
IPAddress myDns(MYDNS);
void initGPIO()
{
  pinMode(COM2_RX, OUTPUT); // 232RX
  pinMode(COM2_TX, INPUT);  // 232TX
  Ethernet.init(LAN_CS);    // MKR ETH Shield
}
void setup()
{
  initGPIO();
  Serial.begin(115200);

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0)//板子嘗試用DHCP連網
  {
    //DHCP連網失敗
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  }
  else
  {//板子成功用DHCP連上網
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, PORT) == 1) 
  {
    //板子連上server
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  StoE();
}

void loop()
{
  Serial.println(client.available());
  if (!Serial.available()) // serial沒東西
  {
   Serial.println("Waiting for server......"); 
    while (!client.available()) //等待server回覆
    {
      // Serial.println(client.available());
      // delay(3000);
      // break;
    }
    Serial.println("Server return: ");
    int len = client.available();
    if (len > 0)
    {
      byte buffer[80];
      if (len > 80)
        len = 80;
      client.read(buffer, len);
      if (printWebData)
      {
        Serial.write(buffer, len); // show in the serial monitor (slows some boards)
      }
    }
    // Serial.println("");
  }
  if (client.available() == 0 && Serial.available() > 0)
  {
    //server丟完了 且 Serial有東西輸入
    StoE(); //serial輸入
  }
}