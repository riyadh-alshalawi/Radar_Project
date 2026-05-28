/**
 * Remote Visualization Radar Terminal - Processing IDE Sketch
 * * Host Protocol: Asynchronous Serial Communication via COM3 Port
 * GUI Architecture: High-fidelity vector mapping terminal with persistence decay.
 * * Description: 
 * Asynchronously captures real-time data packets from the host microcontroller.
 * Parses character strings into meaningful Angle and Proximity variables.
 * Renders full screen tracking indicators and dynamic multi-layered radar metrics.
 */

import processing.serial.*; // Import Processing standard serial interface library

Serial myPort;        // Define communication instance mapping to the microcontroller host
String serialData = ""; // Local string container to hold raw incoming buffer characters
int angle = 0;        // Extracted directional angle orientation variable
int distance = 0;     // Extracted range proximity telemetry data variable
int maxDistance = 50; // Calibrated maximum range boundary to match embedded controller specs

void setup() {
  size(800, 500); // Instantiate graphics viewport frame dimensions (800x500 pixels)
  smooth();       // Activate anti-aliasing interpolation filters to optimize line rendering
  
  // Initialize Serial port connection at standard 9600 baud rate matching the Arduino
  // NOTE: Verify port string assignment "COM3" corresponds to the active system port
  myPort = new Serial(this, "COM3", 9600); 
  myPort.bufferUntil('\n'); // Program serial driver interrupts to trigger exclusively upon line feed completions
}

void draw() {
  // Persistence Tracking Module (Fade Effect): Draws a semi-transparent black overlay
  // This leaves a glowing trail effect behind the moving sweeping vector line
  fill(0, 15);
  noStroke();
  rect(0, 0, width, height);
  
  pushMatrix(); // Isolate coordinate transform states to prevent global matrix pollution
  translate(width/2, height - 50); // Remap operational coordinate origin to bottom center of the interface
  
  drawRadarGrid(); // Draw background circular range scales and angular line references
  drawRadarLine(); // Draw the primary high-intensity sweeping vector cursor line
  drawTarget();    // Render identified threat positions with active hazard alerts
  
  popMatrix();  // Restore pristine baseline coordinate tracking configurations
  drawText();      // Output real-time textual dashboards and tracking alerts on the display layout
}

/**
 * Serial Asynchronous Communication Event Handler: Automatically executes via serial hardware interrupt
 */
void serialEvent(Serial myPort) {
  try {
    serialData = myPort.readStringUntil('\n'); // Extract buffered string stream up to the newline delimiter
    if (serialData != null) {
      serialData = trim(serialData); // Strip out blank trailing whitespaces or carriage return bits
      int[] list = int(split(serialData, ',')); // Deconstruct the comma-separated data bundle into index slots
      if (list.length >= 2) {
        angle = list[0];    // Assign index 0 directly to system swept position angle
        distance = list[1]; // Assign index 1 directly to target obstacle range measurement
      }
    }
  } catch(Exception e) {
    // Robust Error Catch Exception block: Prevents thread execution failure in case of transmission noise
  }
}

/**
 * Technical Vector Map Graphics Engine: Renders standard radar geometric layout grids
 */
void drawRadarGrid() {
  pushStyle(); // Safeguard configuration parameters to isolate visual stroke modifications
  noFill();
  strokeWeight(1);
  stroke(0, 200, 0); // Apply standard radar dark-green styling tone
  
  // Generate four equidistant geometric tracking arc divisions
  for (int r = 100; r <= 400; r += 100) {
    arc(0, 0, r*2, r*2, PI, TWO_PI); // Draw standard top-half tracking hemisphere arcs
  }
  
  // Render directional angular divider axes at standard 30-degree tracking intervals
  for (int a = 30; a <= 150; a += 30) {
    float rad = radians(a);
    line(0, 0, -cos(rad)*400, -sin(rad)*400); // Standard mathematical polar-to-cartesian projection
  }
  popStyle(); // Restore global visual styles back to core sketch configurations
}

/**
 * Sweeper Vector UI Layer: Renders the active scanning pointer cursor line
 */
void drawRadarLine() {
  pushStyle();
  strokeWeight(3);
  stroke(0, 255, 0); // Bright high-visibility green for active tracking vector
  float rad = radians(angle);
  line(0, 0, cos(rad)*400, -sin(rad)*400); // Direct line plotting toward current heading position
  popStyle();
}

/**
 * Threat Target Visualizer: Maps and tracks active obstacles within operational constraints
 */
void drawTarget() {
  pushStyle();
  if (distance < maxDistance) {
    // Scale spatial metric distance to viewport display resolution constraints (0 to 400 pixels)
    float mappedDist = map(distance, 0, maxDistance, 0, 400);
    float rad = radians(angle);
    
    // Compute targeted Cartesian pixel positions relative to adjusted screen origin
    float x = cos(rad) * mappedDist;
    float y = -sin(rad) * mappedDist;
    
    // Draw high-alert threat indicators at target coordinate node
    fill(255, 0, 0); // Danger alert: Solid Red
    noStroke();
    ellipse(x, y, 12, 12); // Primary target intercept node indicator
    
    stroke(255, 0, 0);
    noFill();
    ellipse(x, y, 24, 24); // Perimeter perimeter tracking target ring overlay
  }
  popStyle();
}

/**
 * Telemetry Status Terminal: Renders digital measurements and alert notification banners
 */
void drawText() {
  pushStyle();
  fill(0);
  noStroke();
  rect(0, height-40, width, 40); // Generate deep black layout bar for status tracking texts
  
  fill(0, 255, 0);
  textSize(18);
  text("Angle: " + angle + "°", 50, height - 15); // Print real-time dynamic servo angle position
  
  // Active Danger Monitoring Logic
  if(distance < maxDistance) {
    fill(255, 0, 0); // Render bold danger warning colors upon close obstacle detection
    text("Object Detected! Distance: " + distance + " cm", width/2 - 120, height - 15);
  } else {
    fill(0, 255, 0); // Stable tracking message display mode
    text("Scanning... No Object", width/2 - 80, height - 15);
  }
  
  // Render calculated metric text scales over tracking grid rings
  fill(0, 200, 0);
  textSize(12);
  text("12.5 cm", width/2 + 105, height - 55);
  text("25.0 cm", width/2 + 205, height - 55);
  text("37.5 cm", width/2 + 305, height - 55);
  text("50.0 cm", width/2 + 390, height - 55);
  popStyle();
}
