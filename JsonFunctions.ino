/**
   Function to serialize file to SD
   by Renzo Mischianti <https://www.mischianti.org>
   example:
   boolean isSaved = saveJSonToAFile(&doc, filename);
*/
bool saveJSonToAFile(DynamicJsonDocument *doc, String filename) {
  SD_MMC.remove(filename);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Serial.println(F("Open file in write mode"));
  myFileSDCart = SD_MMC.open(filename, FILE_WRITE);
  if (myFileSDCart) {
    Serial.print(F("Filename --> "));
    Serial.println(filename);

    Serial.print(F("Start write..."));

    serializeJson(*doc, myFileSDCart);

    Serial.print(F("..."));
    // close the file:
    myFileSDCart.close();
    Serial.println(F("done."));

    return true;
  } else {
    // if the file didn't open, print an error:
    Serial.print(F("Error opening "));
    Serial.println(filename);

    return false;
  }
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SD_MMC.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char) file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}
