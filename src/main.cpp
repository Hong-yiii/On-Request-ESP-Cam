#include <header.h>
#define Debugging_mode_on

void HandleNewMessages(int numNewMessages);

bool UpdateSingularPhoto(String chat_id);
bool UpdateRecurringPhotoSetup(String chat_id, int numNewMessages);
void HandleTimeout();

enum botStateDtype {
  AWAITING_COMMAND,
  DEFAULT_POLLING_STATE,
  EXECUTING_COMMAND
};

enum botCommandType {
  IMAGE_UPDATE_DURATION_QUERY,
  IMAGE_UPDATE_INTERVAL_QUERY,
  ERROR_NOT_EXPECTING_INPUT
};

enum botExecuteType {
  NULL_EXECUTE,
  EXECUTING_RECURRING_UPDATE
};

botStateDtype BotState = DEFAULT_POLLING_STATE; //initialize to default state
botCommandType ExpectedCommandReturn = ERROR_NOT_EXPECTING_INPUT; //initialize to first query
botExecuteType BotCurrentExecute = NULL_EXECUTE;

String chat_id;
unsigned long StartInputTime;
const unsigned long InputTimeout = 60000; // Timeout in milliseconds (1 minute)

unsigned long ImageUpdateDurationInMs;
unsigned long ImageUpdateIntervalInMs;
unsigned long RecurringUpdateStartTime; // millis - StartTime > Duration
unsigned long LastPhotoUpdateTime; //log when the photo is sent


WiFiClientSecure secured_client;
UniversalTelegramBot bot(BotToken, secured_client);
unsigned long last_bot_poll_time; // last time the API poll has been made

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while( WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi is not WIFIing");
  }
  // display connected when it has connected
  if( WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI is WIFIing");
  }

  secured_client.setInsecure(); //im not interested in the response certificate
  last_bot_poll_time = millis(); //initialize last_bot_poll_time
  #ifdef Debugging_mode_on
  Serial.println(last_bot_poll_time);
  #endif
  BotState = DEFAULT_POLLING_STATE;
  ExpectedCommandReturn = ERROR_NOT_EXPECTING_INPUT;
  BotCurrentExecute = NULL_EXECUTE;
  esp_task_wdt_init(WDTO_8S, true); // Set watchdog timeout for 8 seconds and enable panic handler
}

void loop() {
  //default polling state
  #ifdef Debugging_mode_on
  long current_milis = millis();
  Serial.println(current_milis);
  delay(2000);
  esp_task_wdt_reset();
  #endif

  if (millis() - last_bot_poll_time > 20000 && BotState == DEFAULT_POLLING_STATE) { // 20 seconds after previous poll if in normal polling state
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages > 0) {
      
      #ifdef Debugging_mode_on
      Serial.println("Response recieved from API, new Messages have been received");
      #endif

      HandleNewMessages(numNewMessages);
    }
    last_bot_poll_time = millis();

    #ifdef Debugging_mode_on
    Serial.println("Bot State is DEDAULT_POLLING_STATE \n Last_bot_poll_time is: " + last_bot_poll_time);
    #endif
    // log the completion of the function? TBC

  //Check for logs and memory use, clear accordinly to conserve memory
  #ifdef Debugging_mode_on
  // Serial.print if logs were cleared or not cleared
  #endif
  }
  // Polling when waiting for new command, check for update every second from user
  if (BotState == AWAITING_COMMAND && millis() - last_bot_poll_time > 2000) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages > 0) {
      HandleNewMessages(numNewMessages);
    } else if (millis() - StartInputTime > InputTimeout && BotState == AWAITING_COMMAND) {
      // handle timeout
      #ifdef Debugging_mode_on
      Serial.println("timedout, calling timeout function");
      #endif
      HandleTimeout();
      last_bot_poll_time = millis();
    }
  }

  //Polling when executing command
  if (BotState == EXECUTING_COMMAND) {
    switch(BotCurrentExecute) {
        case EXECUTING_RECURRING_UPDATE:
          if (millis() - LastPhotoUpdateTime > ImageUpdateIntervalInMs) {
            bool PhotoUpdateStatus = UpdateSingularPhoto(chat_id);
            LastPhotoUpdateTime = millis();
            
            #ifdef Debugging_mode_on
            Serial.println("Photo updated Succesfully at: " + LastPhotoUpdateTime);
            Serial.println("ImageUpdateIntervalInMs = " + ImageUpdateDurationInMs);
            #endif
            // log photo sending time and PhotoUpdateStatus
          }

          if (millis() - RecurringUpdateStartTime > ImageUpdateDurationInMs) {
               
            #ifdef Debugging_mode_on
            Serial.println("Recurring Update Function is done");
            #endif
            
            BotState = DEFAULT_POLLING_STATE;
            BotCurrentExecute = NULL_EXECUTE;
            
          }
          break;

        case NULL_EXECUTE: 

          #ifdef Debugging_mode_on
          Serial.println("NULL_EXECUTE, there has been a wrongly handled state");
          #endif 

          break; 
        
        default:

          #ifdef Debugging_mode_on
          Serial.println("fell through to default, Mishandled State");
          #endif 

          break; 

      }
  }
}

