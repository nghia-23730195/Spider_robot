#define MQ2_PIN 25  // Chân analog của ESP32

void setup() {
  Serial.begin(115200);  // Khởi động Serial Monitor
  
  pinMode(MQ2_PIN, INPUT);  // Đặt chân cảm biến là đầu vào
}

void loop() {
  int gasValue = analogRead(MQ2_PIN);  // Đọc giá trị từ cảm biến MQ-2
  Serial.print("Gas Value: ");
  Serial.println(gasValue);  // In giá trị ra màn hình Serial Monitor

  // Nếu giá trị vượt ngưỡng, cảnh báo khí gas hoặc khói
  if (gasValue > 1500) {  // Ngưỡng có thể điều chỉnh, bạn có thể thử với giá trị khác
    Serial.println("Cảnh báo: Phát hiện khí gas hoặc khói!");
  } else {
    Serial.println("Không có khí gas hoặc khói.");
  }

  delay(2000);  // Đọc giá trị mỗi 2 giây một lần
}
