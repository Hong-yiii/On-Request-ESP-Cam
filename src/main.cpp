#include <header.h>
#define Debugging_mode_on

void HandleNewMessages(int numNewMessages);

bool UpdateSingularPhoto(String chat_id);
bool UpdateRecurringPhoto(String chat_id);


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
      numNewMessages = bot.getUpdates(bot.last_message_received + 1); //offset so that it checkes the next "update" in the API for a return
    }
    last_bot_poll_time = millis();
    #ifdef Debugging_mode_on
    Serial.println("Last_bot_poll_time is: " + last_bot_poll_time);
    #endif
    // log the completion of the function? TBC

  }
  delay(5000);

  //Check for logs and memory use, clear accordinly to conserve memory
  #ifdef Debugging_mode_on
  // Serial.print if logs were cleared or not cleared
  #endif

}

void HandleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String UserText = bot.messages[i].text;
    // chat_id is the identify user and usertext is user's text input 

    if (UserText == "/help") {
      String HelpMessage = "1. /function 1 (...) \n"
                           "2. /function 1 (...) \n"
                           "3. /function 1 (...) \n";
      bool BotMessageStatus = bot.sendMessage(chat_id, HelpMessage, ""); //return value is if the messages sent succesfully

      #ifdef Debugging_mode_on
      if (BotMessageStatus) {
        Serial.println("Bot reply to /help sent successfully");
      } else {
        Serial.println("Bot FAILED to reply to /help");
      }
      #endif

      // Implement Logging Accordingly "/help during" + TIME + ", bot reply status" + BotMessageStatus
    }

    if (UserText == "/update") {
      
      bool BotUpdateStatus = UpdateSingularPhoto(chat_id);

      #ifdef Debugging_mode_on
      if (BotUpdateStatus) {
        Serial.println("Bot sent photo update successfully");
      } else {
        Serial.println("Bot FAILED to send Photo update");
      }
      #endif

      // Implement Logging Accordingly "/update during" + TIME + ", bot reply status" + BotMessageStatus
    }


    if (UserText == "/RecurringUpdate") {
      
      bool BotUpdateStatus = UpdateSingularPhoto(chat_id);

      #ifdef Debugging_mode_on
      if (BotUpdateStatus) {
        Serial.println("Bot sent photo update successfully");
      } else {
        Serial.println("Bot FAILED to send Photo update");
      }
      #endif

      // Implement Logging Accordingly "/update during" + TIME + ", bot reply status" + BotMessageStatus
    }
  }
}

bool UpdateSingularPhoto(String chat_id) {

}

bool UpdateRecurringPhoto(String chat_id) {
  // Send message asking for 1st input --> update the states in enum
  // repeatedly try to recieve numNewMessages until timeout --> HandleNewMessages to retrive reply
  // Second message asking for 2nd input --> update state to pending input --> Handle New Messages to retrieve reply --> update state back to normal state
  // actually call function --> check for new message, if numNewMessages !=0 , use as interrupt

  long ms_between_updates;
  long update_duration_in_ms;
  // have to implement states int the funcion

  
}



