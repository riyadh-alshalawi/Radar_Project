import processing.serial.*;

Serial myPort;        
String serialData = "";
int angle = 0;
int distance = 0;
int maxDistance = 50; // متوافق مع كود الأردوينو

void setup() {
  size(800, 500); // حجم الشاشة المفتوحة بالكمبيوتر
  smooth();
  
  // تنبيه: غير "COM3" إلى المنفذ الصحيح الذي يظهر لك في الأردوينو
  myPort = new Serial(this, "COM7", 9600); 
  myPort.bufferUntil('\n'); // الانتظار حتى اكتمال السطر قبل قراءة البيانات
}

void draw() {
  // تأثير التلاشي (Fade effect) لخط الرادار ليعطي حركة سينمائية
  fill(0, 15);
  noStroke();
  rect(0, 0, width, height);
  
  pushMatrix();
  translate(width/2, height - 50); // تحويل نقطة الصفر لمنتصف أسفل الشاشة
  
  drawRadarGrid(); // رسم الرادار الخلفي
  drawRadarLine(); // رسم مؤشر المسح الأخضر
  drawTarget();    // رسم الأهداف المكتشفة باللون الأحمر
  
  popMatrix();
  drawText();      // طباعة معلومات الزاوية والمسافة أسفل الشاشة
}

// دالة قراءة واستقبال البيانات وتفكيكها
void serialEvent(Serial myPort) {
  try {
    serialData = myPort.readStringUntil('\n');
    if (serialData != null) {
      serialData = trim(serialData);
      int[] list = int(split(serialData, ','));
      if (list.length >= 2) {
        angle = list[0];
        distance = list[1];
      }
    }
  } catch(Exception e) {
    // لحماية البرنامج من التوقف المفاجئ في حال حدوث تشويش بالبيانات
  }
}

// رسم خطوط وشبكة الرادار
void drawRadarGrid() {
  pushStyle();
  noFill();
  strokeWeight(1);
  stroke(0, 200, 0);
  
  // رسم الأقواس الأربعة للمسافات
  for (int r = 100; r <= 400; r += 100) {
    arc(0, 0, r*2, r*2, PI, TWO_PI);
  }
  
  // رسم الخطوط الشعاعية للزوايا الأساسية
  for (int a = 30; a <= 150; a += 30) {
    float rad = radians(a);
    line(0, 0, -cos(rad)*400, -sin(rad)*400);
  }
  popStyle();
}

// رسم مؤشر خط الرادار المتحرك
void drawRadarLine() {
  pushStyle();
  strokeWeight(3);
  stroke(0, 255, 0); 
  float rad = radians(angle);
  line(0, 0, cos(rad)*400, -sin(rad)*400); 
  popStyle();
}

// رسم الأجسام المكتشفة باللون الأحمر عند اقترابها
void drawTarget() {
  pushStyle();
  if (distance < maxDistance) {
    // تحويل المسافة من سم إلى بكسل ليتناسب مع أبعاد الشاشة (أقصى مدى 400 بكسل)
    float mappedDist = map(distance, 0, maxDistance, 0, 400);
    float rad = radians(angle);
    
    float x = cos(rad) * mappedDist;
    float y = -sin(rad) * mappedDist;
    
    fill(255, 0, 0);
    noStroke();
    ellipse(x, y, 12, 12); // نقطة الهدف الأساسية
    
    stroke(255, 0, 0);
    noFill();
    ellipse(x, y, 24, 24); // حلقة الرصد المحيطة بالهدف
  }
  popStyle();
}

// طباعة النصوص الإرشادية والبيانات المباشرة
void drawText() {
  pushStyle();
  fill(0);
  noStroke();
  rect(0, height-40, width, 40); // خلفية سوداء للشريط السفلي
  
  fill(0, 255, 0);
  textSize(18);
  text("Angle: " + angle + "°", 50, height - 15);
  
  if(distance < maxDistance) {
    fill(255, 0, 0);
    text("Object Detected! Distance: " + distance + " cm", width/2 - 120, height - 15);
  } else {
    text("Scanning... No Object", width/2 - 80, height - 15);
  }
  
  // طباعة أرقام تدريج المسافات على الأقواس
  fill(0, 200, 0);
  textSize(12);
  text("12.5 cm", width/2 + 105, height - 55);
  text("25.0 cm", width/2 + 205, height - 55);
  text("37.5 cm", width/2 + 305, height - 55);
  text("50.0 cm", width/2 + 390, height - 55);
  popStyle();
}
