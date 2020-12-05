#include <ArduinoJson.h>
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include <WiFi.h>
#include "time.h"
#include "configWifi.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char *filename = "/test.jso";  // <- SD library uses 8.3 filenames

File myFileSDCart;

struct tm timeinfo;

void initWifi() {
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

/**
   Function to deserialize file from SD
   by Renzo Mischianti <https://www.mischianti.org>
   example:
    DynamicJsonDocument doc(1024);
    JsonObject obj;
    obj = getJSonFromFile(&doc, filename);
*/
JsonObject getJSonFromFile(DynamicJsonDocument *doc, String filename, bool forceCleanONJsonError = true ) {
  // open the file for reading:
  myFileSDCart = SD_MMC.open(filename);
  if (myFileSDCart) {
    // read from the file until there's nothing else in it:
    //          if (myFileSDCart.available()) {
    //              firstWrite = false;
    //          }

    DeserializationError error = deserializeJson(*doc, myFileSDCart);
    if (error) {
      // if the file didn't open, print an error:
      Serial.print(F("Error parsing JSON "));
      Serial.println(error.c_str());

      if (forceCleanONJsonError) {
        return doc->to<JsonObject>();
      }
    }

    // close the file:
    myFileSDCart.close();

    return doc->as<JsonObject>();
  } else {
    // if the file didn't open, print an error:
    Serial.print(F("Error opening (or file not exists) "));
    Serial.println(filename);

    Serial.println(F("Empty json created"));
    return doc->to<JsonObject>();
  }
}

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial)
    continue;

  delay(500);

  // Initialize SD library
  while (!SD_MMC.begin()) {
    Serial.println(F("Failed to initialize SD library"));
    delay(1000);
  }

  Serial.println(F("SD library initialized"));

  Serial.println(F("Delete original file if exists!"));
  SD_MMC.remove(filename);

  initWifi();
}

void loop() {
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //  StaticJsonDocument<512> doc;
  // You can use DynamicJsonDocument as well
  DynamicJsonDocument doc(10240);

  JsonObject obj;
  obj = getJSonFromFile(&doc, filename);

  //obj[F("millis")] = millis();
  //obj[F("timeInf")] = printLocalTime();

  JsonArray data;
  // Check if exist the array
  if (!obj.containsKey(F("data"))) {
    Serial.println(F("Not find data array! Crete one!"));
    data = obj.createNestedArray(F("data"));
  } else {
    Serial.println(F("Find data array!"));
    data = obj[F("data")];
  }

  // create an object to add to the array
  JsonObject objArrayData = data.createNestedObject();
  
  objArrayData["timeInf"] = printLocalTime();
  objArrayData["prevNumOfElem"] = data.size();
  objArrayData["newNumOfElem"] = data.size() + 1;

  boolean isSaved = saveJSonToAFile(&doc, filename);

  if (isSaved) {
    Serial.println("File saved!");
  } else {
    Serial.println("Error on save File!");
  }

  // Print test file
  Serial.println(F("Print test file..."));
  
  printFile(filename);

  delay(5000);
}
