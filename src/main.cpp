#include <header.h>

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BotToken, secured_client);

void setup() {
  Serial.begin(11200);
  WiFi.begin(ssid, password);
  while( WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi is not WIFIing");
    delay(10000);
  }
  // display connected when it has connected
  if( WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI is WIFIing");
  }

  secured_client.setInsecure(); //im not interested in the response certificate
}

void loop() {
  // put your main code here, to run repeatedly:


}