char rx_byte = 0;
void setup() {

  //This is for serial port , which is the one used for the Nextion in this demonstration.
  //You might need to change this depending on which serial port you are using for your display.
  Serial1.begin(9600);
  //For demonstration
  HMI_startup_message();
}
void HMI_startup_message() {
  Serial1.print(F("regen0.txt=\""));
  Serial1.print(F("Nextion"));
  Serial1.print(F("\""));
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  if (Serial.available() > 0) {  // is a character available?
    rx_byte = Serial.read();     // get the character
  }
  Serial.println(": " + String(rx_byte) + ":");
}
void loop() {
}