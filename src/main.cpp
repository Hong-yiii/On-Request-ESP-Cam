#include <header.h>
#define Debugging_mode_on

void HandleNewMessages(int numNewMessages);





WiFiClientSecure secured_client;
UniversalTelegramBot bot(BotToken, secured_client);
unsigned long last_bot_poll_time; // last time the API poll has been made

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
  if (millis() - last_bot_poll_time > 20000) { // 20 seconds after previous poll
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      
      #ifdef Debugging_mode_on
      Serial.println("Response recieved from API, new Messages have been received");
      #endif

      HandleNewMessages(numNewMessages);

    }
  }


}