void HandleNewMessages(int numNewMessages) {
  // respond based of BotState
  switch(BotState) {
    case DEFAULT_POLLING_STATE:
    {
      if (numNewMessages > 0) {
        int lastMessageIndex = numNewMessages - 1; //we are only going to process the latest text in the batch
        chat_id = String(bot.messages[lastMessageIndex].chat_id);
        String UserText = bot.messages[lastMessageIndex].text;
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
        } else if (UserText == "/update") {
          
          bool BotUpdateStatus = UpdateSingularPhoto(chat_id);

          #ifdef Debugging_mode_on
          if (BotUpdateStatus) {
            Serial.println("Bot sent photo update successfully");
          } else {
            Serial.println("Bot FAILED to send Photo update");
          }
          #endif

          // Implement Logging Accordingly "/update during" + TIME + ", bot reply status" + BotMessageStatus
        } else if (UserText == "/RecurringUpdate") {
          
          #ifdef Debugging_mode_on
          Serial.println("Calling UpdateRecurringPhotoSetup");
          #endif

          bool BotUpdateStatus = UpdateRecurringPhotoSetup(chat_id, numNewMessages);

          // Implement Logging Accordingly "/update during" + TIME + ", bot reply status" + BotMessageStatus
        } else {
          //no matching command
          String HelpMessage = "1. /function 1 (...) \n"
                              "2. /function 1 (...) \n"
                              "3. /function 1 (...) \n";
          bot.sendMessage(chat_id, HelpMessage, "");

          #ifdef Debugging_mode_on
          Serial.println("input not recognised");
          //log no matching command
          #endif

        }
      }
      break; //break out of DEFAULT_POLLING_STATE      
    }

  
    case AWAITING_COMMAND:
    {
      int lastMessageIndex = numNewMessages - 1; //we are only going to process the latest text in the batch
      String chat_id = String(bot.messages[lastMessageIndex].chat_id);
      String UserText = bot.messages[lastMessageIndex].text;
      // Awaiting command will behave differently based off ExpectedCommandReturn
      switch(ExpectedCommandReturn) {
        case IMAGE_UPDATE_DURATION_QUERY:
        {
          
          #ifdef Debugging_mode_on
          Serial.println("Calling updatreSingularPHotoSetup");
          #endif

          bool BotUpdateStatus = UpdateRecurringPhotoSetup(chat_id, numNewMessages);

          #ifdef Debugging_mode_on
          if (BotUpdateStatus) {
            Serial.println("Bot State update successfully");
          } else {
            Serial.println("Bot State Failed");
          }
          #endif
          
          break;
        }
        case IMAGE_UPDATE_INTERVAL_QUERY:
        {
          //handle the return val
                    
          #ifdef Debugging_mode_on
          Serial.println("Calling updatreSingularPHoto");
          #endif

          bool BotUpdateStatus = UpdateRecurringPhotoSetup(chat_id, numNewMessages);

          #ifdef Debugging_mode_on
          if (BotUpdateStatus) {
            Serial.println("Bot State update successfully");
          } else {
            Serial.println("Bot State FAILED ");
          }
          #endif
          
          break;
        }
      }
    }
    
  }
}


