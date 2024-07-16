/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <string.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <WiFi.h>

//#define TARGET_ADDR "ec:da:3b:bb:e7:46"
//#define TARGET_MANU 0xD502    // OMERON
// OMERON Manufacture
#define TARGET_MANU
#define TARGET_MANU_STR "d502"
#define TARGET_MANU_HEX 0xd502
//#define CHECK_ADDR "ec:da:3b:bb:e7:46"

// BLE
#define advData_top   0
#define advData_manu  advData_top
#define advData_fix   (advData_manu + 2)
#define advData_seq   (advData_fix + 1)
#define advData_temp  (advData_seq + 1)
#define advData_humid (advData_temp + 2)
#define advData_lux   (advData_humid + 2)
#define advData_press (advData_lux + 2)

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
int deviceCount;

unsigned long value_Press;
unsigned int value_Temp;
unsigned int value_Humid;

unsigned long sv_value_Press;
unsigned int sv_value_Temp;
unsigned int sv_value_Humid;

bool get_ble_data = false;
char macaddr[18];

extern void sendSensorData_setup(void);
extern bool sendSensorData(void);

/*
 * Main process
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      int i;
      char *cHex;
      bool getData = false;

      for (i = 0; i < deviceCount; i++) {
        char *bHex = (char *)advertisedDevice.getManufacturerData().data();
        int manu = (*bHex << 8) + *(bHex + 1);
        value_Press = *(bHex + advData_press) + (*(bHex + advData_press + 1) << 8);
        value_Press += (*(bHex + advData_press + 2) << 16) + (*(bHex + advData_press + 3) << 24);
        value_Temp = *(bHex + advData_temp) + (*(bHex + advData_temp + 1) << 8);
        value_Humid = *(bHex + advData_humid) + (*(bHex + advData_humid + 1) << 8);
#if defined(TARGET_ADDR)
        //char *addr = (char *)advertisedDevice.getAddress().toString().c_str();
        strncpy(macaddr, (char *)advertisedDevice.getAddress().toString().c_str(), sizeof(macaddr));
        if (strncmp(macaddr, TARGET_ADDR, (sizeof(TARGET_ADDR) - 1)) == 0) {
#elif defined(TARGET_MANU)
        if (TARGET_MANU_HEX == manu) {
          //char *addr = (char *)advertisedDevice.getAddress().toString().c_str();
          strncpy(macaddr, (char *)advertisedDevice.getAddress().toString().c_str(), sizeof(macaddr));
#if defined(CHECK_ADDR)
          if (strncmp(macaddr, CHECK_ADDR, (sizeof(CHECK_ADDR) - 1)) != 0) {
            Serial.printf("different macaddr:%s\n", macaddr);
            continue;
          }
#endif /* CHECK_ADDR */
#endif /* TARGET_ADDR */
          getData = true;
          //Serial.printf("adress: %s\n", macaddr);
          cHex = BLEUtils::buildHexData(nullptr,
              (uint8_t*)advertisedDevice.getManufacturerData().data(),
              advertisedDevice.getManufacturerData().length());
          //Serial.printf("manufacturer data: %s\n", cHex);
          //Serial.printf("Pre: %d, Tmp:%d, Hum:%d\n", value_Press, value_Temp, value_Humid);
          sv_value_Press = value_Press;
          sv_value_Temp = value_Temp;
          sv_value_Humid = value_Humid;
          get_ble_data = true;
          //break;
/*
        } else {
          Serial.printf("adress: %s\n", macaddr);
          Serial.printf("non-target manufacturer data: %s\n", cHex);
*/
        }
      }
      //Serial.printf("deviceCount=%d\n", deviceCount);
      if (getData) {
        Serial.printf("adress: %s\n", macaddr);
        Serial.printf("manufacturer data: %s\n", cHex);
        Serial.printf("Pre: %d, Tmp:%d, Hum:%d\n", value_Press, value_Temp, value_Humid);
      }
    }
};

void BLE_setup(void) {
  BLEDevice::deinit("");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //use in passive scan.
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

/*
 * ESP32 setup
 */
void setup() {
  uint8_t dev_counter;

  Serial.begin(115200);
  while (!Serial && (dev_counter < 50)) { dev_counter++; delay(100);}
  Serial.println("Scanning...");

  sendSensorData_setup();
  BLE_setup();
}

/*
 * ESP32 loop process
 */
void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  deviceCount = foundDevices.getCount();
  //Serial.println(foundDevices.getCount());
  //Serial.println(deviceCount);
  //Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  if (get_ble_data) {
#if defined(ARDUINO_ESP32_DEV)
    delay(1000);
#endif
    if (!sendSensorData()) {
      WiFi.disconnect(true, true);
      Serial.println("Do deep-sleep!!");
      esp_sleep_enable_timer_wakeup(2000);
      esp_deep_sleep_start();
    }
  }
  //delay(200);
}
