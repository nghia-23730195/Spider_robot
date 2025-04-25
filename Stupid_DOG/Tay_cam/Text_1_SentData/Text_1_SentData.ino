/*
  J1: A0, A1
  J2: A2, A3
  Pot1: A7
  Pot2: A6

  B1: D8
  B2: D9
  B3: D2
  B4: D3

  RF ::::
  MOSI : D11 
  MISO : D12 
  CSN  : D6
  CE   : D5
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(5, 6); // CE, CSN
const byte address[6] = "00001";

int j2x, j2y;
int j1x, j1y;
byte dataSend[2]; //mảng gửi dữ liệu đi
//0:stand 1:forward 2:backward 3:turn-right 4:turn-left
int status_2 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Cấu hình nRF24L01, gửi
  if (!radio.begin()) 
  {
    Serial.println("ĐANG KHỞI ĐỘNG!");
    while (1) {}
  }   
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN); 
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);  
  radio.stopListening(); //Cài đặt module là TX
  if (!radio.available())
  {
    Serial.println("ĐANG KẾT NỐI");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  read_J2();
  dataSend[0] = status_2;
  dataSend[1] = analogRead(A1);

  radio.write(&dataSend, sizeof(dataSend));
  Serial.println(dataSend[0]);
  delay(100);
}

void read_J2()  {
  j2x = analogRead(A2);
  j2y = analogRead(A3);
  Serial.print("j2x = ");
  Serial.print(j2x);
  Serial.print("\tj2y = ");
  Serial.println(j2y);
  if(j2x<100) {
    status_2 = 3;
  }
  else if(j2x>600)  {
    status_2 = 4;
  }
  else if(j2y>600)  {
    status_2 = 1;
  }
  else if(j2y<100)  {
    status_2 = 2;
  }
  else
    status_2 = 0;
}

void read_J1()  {
  j1x = analogRead(A1);
  j1y = analogRead(A0);
  Serial.print("j1x = ");
  Serial.print(j1x);
  Serial.print("\tj1y = ");
  Serial.println(j1y);
  //     1023
  //1023      0
  //      0
}

