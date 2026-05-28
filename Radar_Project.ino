/**
 * Embedded Radar System - Arduino Firmware
 * * Hardware Platform: Arduino Uno + Sensor Shield V5.0
 * Peripherals: 1.8" TFT ST7735 LCD, HC-SR04 Ultrasonic Sensor, SG90 Servo Motor
 * * Description: 
 * Controllably sweeps the environment 0-180 degrees via PWM Servo tracking.
 * Calculates proximity distances through Time-of-Flight (ToF) acoustic echo.
 * Performs real-time local drawing on TFT screen and stream packets via Serial.
 */

#include <Adafruit_GFX.h>    // Core graphics library for LCD displays
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735 controller
#include <Servo.h>           // Standard PWM actuator control library
#include <SPI.h>             // Serial Peripheral Interface communication library

// =====================================================
// PIN CONFIGURATION & HARDWARE MAPPING (SPI Interface)
// =====================================================
#define TFT_CS    10  // Chip Select Line
#define TFT_RST   7   // Hardware Reset Line
#define TFT_DC    8   // Data/Command Selection Line
#define TFT_MOSI  5   // SPI Master Output Slave Input (Data Line)
#define TFT_SCLK  13  // SPI Serial Clock Line

// Instantiating the localized TFT Screen object using explicit hardware pins
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Visual interface reference layout coordinates
const int centerX = 80;
const int centerY = 112; 
const int radarRadius = 105;

// =====================================================
// SENSOR & ACTUATION PIN MAPPING (Sensor Shield Channels)
// =====================================================
#define TRIG_PIN 12  // Output channel: Sends acoustic trigger bursts
#define ECHO_PIN 11  // Input channel: Measures acoustic reflections pulse duration

Servo myServo;       // Declaring the PWM servo control instance
#define SERVO_PIN 9  // Output channel mapped to Servo Signal port

int lastAngle = 0;   // Stores preceding angle to optimize dynamic pixel cleaning

// Maximum operating distance envelope threshold capped at 50 cm
const int maxDistance = 50; 

void setup() {
  // Initialize asynchronous Serial Data pipeline at 9600 Baud for remote host communication
  Serial.begin(9600); 

  // Direct Pin Mode assignments for standard acoustic IO transceiver configuration
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize Servo motor with explicitly calibrated pulse-width bounds (500us to 2400us)
  myServo.attach(SERVO_PIN, 500, 2400);
  
  // Force initialization posture: Move the radar turret to starting baseline (0 Degrees)
  myServo.write(0); 
  delay(1500); // Guard delay to safely complete the physical mechanical migration

  // Initialize localized TFT controller registers (Black Tab configuration)
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1); // Set orientation to landscape layout
  tft.fillScreen(ST7735_BLACK); // Flush screen with dark background to eliminate noise

  // Render the static structural radar vector background template
  drawRadarBackground();
}

void loop() {
  // Primary Forward Sweep: Incrementally move the turret from Right (0) to Left (180)
  for (int angle = 0; angle <= 180; angle += 2) {
    radarSweep(angle);
  }
  // Primary Backward Sweep: Smooth mechanical return from Left (180) to Right (0)
  for (int angle = 180; angle >= 0; angle -= 2) {
    radarSweep(angle);
  }
}

/**
 * Executes a single localized telemetry sweep slice at a specific degree.
 */
void radarSweep(int angle) {
  myServo.write(angle); // Update the physical motor alignment position
  int distance = getDistance(); // Trigger sensor routine to measure distance
  
  // Format and dispatch data packets to Processing IDE host via standard comma-separation: (Angle,Distance)
  Serial.print(angle);
  Serial.print(",");
  Serial.println(distance);
  
  // Refresh the local 1.8" TFT visual GUI layer
  updateRadar(angle, lastAngle, distance);
  lastAngle = angle; // Cache the current angle for the next step's pixel clearing process
  delay(25); // Deterministic delay to stabilize motor motion and prevent physical jitter
}

/**
 * Calculates obstacle distance using Time-of-Flight (ToF) echo acoustics.
 */
int getDistance() {
  // Generate a clean low-state baseline to prevent signal distortion
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Send out a precise high-intensity 10-microsecond acoustic trigger burst
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the active high-pulse return duration on Echo pin with a 20ms safety timeout
  long duration = pulseIn(ECHO_PIN, HIGH, 20000); 
  if (duration == 0) return maxDistance; // Return default maximum distance if no reflection is detected
  
  // Mathematical conversion formula: Distance = (Time * Speed of Sound 0.034 cm/us) / 2
  int dist = duration * 0.034 / 2;
  // Apply software filtering logic to eliminate out-of-bounds sensor anomalies
  if (dist <= 2 || dist > maxDistance) dist = maxDistance;
  return dist;
}

/**
 * Utility Function: Computes and draws dashed circular scale markers on the local screen.
 */
void drawDashedArc(int cx, int cy, int r, uint16_t color) {
  for (int a = 0; a <= 180; a += 4) { 
    float rad = radians(a); // Convert polar angles to radians for standard trigonometry
    int x = cx + cos(rad) * r;
    int y = cy - sin(rad) * r;
    tft.drawPixel(x, y, color); // Plot localized pixels along the calculated arc trajectory
  }
}

/**
 * Renders the permanent visual infrastructure and static telemetry markings.
 */
