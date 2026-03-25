#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "TMPL6WGiD-pVn"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitor"
#define BLYNK_AUTH_TOKEN    "MdlCC1N2wHYk_YvZqnB9ZrtiA5_cleL6"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <GP2YDustSensor.h> 
#include "time.h"

char ssid[] = "klong";         
char pass[] = "klong123";      
String NEW_SCRIPT_ID = "AKfycbxfF-CSYtUpB2ZKFBsacDbP9eomLylR0wwWZYfkcG81OK09EJnceqH74xQA5AAMzses"; 

#define DHTPIN 14
#define MQ135_PIN 35
#define DUST_LED_PIN 25
#define DUST_MEASURE_PIN 34

DHT dht(DHTPIN, DHT11);
GP2YDustSensor dustSensor(GP2Y1010AU0F, DUST_LED_PIN, DUST_MEASURE_PIN);
WebServer server(80);

// khai báo
float t = 0, h = 0, d = 0, pm25 = 0, pm10 = 0;
int g = 0;
String formattedTime = "00:00:00";
String dataBuffer = ""; 
int count = 0;
const int batchLimit = 12; 
unsigned long thoiGianTruocDoc = 0;
const long chuKyDoc = 5000; 
//cảnh báo blynk
bool autoWarning = false;
bool daCanhBao = false;

//ngưỡng cảnh báo 
#define TEMP_MAX 35
#define TEMP_MIN 10
#define PM_MAX   150
#define GAS_MAX  1000

// các hàm xử lý 
void updateLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    formattedTime = "Loi gio";
    return;
  }
  char buf[20];
  strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
  formattedTime = String(buf);
}

void guiLenGoogleGop() {
  if (WiFi.status() == WL_CONNECTED && dataBuffer != "") {
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + NEW_SCRIPT_ID + "/exec?allData=" + dataBuffer;
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("\n[Google Sheet] Da gui goi du lieu! (Code: %d)\n", httpCode);
      dataBuffer = ""; 
      count = 0;
    }
    http.end();
  }
}

void checkAutoWarning() {
  if (!autoWarning) {
    daCanhBao = false;
    return;
  }
  bool nguyHiem = false;
  if (t > TEMP_MAX || t < TEMP_MIN) nguyHiem = true;
  if (pm25 > PM_MAX) nguyHiem = true;
  if (g > GAS_MAX) nguyHiem = true;

  if (nguyHiem && !daCanhBao) {
    Blynk.logEvent("auto_warning", "CANH BAO: Moi truong khong an toan!");
    daCanhBao = true;
  }
  if (!nguyHiem) daCanhBao = false;
}

//BLYNK & SERVER
BLYNK_WRITE(V3) {   
  if (param.asInt() == 1) {
    String statusMsg = "T:" + String(t,1) + " PM2.5:" + String(pm25,1);
    Blynk.logEvent("check_data", statusMsg);
  }
}

BLYNK_WRITE(V4) {   
  autoWarning = param.asInt();
  Serial.println(autoWarning ? "\n[BLYNK] AUTO WARNING: ON" : "\n[BLYNK] AUTO WARNING: OFF");
}

void handleRoot() {
  String s = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  s += "<title>Simon Air Monitor</title>";
  s += "<script>setInterval(function(){fetch('/data').then(r=>r.json()).then(d=>{";
  s += "document.getElementById('t').innerHTML=d.t; document.getElementById('h').innerHTML=d.h;";
  s += "document.getElementById('g').innerHTML=d.g; document.getElementById('p25').innerHTML=d.p25;";
  s += "document.getElementById('time').innerHTML=d.time;});}, 2000);</script></head><body>";
  s += "<h1>Air Monitor Dashboard</h1><h2 id='time'>--:--:--</h2>";
  s += "<div>Temp: <span id='t'>0</span> | Hum: <span id='h'>0</span> | PM2.5: <span id='p25'>0</span> | Gas: <span id='g'>0</span></div>";
  s += "</body></html>";
  server.send(200, "text/html", s);
}

void handleData() {
  String json = "{\"t\":" + String(t) + ",\"h\":" + String(h) + ",\"g\":" + String(g) + 
                ",\"d\":" + String(d) + ",\"p25\":" + String(pm25) + ",\"p10\":" + String(pm10) + 
                ",\"time\":\"" + formattedTime + "\"}";
  server.send(200, "application/json", json);
}

//SETUP
void setup() {
  Serial.begin(115200);
  dht.begin();
  dustSensor.begin();
  pinMode(MQ135_PIN, INPUT);

  Serial.println("\n--- Dang ket noi Blynk & WiFi ---");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  configTime(7 * 3600, 0, "pool.ntp.org");
  Serial.println("\nDang dong bo gio...");
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("\n[THONG BAO] He thong bat dau do du lieu...");
  Serial.println("------------------------------------------------------------------------------------");
  Serial.println("Gio         | Nhiet do | Do am | Gas (ADC) | PM 2.5 | PM 10");
  Serial.println("------------------------------------------------------------------------------------");
}

//LOOP 
void loop() {
  Blynk.run();           
  server.handleClient(); 
  
  unsigned long currentMillis = millis();

  if (currentMillis - thoiGianTruocDoc >= chuKyDoc) {
    thoiGianTruocDoc = currentMillis;
    updateLocalTime();

    t = dht.readTemperature();
    h = dht.readHumidity();
    g = analogRead(MQ135_PIN);
    d = dustSensor.getDustDensity(); 
    
    if (isnan(t) || isnan(h)) { t = 0; h = 0; }

    pm25 = d * 0.54; 
    pm10 = d * 0.9;

    // Gửi Blynk
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, pm25);
    Blynk.virtualWrite(V5, g);
    Blynk.virtualWrite(V6, pm10);

    checkAutoWarning();

    // Buffer Google Sheets
    dataBuffer += formattedTime + "," + String(t) + "," + String(h) + "," + String(g) + "," + String(pm25) + "," + String(pm10) + "|";
    count++;
    
    // Hiển thị Serial Monitor
    Serial.printf("%s    | %.1f C   | %.1f%% | %-9d | %.2f   | %.2f\n", formattedTime.c_str(), t, h, g, pm25, pm10);

    if (count >= batchLimit) {
      guiLenGoogleGop();
    }
  }
}