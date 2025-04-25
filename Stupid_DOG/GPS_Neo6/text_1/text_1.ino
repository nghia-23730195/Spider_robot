#include <SoftwareSerial.h>
#include <TinyGPS++.h>
SoftwareSerial gpsSerial(8,9); //Tx-Rx nano
TinyGPSPlus gps;
float lattitude,longitude;
String viTri = "";

void setup() {
  gpsSerial.begin(9600);
  Serial.begin(9600);
}

void loop()
{
  while (gpsSerial.available())
  {
    int data = gpsSerial.read();
    if (gps.encode(data))
    {
      lattitude = (gps.location.lat());
      longitude = (gps.location.lng());
      /*
      Serial.print ("lattitude: ");
      Serial.println (lattitude,5);
      Serial.print ("longitude: ");
      Serial.println (longitude,5);
      */
      String lat, longi;
      lat = String(lattitude,5);
      longi = String(longitude,5);
      viTri = lat + "," + longi;
      Serial.println(viTri);
    }
  }
}
