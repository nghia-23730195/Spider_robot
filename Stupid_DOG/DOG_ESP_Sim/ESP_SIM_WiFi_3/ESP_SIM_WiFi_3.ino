//chọn bo "DIOT ESP32 DevKit V1"
#include <WiFi.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define simSerial               Serial2
#define MCU_SIM_BAUDRATE        115200
#define MCU_SIM_TX_PIN          17
#define MCU_SIM_RX_PIN          16
#define MCU_SIM_EN_PIN          15

// Định nghĩa chân pin cảm biến lửa và khí gas
#define FLAME_PIN_1             5
#define FLAME_PIN_2             18
#define MQ2_PIN                 32
#define MP135_PIN               33

// Ngưỡng giá trị cho cảm biến khí gas
#define MQ2_THRESHOLD           1500
#define FLAME_THRESHOLD         LOW // Giả sử khi chân pin cảm biến lửa xuất hiện mức LOW là phát hiện lửa

#define PHONE_NUMBER            "0707690321" // nhập số điện thoại 

const char* ssid = "SPIDER - ROBOT";
const char* password = "12345678";

void sim_at_wait()
{
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

bool sim_at_send(char c){
    simSerial.write(c);
    return true;
}

void sent_sms(String message) {
    sim_at_cmd("AT+CMGF=1"); // Chế độ văn bản
    String temp = "AT+CMGS=\"";
    temp += PHONE_NUMBER;
    temp += "\"";
    sim_at_cmd(temp);
    sim_at_cmd(message); // Nội dung tin nhắn
    
    // Kết thúc tin nhắn
    sim_at_send(0x1A);
}

void call() {
    String temp = "ATD";
    temp += PHONE_NUMBER;
    temp += ";";
    sim_at_cmd(temp); // Gọi đi
    
    delay(20000); // Đợi 20 giây
    
    sim_at_cmd("ATH"); // Cúp máy
}

void setup() 
{
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
    pinMode(MP135_PIN, INPUT);
    delay(5000); 

    // Thực hiện phát WIFI
    WiFi.mode(WIFI_AP);
    if(!WiFi.softAP(ssid, password))  {
        Serial.println("Soft AP creation failed!");
        while(1);
    }
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void loop() 
{ 
    int flame_detected_1 = digitalRead(FLAME_PIN_1);
    int flame_detected_2 = digitalRead(FLAME_PIN_2);
    Serial.print("Trạng thái cảm biến lửa: ");
    Serial.println(flame_detected_1);
    delay (1000);
    if (flame_detected_1 == FLAME_THRESHOLD || flame_detected_2 == FLAME_THRESHOLD) {
        Serial.println("CẢNH BÁO! PHÁT HIỆN LỬA!!!");
        Serial.println("---------------------------");
        sent_sms("CANH BAO! PHAT HIEN LUA!!!");
        delay(5000); 
        call(); // Thực hiện cuộc gọi
    } 

    // Kiểm tra phát hiện khí gas từ cảm biến MQ2
    int MQ2_value = analogRead(MQ2_PIN);
    Serial.print("Giá trị MQ2: ");
    Serial.println(MQ2_value);
    delay (1000);
    if (MQ2_value > MQ2_THRESHOLD) {
        Serial.println("CẢNH BÁO! RÒ RỈ KHÍ GAS!!!");
        Serial.println("---------------------------");
        sent_sms("CANH BAO! RO RI KHI GAS!!!");
        delay(5000); 
        call();
    }
    
    // Đọc và hiển thị thông số cảm biến MP135
    int MP135_value = analogRead(MP135_PIN);
    float nh3 = map(MP135_value, 0, 1023, 0, 100);  
    float nox = map(MP135_value, 0, 1023, 0, 150);  
    float alcohol = map(MP135_value, 0, 1023, 0, 50);  
    float benzene = map(MP135_value, 0, 1023, 0, 75);  
    float smoke = (MP135_value > 100) ? 100 : 0;  
    float co2 = map(MP135_value, 0, 1023, 0, 200);  

    Serial.print("MP135 (Air Quality) Value: ");
    Serial.println(MP135_value);
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
    Serial.println("---------------------------");
    delay(2000);

    // Giao tiếp với mô-đun SIM qua cổng Serial
    if (Serial.available()){
        char c = Serial.read();
        simSerial.write(c);
    }
    sim_at_wait();
    
}