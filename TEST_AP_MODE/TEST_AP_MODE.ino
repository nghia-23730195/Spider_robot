#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

#define MP135_PIN 32  // Chân Analog của MP-135

const char* ssid = "ESP32_AP";
const char* password = "12345678";

WebServer server(80);

// Đọc dữ liệu từ cảm biến MP-135
void readMP135(float &nh3, float &nox, float &alcohol, float &benzene, float &smoke, float &co2) {
    int MP135_value = analogRead(MP135_PIN);
    nh3 = map(MP135_value, 0, 4095, 0, 100);
    nox = map(MP135_value, 0, 4095, 0, 150);
    alcohol = map(MP135_value, 0, 4095, 0, 50);
    benzene = map(MP135_value, 0, 4095, 0, 75);
    smoke = (MP135_value > 100) ? 100 : 0;
    co2 = map(MP135_value, 0, 4095, 0, 200);
}

// Xử lý trang web
void handleRoot() {
    float nh3, nox, alcohol, benzene, smoke, co2;
    readMP135(nh3, nox, alcohol, benzene, smoke, co2);
    
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Chất Lượng Không Khí</title>";
    html += "<style> body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }";
    html += " .container { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; padding: 20px; max-width: 900px; margin: auto; }";
    html += " .box { padding: 30px; font-size: 24px; font-weight: bold; text-align: center; border-radius: 10px; width: 100%; height: 120px; display: flex; align-items: center; justify-content: center; }";
    html += " .good { background-color: #4CAF50; color: white; } "
            " .moderate { background-color: #FFC107; color: black; } "
            " .danger { background-color: #FF5722; color: white; } ";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Chất Lượng Không Khí</h1>";
    html += "<div class='container'>";
    html += "<div class='box good'><strong>NH₃:</strong> " + String(nh3) + " ppm</div>";
    html += "<div class='box moderate'><strong>NOx:</strong> " + String(nox) + " ppm</div>";
    html += "<div class='box good'><strong>Alcohol:</strong> " + String(alcohol) + " ppm</div>";
    html += "<div class='box moderate'><strong>Benzene:</strong> " + String(benzene) + " ppm</div>";
    html += "<div class='box danger'><strong>CO₂:</strong> " + String(co2) + " ppm</div>";
    html += "<div class='box danger'><strong>Khói:</strong> " + String((smoke > 0) ? "Cảnh báo khói" : "Không có khói") + "</div>";
    html += "</div></body></html>";
    
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("WiFi AP đã tạo!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    
    pinMode(MP135_PIN, INPUT);
    server.on("/", handleRoot);
    server.begin();
}

void loop() {
    server.handleClient();
}