void HandleTimeout() {
  String chat_id = String(bot.messages[0].chat_id); // Assuming the last chat to interact
  bot.sendMessage(chat_id, "Response timed out. Please start again");
  BotState = DEFAULT_POLLING_STATE; // Reset states
  ExpectedCommandReturn = ERROR_NOT_EXPECTING_INPUT;
}


bool UpdateSingularPhoto(String chat_id) {

  #ifdef Debugging_mode_on
  Serial.println("Sending 1x Photo!");
  #endif 

  return true;
}

bool UpdateRecurringPhotoSetup(String chat_id, int numNewMessages) {
  String ImageUpdateDurationInMin;
  String ImageUpdateIntervalInMin;
  int lastMessageIndex;


  switch(ExpectedCommandReturn) {
    case ERROR_NOT_EXPECTING_INPUT: //first input after /updaterecurringphoto
      bot.sendMessage(chat_id, "How long do you want to be updated for (min) ?");
      StartInputTime = millis();
      BotState = AWAITING_COMMAND;
      ExpectedCommandReturn = IMAGE_UPDATE_DURATION_QUERY;

      #ifdef Debugging_mode_on
      Serial.println("ExpectedCommmandReturnState has been updaated to IMAGE_UPDATE_DURATION_QUERY");
      #endif

      break;

    case IMAGE_UPDATE_DURATION_QUERY:  //second input
      //process the user message input and write to the global var
      lastMessageIndex = numNewMessages - 1;
      ImageUpdateDurationInMin = bot.messages[lastMessageIndex].text;
      ImageUpdateDurationInMs = ImageUpdateDurationInMin.toInt() * 60000;
      
      bot.sendMessage(chat_id, "What is your desired interval between updates (min) ?");
      StartInputTime = millis();
      ExpectedCommandReturn = IMAGE_UPDATE_INTERVAL_QUERY;

      #ifdef Debugging_mode_on
      Serial.println("ExpectedCommmandReturnState has been updaated to IMAGE_UPDATE_INTERVAL_QUERY");
      #endif

      break;

    case IMAGE_UPDATE_INTERVAL_QUERY:
      //process the second message input and write to global var
      lastMessageIndex = numNewMessages - 1;
      ImageUpdateIntervalInMin = bot.messages[lastMessageIndex].text;
      ImageUpdateIntervalInMs = ImageUpdateIntervalInMin.toInt() * 60000;
      
      StartInputTime = millis();

      #ifdef Debugging_mode_on
      Serial.println("ExpectedCommmandReturnState has been updaated to ERROR_NOT_EXPECTING_INPUT");
      #endif

      ExpectedCommandReturn = ERROR_NOT_EXPECTING_INPUT;
      BotState = EXECUTING_COMMAND;
      BotCurrentExecute = EXECUTING_RECURRING_UPDATE;
      RecurringUpdateStartTime = millis();
      break;


  }



  // repeatedly try to recieve numNewMessages until timeout --> HandleNewMessages to retrive reply
  // Second message asking for 2nd input --> update state to pending input --> Handle New Messages to retrieve reply --> update state back to normal state
  // actually call function --> check for new message, if numNewMessages !=0 , use as interrupt
  
  #ifdef Debugging_mode_on
  Serial.println("UpdateRecurringPhotoSetup fn is done");
  #endif

  return true;
}



