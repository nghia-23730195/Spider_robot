#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define simSerial Serial2
#define MCU_SIM_BAUDRATE 115200
#define MCU_SIM_TX_PIN 17
#define MCU_SIM_RX_PIN 16
#define MCU_SIM_EN_PIN 15

// Định nghĩa chân cảm biến
#define FLAME_PIN_1 5
#define FLAME_PIN_2 18
#define MQ2_PIN 32
#define MQ135_PIN 33

// Ngưỡng cho từng chất khí (có thể điều chỉnh tùy theo dữ liệu thực tế)
#define NH3_THRESHOLD 30      // Amoniac (NH₃)
#define NOX_THRESHOLD 60      // Nitơ oxit (NOx)
#define ALCOHOL_THRESHOLD 20  // Cồn (Alcohol)
#define BENZENE_THRESHOLD 40  // Benzen (Benzene)
#define SMOKE_THRESHOLD 50    // Khói
#define CO2_THRESHOLD 150     // Carbon dioxide (CO₂)

// Ngưỡng cảnh báo
#define MQ2_THRESHOLD 1500
#define FLAME_THRESHOLD LOW
#define PHONE_NUMBER "0367505875"

const char* ssid = "SPIDER - ROBOT";
const char* password = "123456789";
WebServer server(80);

void sim_at_wait() {
  delay(100);
  while (simSerial.available()) {
    Serial.write(simSerial.read());
  }
}

bool sim_at_cmd(String cmd) {
  simSerial.println(cmd);
  sim_at_wait();
  return true;
}

void sent_sms(String message) {
  sim_at_cmd("AT+CMGF=1");  // Chế độ văn bản
  sim_at_cmd(String("AT+CMGS=\"") + PHONE_NUMBER + "\"");
  sim_at_cmd(message);  // Nội dung tin nhắn
  // Kết thúc tin nhắn
  simSerial.write(0x1A);
}

void call() {
  String temp = "ATD";
  temp += PHONE_NUMBER;
  temp += ";";
  sim_at_cmd(temp);  // Gọi đi

  delay(20000);  // Đợi 20 giây

  sim_at_cmd("ATH");  // Cúp máy
}

// Khai báo hàm ngoài handleRoot()
String getWarning(float value, float threshold, String name) {
  if (value > threshold) {
    return "⚠️ Nguy hiểm! " + name + " vượt ngưỡng";
  } else {
    return "✅ Bình thường: " + name + " ở mức an toàn";
  }
}

