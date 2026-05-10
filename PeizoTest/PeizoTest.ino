const int piezoPin = A1; // Pin connected to Piezo Red wire

int baseline = 900; // Your average "idle" value
int sensitivity = 40; // How much HIGHER than baseline to trigger

void setup() {
  Serial.begin(9600);
  pinMode(piezoPin, INPUT);
  Serial.println("--- SYNAPSE-X Piezo Sensitivity Test ---");
  Serial.println("1. Open Serial Plotter (Tools > Serial Plotter)");
  Serial.println("2. Squeeze the gripper finger against the thumb.");
}

void loop() {
  int rawValue = analogRead(piezoPin);
  
  // Look for a spike ABOVE the idle range
  if (rawValue > (baseline + sensitivity) || rawValue < (baseline - sensitivity)) {
    // This is a real impact/squeeze
    int impactForce = abs(rawValue - baseline);
    Serial.print("Touch Detected! Force: ");
    Serial.println(impactForce);
    
    // Send Haptic signal to ESP32
    delay(100); // Small debounce
  }
}