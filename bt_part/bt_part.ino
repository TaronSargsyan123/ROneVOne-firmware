#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
bool stage = true;
void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
}

void loop() {
  if(stage){
    if (SerialBT.available()) {
      String message = SerialBT.readString();
      Serial.println(message);
      delay(1000);
      stage = false;
   }
  }
}
