#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyAUWlHbiETZjEFrjyvg124_GomK8wx9HMU"
#define DATABASE_URL "https://revive-smart-home-692c2-default-rtdb.europe-west1.firebasedatabase.app/" 
int RELAY_PIN = 2;

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;

bool signupOK = false;


bool stage = true;

void setup() {
  Serial.begin(115200);
}

void loop() {
  delay(5000);
  if(stage){
    if (Serial.available()) {
      String inputString = Serial.readString();
      inputString.trim();
      //input data format ssid/password/username/device

      String ssid;
      String password;
      String userName;
      String deviceName;
      int delimiterIndex1 = inputString.indexOf("/");
      int delimiterIndex2 = inputString.indexOf("/", delimiterIndex1 + 1);
      int delimiterIndex3 = inputString.indexOf("/", delimiterIndex2 + 1);

      
      if (delimiterIndex1 >= 0 && delimiterIndex2 >= 0 && delimiterIndex3 >= 0) {
        ssid = inputString.substring(0, delimiterIndex1);
        password = inputString.substring(delimiterIndex1 + 1, delimiterIndex2);
        userName = inputString.substring(delimiterIndex2 + 1, delimiterIndex3);
        deviceName = inputString.substring(delimiterIndex3 + 1);
      } else {
        ssid = inputString;
        password = "";
        userName = "";
        deviceName = "";
      }

      
      String pathToData = "/users/" + userName + "/" + deviceName + "/" + "stage";
      Serial.println("SSID: " + ssid);
      Serial.println("Password: " + password);

      char* passwordFinal = (char*) password.c_str();
      char* ssidFinal = (char*) ssid.c_str();
      connectToFirebase(ssidFinal, passwordFinal);
      
      while(true){
          checkStage(pathToData);
        }
      stage = false;
    }
  }
}

void checkStage(String path){
  readFromFirebase(path);
    if(intValue == 1){
      digitalWrite(RELAY_PIN, HIGH);
      }else{

        digitalWrite(RELAY_PIN, LOW);
        }
}

void readFromFirebase(String path){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    if (Firebase.RTDB.getInt(&fbdo, path)) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        Serial.println(intValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
  
  
}

void connectToFirebase(char* ssid, char* password){
    WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
}
