# 🕷️ Robot Nhện Giám Sát Môi Trường

![Spider Robot](./5b1de07f-a06e-40f5-b08d-454f1de87270.png)

## 📌 Giới thiệu

Đây là một robot nhện sử dụng Arduino với khả năng **di chuyển bằng 4 chân (12 servo)** và tích hợp **nhiều cảm biến môi trường**. Robot được thiết kế để phục vụ các bài toán về **giám sát khí gas, chất lượng không khí, lửa**, đồng thời có khả năng truyền dữ liệu qua **camera ESP32-CAM** và **module SIM**.

Dự án mang tính ứng dụng cao trong các mô hình **STEM**, **robotics**, và **giám sát môi trường thông minh**.

## ⚙️ Phần cứng sử dụng

- ♻️ **12 x Servo SG90** – điều khiển 4 chân nhện (3 servo mỗi chân)
- 🔋 **Nguồn: 2 pin 18650**
- 📷 **ESP32-CAM** – truyền hình ảnh/video trực tiếp
- 📡 **Module SIM800/SIM7600** – gửi dữ liệu qua mạng di động
- 🌫️ **MQ135** – cảm biến chất lượng không khí
- 🔥 **Cảm biến lửa**
- 💨 **MQ2** – cảm biến khí gas

## 🧠 Tính năng

- Điều khiển chuyển động của robot bằng Arduino
- Truyền video và dữ liệu cảm biến qua Wi-Fi/4G
- Cảnh báo khi phát hiện khí gas, cháy, hoặc chất lượng không khí kém
- Tự động di chuyển hoặc dừng khi gặp nguy hiểm
- Phù hợp với giáo dục STEM và mô hình nghiên cứu môi trường