void handleRoot() {
  int flame1 = digitalRead(FLAME_PIN_1);
  int flame2 = digitalRead(FLAME_PIN_2);
  int MQ2_value = analogRead(MQ2_PIN);
  int MQ135_value = analogRead(MQ135_PIN);

  float nh3 = map(MQ135_value, 0, 4095, 0, 100);
  float nox = map(MQ135_value, 0, 4095, 0, 150);
  float alcohol = map(MQ135_value, 0, 4095, 0, 50);
  float benzene = map(MQ135_value, 0, 4095, 0, 75);
  float smoke = (MQ135_value > 300) ? 100 : 0;
  float co2 = map(MQ135_value, 0, 4095, 0, 200);

  String ip = WiFi.localIP().toString();  // Lấy địa chỉ IP của ESP32
  String html = "<html><head><meta http-equiv='refresh' content='2'><meta charset='UTF-8'><style>"
                "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }"
                ".container { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; max-width: 800px; margin: auto; }"
                ".grid-item { padding: 15px; font-size: 18px; border-radius: 10px; color: white; text-align: center; }"
                ".alert-fire { background-color: red; }"
                ".normal-fire { background-color: green; }"
                ".alert-gas { background-color: orange; }"
                ".normal { background-color: blue; }"
                "#camera { grid-column: span 3; margin-top: 20px; }"
                "</style></head><body><h2>🔍 HỆ THỐNG GIÁM SÁT MÔI TRƯỜNG</h2>";

  html += "<div class='container'>";

  // Hiển thị thông tin trường
  html += "<div class='grid-item' style='grid-column: span 3; background-color: #4CAF50; color: white;'>";
  html += "<h3>Trường Trung Học Cơ Sở Nguyễn Du</h3>";
  html += "</div>";

  // // Add camera stream using dynamic IP
  // html += "<div id='camera' class='grid-item' style='text-align:center;'>";
  // html += "<h3>📷 Camera</h3>";
  // html += "<img src='http://192.168.4.2/stream' width='100%' />";
  // html += "</div>";

  // Cảm biến lửa 1
  html.concat("<div class='grid-item ");
  html.concat(flame1 == FLAME_THRESHOLD ? "alert-fire" : "normal-fire");
  html.concat("'>");
  html.concat("🔥 Cảm biến lửa 1<br><span>" + String(flame1) + "</span><br>");
  html.concat(flame1 == FLAME_THRESHOLD ? "🚨 Cảnh báo! Có lửa!" : "✅ Không có lửa");
  html.concat("</div>");

  // Cảm biến lửa 2
  html.concat("<div class='grid-item ");
  html.concat(flame2 == FLAME_THRESHOLD ? "alert-fire" : "normal-fire");
  html.concat("'>");
  html.concat("🔥 Cảm biến lửa 2<br><span>" + String(flame2) + "</span><br>");
  html.concat(flame2 == FLAME_THRESHOLD ? "🚨 Cảnh báo! Có lửa!" : "✅ Không có lửa");
  html.concat("</div>");

  // Cảm biến khí gas
  html.concat("<div class='grid-item ");
  html.concat(MQ2_value > 1000 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến MQ2 (Khí Gas)<br><span>" + String(MQ2_value) + "</span><br>");
  html.concat(MQ2_value > 1000 ? "⚠️ Cảnh báo! Có khí gas!" : "✅ Không có khí gas");
  html.concat("</div>");

  // Cảm biến NH3
  html.concat("<div class='grid-item ");
  html.concat(nh3 > 60 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến NH3<br><span>" + String(nh3) + "</span><br>");
  html.concat(nh3 > 60 ? "⚠️ Cảnh báo! Có NH3!" : "✅ Không có NH3");
  html.concat("</div>");

  // Cảm biến NOx
  html.concat("<div class='grid-item ");
  html.concat(nox > 100 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến NOx<br><span>" + String(nox) + "</span><br>");
  html.concat(nox > 100 ? "⚠️ Cảnh báo! Có NOx!" : "✅ Không có NOx");
  html.concat("</div>");

  // Cảm biến CO2
  html.concat("<div class='grid-item ");
  html.concat(co2 > 150 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến CO2<br><span>" + String(co2) + "</span><br>");
  html.concat(co2 > 150 ? "⚠️ Cảnh báo! Có CO2!" : "✅ Không có CO2");
  html.concat("</div>");

  // Cảm biến Benzene
  html.concat("<div class='grid-item ");
  html.concat(benzene > 50 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến Benzene<br><span>" + String(benzene) + "</span><br>");
  html.concat(benzene > 50 ? "⚠️ Cảnh báo! Có Benzene!" : "✅ Không có Benzene");
  html.concat("</div>");

  // Cảm biến Alcohol
  html.concat("<div class='grid-item ");
  html.concat(alcohol > 30 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Cảm biến Alcohol<br><span>" + String(alcohol) + "</span><br>");
  html.concat(alcohol > 30 ? "⚠️ Cảnh báo! Có Alcohol!" : "✅ Không có Alcohol");
  html.concat("</div>");

  // Cảm biến Khói
  html.concat("<div class='grid-item ");
  html.concat(smoke > 300 ? "alert-gas" : "normal");
  html.concat("'>");
  html.concat("💨 Khói<br><span>" + String(smoke) + "</span><br>");
  html.concat(smoke > 300 ? "⚠️ Có khói!" : "✅ Không có khói");
  html.concat("</div>");

  html.concat("</div></body></html>");

  server.send(200, "text/html", html);
}

void setup() {
  /*  Bật nguồn mô-đun SIM  */
  pinMode(MCU_SIM_EN_PIN, OUTPUT);
  digitalWrite(MCU_SIM_EN_PIN, LOW);
  delay(20);
  Serial.begin(115200);
  Serial.println("\n\n\n\n-----------------------\nHệ thống bắt đầu!!!!");

  // Đợi 8 giây để mô-đun SIM khởi động
  delay(8000);
  simSerial.begin(MCU_SIM_BAUDRATE, SERIAL_8N1, MCU_SIM_RX_PIN, MCU_SIM_TX_PIN);

  // Kiểm tra lệnh AT
  sim_at_cmd("AT");
  sim_at_cmd("ATI");
  sim_at_cmd("AT+CPIN?");
  sim_at_cmd("AT+CSQ");
  sim_at_cmd("AT+CIMI");

  pinMode(FLAME_PIN_1, INPUT);
  pinMode(FLAME_PIN_2, INPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
  delay(5000);

  // Thực hiện phát WIFI
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  int flame_detected_1 = digitalRead(FLAME_PIN_1);
  int flame_detected_2 = digitalRead(FLAME_PIN_2);
  Serial.print("Trạng thái cảm biến lửa: ");
  Serial.println(flame_detected_1);
  delay(1000);
  if (flame_detected_1 == FLAME_THRESHOLD || flame_detected_2 == FLAME_THRESHOLD) {
    Serial.println("CẢNH BÁO! PHÁT HIỆN LỬA!!!");
    Serial.println("---------------------------");
    sent_sms("CANH BAO! PHAT HIEN LUA!!!");
    delay(5000);
    call();  // Thực hiện cuộc gọi
  }

  // Kiểm tra phát hiện khí gas từ cảm biến MQ2
  int MQ2_value = analogRead(MQ2_PIN);
  Serial.print("Giá trị MQ2: ");
  Serial.println(MQ2_value);
  delay(1000);
  if (MQ2_value > MQ2_THRESHOLD) {
    Serial.println("CẢNH BÁO! RÒ RỈ KHÍ GAS!!!");
    Serial.println("---------------------------");
    sent_sms("CANH BAO! RO RI KHI GAS!!!");
    delay(5000);
    call();
  }

  // Đọc và hiển thị thông số cảm biến MP135
  int MQ135_value = analogRead(MQ135_PIN);
  float nh3 = map(MQ135_value, 0, 1023, 0, 100);
  float nox = map(MQ135_value, 0, 1023, 0, 150);
  float alcohol = map(MQ135_value, 0, 1023, 0, 50);
  float benzene = map(MQ135_value, 0, 1023, 0, 75);
  float smoke = (MQ135_value > 600) ? 100 : 0;
  float co2 = map(MQ135_value, 0, 1023, 0, 200);

  Serial.print("MQ135 (Air Quality) Value: ");
  Serial.println(MQ135_value);
  Serial.print("NH₃ (Amoniac): ");
  Serial.println(nh3);
  Serial.print("NOx (Nitrogen Oxides): ");
  Serial.println(nox);
  Serial.print("Ancol (Alcohols): ");
  Serial.println(alcohol);
  Serial.print("Benzen (Benzene): ");
  Serial.println(benzene);
  Serial.print("Khói: ");
  if (smoke > 600) {
    Serial.println("Cảnh báo khói");
  } else {
    Serial.println("Không có khói");
  }
  Serial.print("CO₂ (Carbon Dioxide): ");
  Serial.println(co2);
  Serial.println("---------------------------");
  delay(2000);
}
