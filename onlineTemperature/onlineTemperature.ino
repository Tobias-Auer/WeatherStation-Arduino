//Libraries
#include <DHT.h>
//Constants
#define DHTPIN 2       // what pin we're connected to
#define DHTTYPE DHT22  // DHT 22  (AM2302)

// Choose library to load
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#elif defined(ARDUINO_ARCH_MBED) || defined(ARDUINO_ARCH_RP2040)
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#else
#include <WiFiNINA.h>
#endif
#else  // ESP32
#include <WiFi.h>
#endif

#include <JSON_Decoder.h>

#include <OpenWeather.h>

// Just using this library for unix time conversion
#include <Time.h>

DHT dht(DHTPIN, DHTTYPE);  //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
float hum;   //Stores humidity value
float temp;  //Stores temperature value
// =====================================================
// ========= User configured stuff starts here =========
// Further configuration settings can be found in the
// OpenWeather library "User_Setup.h" file

#define TIME_OFFSET 1UL * 3600UL  // UTC + 0 hour
// Change to suit your WiFi router
#define WIFI_SSID "**************
#define WIFI_PASSWORD "*************"

// OpenWeather API Details, replace x's with your API key
String api_key = "********************";  // Obtain this from your OpenWeather account

// Set both your longitude and latitude to at least 4 decimal places
String latitude = "50.11";  // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "8.86";  // 180.000 to -180.000 negative for West

String units = "metric";  // or "imperial"
String language = "de";   // See notes tab

// =========  User configured stuff ends here  =========
// =====================================================

OW_Weather ow;  // Weather forecast library instance

void setup() {
  Serial.begin(115200);
  int tasks = 5;

  // ======================================================================================================================== //
  // ====+++++++++++++++++++++++++++++++++++++++++++++RESET CONNECTION++++++++++++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  delay(2000);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.print("progressBar.val=0");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  // ========================================================================================================================== //
  // ====-------------------------------------------RESET CONNECTION ENDE-------------------------------------------------==== //
  // ======================================================================================================================== //
  // ====++++++++++++++++++++++++++++++++++++++++++START TEMPERATURESENSOR++++++++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  Serial.print("t0.txt=\"Temperatursensor wird gestartet...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  dht.begin();
  Serial.print("progressBar.val=" + String(100 / tasks * 1));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  // ========================================================================================================================== //
  // ====-------------------------------------------START TEMPERATURSENSOR END--------------------------------------------==== //
  // ======================================================================================================================== //
  // ====++++++++++++++++++++++++++++++++++++++++++++++cONNECT TO WIFI++++++++++++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  Serial.print("t0.txt=\"Verbinde mit dem Wlan...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int valueForTick = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (valueForTick == 360) {
      valueForTick = 0;
    } else {
      valueForTick += 90;
    }
    Serial.print("animation.val=" + String(valueForTick));
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    delay(250);
  }
  Serial.print("progressBar.val=" + String(100 / tasks * 2));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.print("animation.val=" + String(0));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  // ========================================================================================================================== //
  // ====-----------------------------------------------CONNECT TO WIFI END-----------------------------------------------==== //
  // ======================================================================================================================== //
  // ====++++++++++++++++++++++++++++++++++++++++++++++++GET SENSOR VALUE+++++++++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  Serial.print("t0.txt=\"Frage Werte vom Sensor ab...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("progressBar.val=" + String(100 / tasks * 3));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  // ========================================================================================================================== //
  // ====----------------------------------------------GET SENSOR VALUE END-----------------------------------------------==== //
  // ======================================================================================================================== //
  // ====++++++++++++++++++++++++++++++++++++++++++++++++++++GET TIME+++++++++++++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  Serial.print("t0.txt=\"Frage Werte vom Sensor ab...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);

  if (sender.begin(wifiClient, "https://worldtimeapi.org/api/timezone/Europe/Berlin")) {
    // HTTP-Code der Response speichern
    int httpCode = sender.GET();
    if (httpCode > 0) {
      if (httpCode == 200) {
        const char *json = sender.getString().c_str();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, json);

        const String time = doc["datetime"];

        year = time.substring(0, 4);
        month = time.substring(5, 7);
        day = time.substring(8, 10);
        hour = time.substring(11, 13);
        minute = time.substring(14, 16);
        second = time.substring(17, 19);

        // TODO: sent time

      } else {
        // critical error
        Serial.print("t0.txt=\"Synchronisiation fehlgeschalgen!\"");
        Serial.write(0xFF);
        Serial.write(0xFF);
        Serial.write(0xFF);
        delay(3);
      }
    } else {
      time();  // sometimes the request fails --> restart
    }
  }
  Serial.print("progressBar.val=" + String(100 / tasks * 4));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);


  // ========================================================================================================================== //
  // ====---------------------------------------------------GET TIME END--------------------------------------------------==== //
  // ======================================================================================================================== //
  // ====+++++++++++++++++++++++++++++++++++++++++GET VALUES FROM OPENWEATHERMAP++++++++++++++++++++++++++++++++++++++++==== //
  // ====================================================================================================================== //
  Serial.print("t0.txt=\"Frage Werte von OpenWeatherMap ab...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  // Create the structures that hold the retrieved weather
  OW_current *current = new OW_current;
  OW_hourly *hourly = new OW_hourly;
  OW_daily *daily = new OW_daily;

  //On the ESP8266 (only) the library by default uses BearSSL, another option is to use AXTLS
  //For problems with ESP8266 stability, use AXTLS by adding a false parameter thus       vvvvv
  //ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language, false);

  ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);

  // ========================================================================================================================== //
  // ====----------------------------------------GET VALUES FROM OPENWEATHERMAP END---------------------------------------==== //
  // ======================================================================================================================== //

  Serial.print("progressBar.val=" + String(100 / tasks * 5));
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.print("t0.txt=\"DONE...\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  delay(1000);
  Serial.print("page 1");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

void loop() {}
