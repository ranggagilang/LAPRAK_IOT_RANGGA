#define BLYNK_DEVICE_NAME "Esp32IoT"
#define BLYNK_PRINT Serial

#define BLYNK_AUTH_TOKEN "Ev5T6Q2Ul9NdER7HqQ1Sv-9RYIJp7B-M"
#define BLYNK_TEMPLATE_ID "TMPL6G-x2Le-P"
#define BLYNK_TEMPLATE_NAME "IOT ESP RANGGA"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h> //Library untuk DHT

char auth[] = BLYNK_AUTH_TOKEN ; //Auth Token

char ssid[] = "Wokwi-GUEST"; //nama hotspot yang digunakan
char pass[] = ""; //password hotspot yang digunakan

const int DHT_PIN = 15;

int value0, value1, value2, value3, value6;

byte LED_R = 26;
byte LED_Y = 27;
byte LED_G = 14;
byte LED_B = 12;

DHTesp dht;

BlynkTimer timer;

//function untuk pengiriman sensor
void sendSensor()
{
 TempAndHumidity  data = dht.getTempAndHumidity();

//menampilkan temperature pada Serial monitor
Serial.print("% Temperature: ");
Serial.print(data.temperature);
Serial.println("C ");
Serial.print("% Kelembaban: ");
Serial.print(data.humidity);
Serial.println("% ");

Blynk.virtualWrite(V4, data.temperature); //mengirimkan data temperatur ke Virtual pin VO di Blynk Cloud
Blynk.virtualWrite(V3, data.humidity); //mengirimkan data kelemaban ke Virtual pin V1 di Blynk Cloud
}

BLYNK_WRITE(V5)
{
  int nilaiBacaIO =param.asInt();
   digitalWrite(LED_R, nilaiBacaIO);
  Blynk.virtualWrite(V6, nilaiBacaIO);
}

void setup()
{
// Debug console
Serial.begin(115200); //serial monitor menggunakan bautrate 9600
dht.setup(DHT_PIN, DHTesp::DHT22);
pinMode(LED_R, OUTPUT);

Blynk.begin(auth, ssid, pass); //memulai Blynk
timer.setInterval(1000, sendSensor); //Mengaktifkan timer untuk pengiriman data 1000ms
}

void loop()
{

Blynk.run(); //menjalankan blynk
timer.run(); //menjalankan timer
}