void drawRadarBackground() {
  // 1. Plotting the outer perimeter degree tracking tick marks
  for (int a = 0; a <= 180; a += 2) {
    float rad = radians(a);
    int tickLength = 4; 
    
    // Create structural visual weights for standard intervals
    if (a % 10 == 0) tickLength = 7;   // Medium tick marks
    if (a % 30 == 0) tickLength = 11;  // Large major tick marks

    int x1 = centerX + cos(rad) * radarRadius;
    int y1 = centerY - sin(rad) * radarRadius;
    int x2 = centerX + cos(rad) * (radarRadius - tickLength);
    int y2 = centerY - sin(rad) * (radarRadius - tickLength);
    
    tft.drawLine(x1, y1, x2, y2, ST7735_GREEN);
  }

  // 2. Rendering the localized interior scale lines
  drawDashedArc(centerX, centerY, 42, ST7735_GREEN);  
  drawDashedArc(centerX, centerY, 84, ST7735_GREEN);  

  // 3. Draw the bottom tracking horizon boundary line
  tft.drawFastHLine(0, centerY, 160, ST7735_GREEN);

  // 4. Output localized distance text markers (20 cm and 40 cm range steps)
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  
  tft.setCursor(centerX - 6, centerY - 92);
  tft.print("40");
  
  tft.setCursor(centerX - 6, centerY - 50);
  tft.print("20");

  // 5. Aesthetic Decorative Structural Overlays
  tft.drawRect(5, 5, 12, 15, ST7735_GREEN);
  tft.drawFastHLine(5, 9, 8, ST7735_GREEN);
  tft.drawFastHLine(5, 13, 10, ST7735_GREEN);

  tft.drawRect(140, 5, 15, 15, ST7735_GREEN);
  tft.drawFastHLine(140, 12, 15, ST7735_GREEN);
  tft.drawFastVLine(147, 5, 15, ST7735_GREEN);

  tft.fillTriangle(65, centerY+10, 70, centerY+2, 90, centerY+2, ST7735_GREEN);
  tft.fillTriangle(65, centerY+10, 90, centerY+2, 95, centerY+10, ST7735_GREEN);
  tft.fillRect(70, centerY+2, 20, 8, ST7735_GREEN);
}

/**
 * Dynamic GUI Engine: Updates sweeping lines, angular data, and tracks real-time threat objects.
 */
void updateRadar(int currentAngle, int prevAngle, int distance) {
  
  // [1] Efficient UI Update: Clear only the preceding sweep vector line by painting it black
  float oldRad = radians(prevAngle);
  int oldX = centerX + cos(oldRad) * (radarRadius - 13);
  int oldY = centerY - sin(oldRad) * (radarRadius - 13);
  tft.drawLine(centerX, centerY, oldX, oldY, ST7735_BLACK);

  // [2] Structural UI Reconstruction: Repaint the baseline arcs broken by the black sweep clear routine
  drawDashedArc(centerX, centerY, 42, ST7735_GREEN);
  drawDashedArc(centerX, centerY, 84, ST7735_GREEN);
  tft.drawFastHLine(0, centerY, 160, ST7735_GREEN);

  // [3] Plot the active vector cursor line (Fresh green sweep position)
  float rad = radians(currentAngle);
  int x = centerX + cos(rad) * (radarRadius - 13);
  int y = centerY - sin(rad) * (radarRadius - 13);
  tft.drawLine(centerX, centerY, x, y, ST7735_GREEN);
  
  int xEndOuter = centerX + cos(rad) * (radarRadius - 9);
  int yEndOuter = centerY - sin(rad) * (radarRadius - 9);
  tft.drawLine(x, y, xEndOuter, yEndOuter, ST7735_GREEN);

  // [4] Target Acquisition Module: Red alerts displayed locally when an object breaks the 50cm boundary
  if (distance < maxDistance) {
    // Map spatial distance value proportionally onto the pixel constraints of the TFT panel layout
    int targetRadius = map(distance, 0, maxDistance, 10, radarRadius - 15);
    int targetX = centerX + cos(rad) * targetRadius;
    int targetY = centerY - sin(rad) * targetRadius;

    // Draw active danger indicator nodes at target coordinates
    tft.fillCircle(targetX, targetY, 3, ST7735_RED);
    tft.drawCircle(targetX, targetY, 6, ST7735_RED);

    // Refresh and update localized digital distance measurements
    tft.fillRect(100, centerY + 4, 55, 10, ST7735_BLACK); 
    tft.setCursor(100, centerY + 4);
    tft.setTextColor(ST7735_RED);
    tft.print("DIST: ");
    tft.print(distance);
    tft.print("cm");
  } else {
    // Flush the digital readout zone if the specific vector channel path is clear of obstacles
    tft.fillRect(100, centerY + 4, 55, 10, ST7735_BLACK);
  }

  // [5] Static text dashboard refresh: Draw current system sweeping angle readout at the bottom left
  tft.fillRect(5, centerY + 4, 60, 10, ST7735_BLACK); 
  tft.setCursor(5, centerY + 4);
  tft.setTextColor(ST7735_GREEN);
  tft.print("ANG: ");
  tft.print(currentAngle);
  tft.print((char)247); // Output ASCII code 247 representing the standard Degree symbol (°)
}