#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"

// Define the pin where the DHT22 data pin is connected
#define DHTPIN 5

// Define the type of DHT sensor
#define DHTTYPE DHT22

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Define LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledR = 12;
bool ledState = LOW;

const char* WIFI_NAME = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const int myChannelNumber = 2576954;
const char* myApiKey = "ZS3SN69BU4252UM8";
const char* server = "api.thingspeak.com";

WiFiClient client;

void setup() {
  pinMode(ledR, OUTPUT);

  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Connect to WiFi
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi not connected");
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");
  }
  Serial.println("Wifi connected!");
  Serial.println("Local IP: " + String(WiFi.localIP()));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.setCursor(0, 1);
  lcd.print("IP: " + String(WiFi.localIP()));

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor error");
    return;
  }

  // Print the results to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Display temperature and humidity on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  // Check temperature and humidity thresholds, and control the LED accordingly
  if (temperature > 45 || temperature < 10 || humidity > 70 || humidity < 20) {
    ledState = HIGH; // Turn on the LED
  } else {
    ledState = LOW; // Turn off the LED
  }

  // Set the LED state
  digitalWrite(ledR, ledState);

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myApiKey);

  if (x == 200) {
    Serial.println("Data pushed successfully");
  } else {
    Serial.println("Push error: " + String(x));
  }
  Serial.println("---");

  // Wait a few seconds between measurements
  delay(2000);
}
