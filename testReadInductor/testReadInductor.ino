#include <SoftwareSerial.h>

SoftwareSerial Inductor(11,10);

void setup() {
  Serial.begin(9600);
  Inductor.begin(9600);
  Inductor.write("08271000");
  
  //Inductor.write("07210000");// stop stream
  while (!Serial);
  Inductor.write(06210000);// start stream
}

void loop() {
  if (Inductor.available()) {
    Serial.write(Inductor.read());
  }
}
