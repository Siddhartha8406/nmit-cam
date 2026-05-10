#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial hc12(10, 11); 
Servo baseS, shoulderL, shoulderR, gripS;

const int piezoPin = A1;
int piezoBaseline = 0;
const int sensitivity = 80; // Trigger if value moves 50 units away from baseline

void setup() {
  Serial.begin(9600);
  hc12.begin(9600);
  
  baseS.attach(3); shoulderL.attach(5); shoulderR.attach(6); gripS.attach(9);

  // --- CALIBRATION STEP ---
  Serial.println("--- SYNAPSE-X: CALIBRATING PIEZO ---");
  long sum = 0;
  for(int i=0; i<20; i++) {
    sum += analogRead(piezoPin);
    delay(20);
  }
  piezoBaseline = sum / 20;
  Serial.print("Baseline Set To: "); Serial.println(piezoBaseline);
  Serial.println("--- ROBOT READY ---");
}

void loop() {
  // --- 1. RELATIVE PIEZO SENSE ---
  int p = analogRead(piezoPin);
  
  // Calculate the difference from the baseline
  int diff = abs(p - piezoBaseline);

  if (diff > sensitivity) { 
    hc12.print('H'); // Send Haptic to ESP32
    Serial.print("!!! TOUCH DETECTED | Val: "); Serial.print(p);
    Serial.print(" | Diff: "); Serial.println(diff);
    delay(100); // Debounce to prevent haptic stutter
  }

  // --- 2. RECEIVE MOVEMENT DATA (Robust Parsing) ---
  if (hc12.available()) {
    if (hc12.peek() == '<') {
      hc12.read(); // Discard the '<'
      int b = hc12.parseInt();
      int s = hc12.parseInt();
      int g = hc12.parseInt();
      
      // Update Servos
      if (b >= 0 && b <= 180) baseS.write(b);
      if (s >= 30 && s <= 170) {
        shoulderL.write(map(s, 0, 180, 145, 180));
        shoulderR.write(map(s, 0, 180, 110, 160)); 
      }
      if (g >= 0 && g <= 150) gripS.write(g);

      // Debug Recv
      Serial.print("MOV -> B:"); Serial.print(b);
      Serial.print(" S:"); Serial.print(s);
      Serial.print(" G:"); Serial.println(g);
      Serial.print(" T:"); Serial.println(diff);
    } else {
      hc12.read(); // Clear buffer junk if not starting with '<'
    }
  }
}