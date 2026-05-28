# Embedded Radar System — Arduino Firmware & Workstation Visualization

An automated, low-cost **Automated Ultrasonic Radar System** designed for real-time spatial awareness, proximity object detection, and synchronized dual-layer telemetry visualization. The system bridges the gap between low-level hardware telemetry and high-level graphic visual computing by tracking obstacles across an active 180-degree horizontal sweeping turret.

---

## 🛠️ System Architecture & Hardware Components

The hardware platform utilizes an **Arduino Uno** accelerated through an **Arduino Sensor Shield V5.0** to form a rigid, vibration-resistant distribution hub.

* **Microcontroller:** Arduino Uno (ATmega328P processing core)
* **Expansion Hub:** Arduino Sensor Shield V5.0
* **Local Display:** 1.8" TFT ST7735 LCD Color Screen
* **Distance Sensor:** HC-SR04 Ultrasonic Transceiver
* **Actuator:** TowerPro SG90 Micro-Servo Motor
* **Chassis Infrastructure:** Laser-Cut Plywood baseplate, elevated turret platform, and angled display console housing secured via nylon standoffs and metallic machine screws.

---

## 🔌 Hardware Interface & Wiring Topology

All peripheral data lines and power references are routed directly into the **Sensor Shield V5.0** as follows:

### 1. SG90 Servo Motor Connections
* **Signal (Orange/Yellow):** `S` pin under **Pin 9**
* **Power (Red VCC):** `V` pin under **Pin 9**
* **Ground (Brown/Black):** `G` pin under **Pin 9**

### 2. HC-SR04 Ultrasonic Sensor Connections
* **Trig (Trigger Output):** `S` pin under **Pin 12**
* **Echo (Echo Input):** `S` pin under **Pin 11**
* **VCC (5V Power):** `V` pin under **Pin 11**
* **GND (Ground):** `G` pin under **Pin 11**

### 3. 1.8" TFT LCD Display Connections (SPI Bus Layout)
* **CS (Chip Select):** `S` pin under **Pin 10**
* **RESET (Hardware Reset):** `S` pin under **Pin 7**
* **A0 (Data/Command Selection):** `S` pin under **Pin 8**
* **SDA (MOSI Data Line):** `S` pin under **Pin 5**
* **SCK (SPI Serial Clock):** `S` pin under **Pin 13**
* **LED (Backlight Power):** `V` pin under **Pin 4**
* **VCC (Main Power):** `V` pin under **Pin 3**
* **GND (System Ground):** `G` pin under **Pin 3**

---

## 💻 Core Software Algorithms & Logic

The system splits computing constraints efficiently: low-level sensor triggers and local UI routines are managed by the microcontroller, while resource-intensive graphical mapping is offloaded to the host PC workstation.

### 1. Acoustic Distance Sensing (Time-of-Flight)
The `getDistance()` routine triggers the HC-SR04 to emit sound bursts (8 cycles at 40 kHz) and captures the active high-pulse return window using a 20ms safety timeout:
$$\text{Distance (cm)} = \frac{\text{Duration in Microseconds} \times 0.034}{2}$$
A software safety filter discards out-of-bounds environmental anomalies, capping the operating envelope at **50 cm** (values $\le 2\text{cm}$ or $> 50\text{cm}$ fall back to maximum distance).

### 2. Embedded GUI Optimization (Dynamic Pixel Cleaning)
To avoid heavy screen-flush routines (`tft.fillScreen()`) which cause extreme processing latency and visible screen flickering, the algorithm uses **Dynamic Pixel Cleaning**:
* It computes the Cartesian coordinates $(X, Y)$ of the *previous* sweep line using polar-to-rectangular conversions.
* It overwrites just that single line in `ST7735_BLACK` before plotting the new active sweep vector in `ST7735_GREEN`.

### 3. Asynchronous Telemetry Pipeline
Telemetry data packets are compiled as compressed alphanumeric strings structured via standard comma separation: `(Angle,Distance)`. These are pushed out down the USB cable link to the host workstation's assigned serial port at a baud rate of **9600 bps**.

### 4. Remote Processing Workspace Terminal
The PC host script catches the data stream asynchronously using a hardware-triggered `serialEvent()` interrupt. It utilizes an **alpha-blending fade screen overlay** (`fill(0, 15)`) to yield a cinematic visual persistence trail behind the radar sweep, allowing operators to trace historical target movements in real time.

---

## ⚙️ Execution & Operating Principle

1.  **Mechanical Sweeping:** The firmware commands the servo motor to rotate back and forth continuously across a horizontal arc from $0^{\circ}$ to $180^{\circ}$ in $2^{\circ}$ angular steps. A deterministic software delay of **25 ms** stabilizes the motor between shifts to eliminate physical jitter.
2.  **Target Acquisition Module:** * **Local UI:** If an object breaks the 50 cm boundary, vibrant red danger nodes (`ST7735_RED`) are drawn over the local radar arcs accompanied by an active digital readout (e.g., `DIST: 24cm`).
    * **Remote UI:** The host workstation terminal flashes an instant red warning banner reading `Object Detected! Distance: [X] cm` which resets instantly to `Scanning... No Object` once the sweeping path is clear.

---

## 👥 Project Contributors

* **Riyadh Mutlaq Alshalawi** (Academic ID: 44458766) — Section 2554
* **Saud Mu'taq Al-Nafie** (Academic ID: 44150184) — Section 641
* **Academic Supervision:** Dr. Faisal Alamri, Dr. Osama Salah, and Dr. Hisham Al-Humyani.
