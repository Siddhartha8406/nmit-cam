#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
HardwareSerial hc12(2); 

const int flexPin = 34; 
const int thumbMotor = 25; 
int16_t ax, ay, az, gx, gy, gz;

// --- RESET CONFIGURATION ---
int startX = 90;  
int startY = 90;  
int offsetX = 0;
int offsetY = 0;

// --- STABILITY SETTINGS ---
const int windowSize = 5;
const int moveThreshold = 4; 

int baseArray[windowSize], shoulderArray[windowSize];
int baseIdx = 0, shoulderIdx = 0;

int lastSentB = 90, lastSentS = 90, lastSentG = 0;

void setup() {
  Serial.begin(115200); 
  delay(1000);
  Serial.println("\n--- SYNAPSE-X GLOVE: DEBUG MODE ACTIVE ---");

  hc12.begin(9600, SERIAL_8N1, 16, 17);  
  pinMode(thumbMotor, OUTPUT);
  digitalWrite(thumbMotor, LOW); 
  
  Wire.begin(21, 22);
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("[OK] MPU6050 Link Established.");
    
    // Startup Buzz
    digitalWrite(thumbMotor, HIGH); delay(150); digitalWrite(thumbMotor, LOW);

    Serial.println("Calibrating... Hold still!");
    delay(1000); 
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    int currentX = map(ax, -14000, 14000, 0, 180);
    int currentY = map(ay, -14000, 14000, 0, 180);
    offsetX = startX - currentX;
    offsetY = startY - currentY;
    
    Serial.print("Baseline X:"); Serial.print(currentX);
    Serial.print(" | Offset X:"); Serial.println(offsetX);
  }
}

int getAverage(int val, int* arr, int& idx) {
  arr[idx] = val;
  idx = (idx + 1) % windowSize;
  long sum = 0;
  for (int i = 0; i < windowSize; i++) sum += arr[i];
  return sum / windowSize;
}

void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  int rawFlex = analogRead(flexPin);
  
  // 1. Calculations
  int calcB = map(ax, -12000, 12000, 0, 180) + offsetX;
  int calcS = map(ay, -12000, 12000, 0, 180) + offsetY;
  int rawG  = constrain(map(rawFlex, 1800, 2500, 150, 0), 0, 150);

  // 2. Smoothing
  int avgB = getAverage(constrain(calcB, 0, 180), baseArray, baseIdx);
  int avgS = getAverage(constrain(calcS, 10, 175), shoulderArray, shoulderIdx);

  // 3. Serial Debug Output (Always Visible)
  Serial.print("B_AVG: "); Serial.print(avgB);
  Serial.print(" | S_AVG: "); Serial.print(avgS);
  Serial.print(" | G_RAW: "); Serial.print(rawG);

  // 4. Send Packet logic
  bool change = false;
  if (abs(avgB - lastSentB) >= moveThreshold) { lastSentB = avgB; change = true; }
  if (abs(avgS - lastSentS) >= moveThreshold) { lastSentS = avgS; change = true; }
  if (abs(rawG - lastSentG) >= 5) { lastSentG = rawG; change = true; }

  if (change) {
    hc12.print("<");
    hc12.print(lastSentB); hc12.print(",");
    hc12.print(lastSentS); hc12.print(",");
    hc12.print(lastSentG);
    hc12.println(">");
    Serial.print(" [TX]"); // Indicate data sent
  }

  // 5. Haptic Feedback
  if (hc12.available()) {
    char feedback = hc12.read();
    if (feedback == 'H') {
      Serial.print(" !!! HAPTIC !!!");
      digitalWrite(thumbMotor, HIGH);
      delay(100); 
      digitalWrite(thumbMotor, LOW);
      while(hc12.available()) hc12.read(); 
    }
  }

  Serial.println(); // Finalize debug line
  delay(40); 
}