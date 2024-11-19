#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define FAN 25
#define Peltier 26

#define DHTPIN 23
#define DHTTYPE DHT22
float t;


LiquidCrystal_I2C lcd(0x27, 16, 2);
const char *ssid = "JollNoy";  // ชื่อ wfii
const char *password = ""; // รหัส wifi
const char *host = "api.thingspeak.com";
const char *api_key = "B1XLF4J8Q6WPOSMR";
unsigned long time1, time2;

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
DynamicJsonDocument doc(2048);
DeserializationError error;

int connectHost() {
  if (!client.connect(host, 80)) {
    Serial.println(" !!! Connection failed !!! ");
    delay(10);
    return 0;
  } else {
    return 1;
  }
}

void setup() {

  pinMode(FAN, OUTPUT);
  pinMode(Peltier, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Boot Start!!");
  lcd.setCursor(0, 1);
  lcd.print("Boot Start!!");
  delay(2000);
  lcd.clear();

  WiFi.disconnect();
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {


 if (millis() - time2 >= 15000)
  { // delay 1 sec
    time2 = millis();
    t = dht.readTemperature();
    if (isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("\t");

      connectHost();
      String url = "/update?api_key=" + String(api_key) + "&field1=" + String(t);
      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.println("GET " + url + " HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Connection: close");
      client.println();
  }

//*******************************
// Turn On/Off Device
//*******************************

  if (millis() - time1 >= 5000) {  // delay 10 sec and start if 
    Serial.println();
    time1 = millis();
    connectHost();

    String url = "/channels/2256770/feeds"; // /channels/<เปลี่ยนเป็นของตนเอง>/feeds
    url = url + ".json?results=";
    url = url + "1";
   
    client.println(String("GET ") + url + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();
    unsigned long timeout = millis();
    while (client.available() == 0) { // start while
      if (millis() - timeout > 5000) { // start if
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      } // end if millis() - timeout > 5000
    } // end while client.available() == 0

    String line = "";
    while (client.available()) { // start while
      line = line + char(client.read());
      delay(1);
    } // end while client.available()

    delay(10);
    client.stop();
    delay(10);
    
    int JSfirst = line.indexOf('{');
    int JSlast = line.lastIndexOf('}');
    line.remove(JSlast + 2);
    line.remove(0, JSfirst);
    error = deserializeJson(doc, line);

    if (error) { // start if
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    } // end if error

    String  feeds = doc["feeds"];
    feeds.replace("[", "");
    feeds.replace("]", "");
    error = deserializeJson(doc, feeds);
    
    if (error) { // start if 
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    } // end if error

    String field4 = doc["field4"];
  
    
    Serial.print("Sent: ");
    Serial.print(field4);
  
  if(field4 == "1"){
Serial.println(" Device is : Turn On");
digitalWrite(FAN, HIGH);
digitalWrite(Peltier, HIGH);
  }
  else{
Serial.println(" Device is : Turn Off");
digitalWrite(FAN, LOW);
digitalWrite(Peltier, LOW);
  }

  } 

//*******************************
// Sent Temperature to ThingSpeak
//*******************************


} // end void loop


