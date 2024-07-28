/*
 * File name: spiffs.ino
 * Author: fumihiko kakuma
 * Date:   2024/07/23
 */

#include "esp_bt_main.h"
#include "esp_bt.h"

#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define FILE_PATH_WIFI "/wifiindex.txt"

//#define FUNC_WIFI_INI
//#define FUNC_WIFI_READ
#define FUNC_WIFI_DEL

#define LOOP_DELAY_TIME 10  // 10 sec

RTC_DATA_ATTR int rtc_wifi_index = -1;
bool result = false;

extern bool spiffs_en;

extern bool readData(fs::FS &fs, const char * path, int *data);
extern bool writeData(fs::FS &fs, const char * path, const int *data);
extern void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
extern bool deleteFile(fs::FS &fs, const char * path);
extern void setup_spiffs(void);

/*
 * ESP32 setup
 */
void setup() {
  uint8_t dev_counter;
  Serial.begin(115200);
  while (!Serial && (dev_counter < 50)) { dev_counter++; delay(100);}

  setup_spiffs();
  if (spiffs_en) {
    listDir(SPIFFS, "/", 0);
  }
}

/*
 * Loop process
 */
void loop() {
  int set_wifi_index = -1;

  if (spiffs_en) {
#if defined(FUNC_WIFI_INI)
    Serial.println("== Initialize SPIFFS file");
    readData(SPIFFS, FILE_PATH_WIFI, &rtc_wifi_index);
    Serial.printf(">> current rtc_wifi_index=%d\n", rtc_wifi_index);
    if (set_wifi_index == rtc_wifi_index) {
      Serial.println("# Data is already written");
    } else {
      writeData(SPIFFS, FILE_PATH_WIFI, &set_wifi_index);
    }
#elif defined(FUNC_WIFI_READ)
    Serial.println("== Read SPIFFS file");
    readData(SPIFFS, FILE_PATH_WIFI, &rtc_wifi_index);
    //Serial.printf(">> current rtc_wifi_index=%d\n", rtc_wifi_index);
    if (set_wifi_index == rtc_wifi_index) {
      Serial.println("# The data in the SPIFFS file is the same as the specified value.");
      Serial.printf(">> Specified value   :%d\n", set_wifi_index);
    } else {
      Serial.println("# The data in the SPIFFS file differs from the specified value.");
      Serial.printf(">> Specified value   :%d\n", set_wifi_index);
      Serial.printf(">> SPIFFS file's data:%d\n", rtc_wifi_index);
    }
#elif defined(FUNC_WIFI_DEL)
    Serial.println("== Delete SPIFFS file");
    if (!result) {
      result = deleteFile(SPIFFS, FILE_PATH_WIFI);
    }
    listDir(SPIFFS, "/", 0);
#endif
  }
  Serial.printf("delay:%d sec\n", LOOP_DELAY_TIME);
  delay(LOOP_DELAY_TIME * 1000);
}
