// Pin connected to ULN2003 Input (IN1)
const int motorPin = 25; 

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  
  Serial.println("--- Haptic Motor Test Starting ---");
  Serial.println("The motor should pulse every 2 seconds.");
}

void loop() {
  Serial.println("Motor ON");
  digitalWrite(motorPin, HIGH); // Turn motor on
  delay(500);                  // Wait 0.5 seconds
  
  Serial.println("Motor OFF");
  digitalWrite(motorPin, LOW);  // Turn motor off
  delay(1500);                 // Wait 1.5 seconds
}