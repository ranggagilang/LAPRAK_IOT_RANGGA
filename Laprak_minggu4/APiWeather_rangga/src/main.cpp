#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
String apiKey = "0c854b30810ba8e3e1f55b2eeabe6da6";
String city = "Malang";
String units = "metric";
String server = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + units + "&appid=" + apiKey;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonNext = 12;
const int buttonPrev = 14;

int scrollIndex = 0;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 60000;

// Data weather
String temp = "N/A";
String feels_like = "N/A";
String precipitation = "0"; // Default 0 mm
String windSpeed = "N/A";
String windDirection = "N/A";
String humidity = "N/A";
String clouds = "N/A";
String pressure = "N/A";
String sunrise = "N/A";
String sunset = "N/A";

// Function prototypes
void fetchWeather();
void displayData(int index);
String getValue(String data, String key);
String getOptionalValue(String data, String key);
String formatTime(String unixTimeStr);

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  
  pinMode(buttonNext, INPUT_PULLUP);
  pinMode(buttonPrev, INPUT_PULLUP);

  // Tampilan awal nama pembuat
  lcd.setCursor(0, 0);
  lcd.print("Created by");
  lcd.setCursor(0, 1);
  lcd.print("TB Rangga Gilang");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yanuari");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!");
  delay(1000);
  lcd.clear();
  
  fetchWeather(); // Fetch pertama
  displayData(scrollIndex);
}

void loop() {
  unsigned long currentMillis = millis();

  // Update data tiap 60 detik
  if (currentMillis - lastUpdate > updateInterval) {
    lastUpdate = currentMillis;
    fetchWeather();
    displayData(scrollIndex);
  }

  // Tombol Next
  if (digitalRead(buttonNext) == LOW) {
    scrollIndex++;
    if (scrollIndex > 9) scrollIndex = 0;
    displayData(scrollIndex);
    delay(300); // Debounce
  }

  // Tombol Prev
  if (digitalRead(buttonPrev) == LOW) {
    scrollIndex--;
    if (scrollIndex < 0) scrollIndex = 9;
    displayData(scrollIndex);
    delay(300); // Debounce
  }
}

void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server); 
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      temp = getValue(payload, "temp");
      feels_like = getValue(payload, "feels_like");
      humidity = getValue(payload, "humidity");
      windSpeed = getValue(payload, "speed");
      windDirection = getValue(payload, "deg");
      pressure = getValue(payload, "pressure");
      clouds = getValue(payload, "all");
      precipitation = getOptionalValue(payload, "\"rain\":{\"1h\":");
      sunrise = formatTime(getValue(payload, "sunrise"));
      sunset = formatTime(getValue(payload, "sunset"));

    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
  }
}

void displayData(int index) {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  switch(index) {
    case 0:
      lcd.print("Temp: " + temp + "C");
      break;
    case 1:
      lcd.print("Feels: " + feels_like + "C");
      break;
    case 2:
      lcd.print("Precip: " + precipitation + "mm");
      break;
    case 3:
      lcd.print("Wind: " + windSpeed + "m/s");
      break;
    case 4:
      lcd.print("Dir: " + windDirection + (char)223);
      break;
    case 5:
      lcd.print("Humidity: " + humidity + "%");
      break;
    case 6:
      lcd.print("Clouds: " + clouds + "%");
      break;
    case 7:
      lcd.print("Pressure: " + pressure + "hPa");
      break;
    case 8:
      lcd.print("Sunrise:");
      lcd.setCursor(0, 1);
      lcd.print(sunrise);
      break;
    case 9:
      lcd.print("Sunset:");
      lcd.setCursor(0, 1);
      lcd.print(sunset);
      break;
  }
}

// Fungsi parsing data biasa
String getValue(String data, String key) {
  int startIndex = data.indexOf(key);
  if (startIndex == -1) return "N/A";
  startIndex = data.indexOf(":", startIndex) + 1;
  int endIndex = data.indexOf(",", startIndex);
  if (endIndex == -1) endIndex = data.indexOf("}", startIndex);
  String value = data.substring(startIndex, endIndex);
  value.trim();
  return value;
}

// Fungsi parsing rain opsional
String getOptionalValue(String data, String key) {
  int start = data.indexOf(key);
  if (start == -1) return "0"; // kalau tidak ada hujan, return 0
  start += key.length();
  int end = data.indexOf(",", start);
  if (end == -1) end = data.indexOf("}", start);
  String value = data.substring(start, end);
  value.trim();
  return value;
}

// Format UNIX Time ke WIB jam:menit
String formatTime(String unixTimeStr) {
  unsigned long unixTime = unixTimeStr.toInt();
  
  // Tambah offset 7 jam untuk WIB
  unixTime += 7 * 3600;

  int hours = (unixTime % 86400L) / 3600;
  int minutes = (unixTime % 3600) / 60;
  
  char buffer[6];
  sprintf(buffer, "%02d:%02d", hours, minutes);
  return String(buffer);
}
