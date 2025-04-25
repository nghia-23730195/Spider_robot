#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 9); // CE, CSN
//defaul SPI config: MOSI:11  MISO:12   SCK:13 
const byte address[6] = "00011";

int valueA1;
int valueJ2;

byte array[2];

void setup() 
{
  Serial.begin(9600);
  
  if (!radio.begin()) 
  {
    Serial.println("ĐANG KHỞI ĐỘNG!");
    while (1) {}
  }    
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);  
  radio.startListening(); //cấu hình là Rx
  if (!radio.available())
  {
    Serial.println("ĐANG KẾT NỐI!");
  }
}

void loop() 
{
  if (radio.available()) 
  {
    radio.read(&array, sizeof(array));
    Serial.print("Status: "); 
    Serial.print(array[0]); 
    Serial.print("\tA1: "); 
    Serial.println(array[1]);
  } 
  else if(!radio.available()) {
    delay(200);
    if(!radio.available())  {
      Serial.println("Chạy GPS");
    }
  }
}
