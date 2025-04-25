#include <Wire.h>

#define MP135_PIN 26  // Chân Analog của ESP32

void setup() {
  Serial.begin(115200);  // Khởi động Serial Monitor
  
  pinMode(MP135_PIN, INPUT);  // Đặt chân cảm biến là đầu vào
}

void loop() {
  int air_quality = analogRead(MP135_PIN);  // Đọc giá trị từ cảm biến MP135
  
  // Phân loại mức độ ô nhiễm không khí và hiển thị các khí
  float nh3 = map(air_quality, 0, 1023, 0, 100);  // Ước lượng NH3
  float nox = map(air_quality, 0, 1023, 0, 150);  // Ước lượng NOx
  float alcohol = map(air_quality, 0, 1023, 0, 50);  // Ước lượng Ancol
  float benzene = map(air_quality, 0, 1023, 0, 75);  // Ước lượng Benzen
  float smoke = (air_quality > 100) ? 100 : 0;  // Ngưỡng cảnh báo khói (600)
  float co2 = map(air_quality, 0, 1023, 0, 200);  // Ước lượng CO2
  
  // In các giá trị lên Serial Monitor
  Serial.print("MP135 (Air Quality) Value: ");
  Serial.println(air_quality);
  
  Serial.print("NH₃ (Amoniac): ");
  Serial.println(nh3);
  Serial.print("NOx (Nitrogen Oxides): ");
  Serial.println(nox);
  Serial.print("Ancol (Alcohols): ");
  Serial.println(alcohol);
  Serial.print("Benzen (Benzene): ");
  Serial.println(benzene);
  Serial.print("Khói: ");
  if (smoke > 0) {
    Serial.println("Cảnh báo khói");
  } else {
    Serial.println("Không có khói");
  }
  Serial.print("CO₂ (Carbon Dioxide): ");
  Serial.println(co2);
  
  delay(2000);  // Đọc giá trị mỗi 2 giây một lần
}
