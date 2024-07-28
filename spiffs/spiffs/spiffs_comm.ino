/*
 * File name: spiffs.ino
 * Author: fumihiko kakuma
 * Date:   2024/07/23
 */

#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define FILE_PATH_WIFI "/wifiindex.txt"

bool spiffs_en = false;

bool readData(fs::FS &fs, const char * path, int *data) {
  String str;
  bool res = false;

  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, FILE_READ);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return false;
  }
  file.seek( 0 );
  while (file.available()) {
    str += char(file.read());
  }
  *data = str.toInt();
  Serial.printf("- read data:%d\n", *data);
  res = true;
  file.close();
  return res;
}

bool writeData(fs::FS &fs, const char * path, const int *data) {
  String str;
  bool res = false;

  Serial.printf("- Writing file: %s data: %d\n", path, *data);

  File file = fs.open(path, FILE_WRITE, true);
  if (!file) {
      Serial.println("- failed to open file for writing");
      return false;
  }
  file.seek( 0 );
  if (file.print(String(*data))) {
      Serial.println("- file written");
      res = true;
  } else {
      Serial.println("- write failed");
  }
  file.close();
  return res;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

bool deleteFile(fs::FS &fs, const char * path) {
    bool res = false;

    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
        res = true;
    } else {
        Serial.println("- delete failed");
    }
    return res;
}

void setup_spiffs(void) {
  uint8_t setup_counter;

  /* Initialize SPIFFS */
  while (setup_counter < 50) {
    if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED) == true) {
      spiffs_en = true;
      break;
    } else {
      Serial.println("SPIFFS begin(): failed");
      delay(100);
    }
    setup_counter++;
  }
}
