#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Spreadsheet
#define WIFI_SSID "momoltd2_G"
#define WIFI_PASSWORD "momo77102941"

// スプレッドシートURL
const char *host = "https://script.google.com/macros/s/AKfycbyqRoXDy0SuBLuv98izHzZQbHd4T1-JXQktlfNSpz_QQ5aogMf3OzDDNsQlElyKwZat/exec";

// 時刻設定
const char* ntpServer = "ntp.nict.jp";
const long gmtOffset_sec = 3600 * 9;
const int daylightOffset_sec = 0;
char localtm[20];   // yyyy/mm/dd hh:mm:ss

// Json設定
StaticJsonDocument<255> json_request;
char buffer[255];

extern unsigned long sv_value_Press;
extern unsigned int sv_value_Temp;
extern unsigned int sv_value_Humid;
extern char macaddr[18];


// Get local time
void printLocalTime(void) {
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
#if 1
    sprintf(localtm, "%04d/%02d/%02d %02d:%02d:%02d",
            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
#else
    sprintf(localtm, "%02d/%02d %02d:%02d:%02d",
            timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
#endif
  } else {
    Serial.println("local time get error!!");
  }
}

// Send Sensor Data
bool sendSensorData(void) {
  printLocalTime();
  //Serial.printf("Date:%s\n", localtm);
  json_request["mode"] = "add";
  //json_request["mode"] = "update";
  json_request["sendtime"] = localtm;
  json_request["atmpress"] = sv_value_Press;
  json_request["temp"] = sv_value_Temp;
  json_request["humid"] = sv_value_Humid;
  json_request["mac"] = macaddr;
  serializeJson(json_request, buffer, sizeof(buffer));

  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "application/json");
  //Serial.printf("buf=%s\n", buffer);
  int status_code = http.POST((uint8_t*)buffer, strlen(buffer));
  Serial.printf(">> status_code=[%d]\n", status_code);
  bool result = false;
  if (status_code > 0) {   
      if (status_code == HTTP_CODE_FOUND) {
        String payload = http.getString();
        Serial.println(payload);
        Serial.println("Send Done!");
        result = true;
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(status_code).c_str());
  }
  http.end();
  return result;
}

void sendSensorData_setup(void) {
  // Wi-Fi接続
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // WiFi Connected
  Serial.println("\nWiFi Connected.");
  Serial.println(WiFi.localIP());

  // Get time information.
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
