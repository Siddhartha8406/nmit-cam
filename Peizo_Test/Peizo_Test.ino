const int piezoPin = 25; // Pin connected to Piezo Red wire

void setup() {
  Serial.begin(9600);
  pinMode(piezoPin, INPUT);
  Serial.println("--- SYNAPSE-X Piezo Sensitivity Test ---");
  Serial.println("1. Open Serial Plotter (Tools > Serial Plotter)");
  Serial.println("2. Squeeze the gripper finger against the thumb.");
}

void loop() {
  int rawValue = analogRead(piezoPin);

  // We only print values above a tiny noise floor to keep the graph clean
  if (rawValue > 5) {
    Serial.println(rawValue);
  }

  delay(10); // Fast sampling to catch the impact spike
}