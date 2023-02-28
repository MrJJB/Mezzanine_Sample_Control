/*

#include <Arduino.h>
#include <SD.h>
#include <Audio.h>

#define _SD_DAT3 46

#define MOSI_PIN 11
#define SCK_PIN 13
#define CS_PIN = BUILTIN_SDCARD

File root;

void setup()
{

 Serial.begin(9600);
 SPI.setMOSI(MOSI_PIN);
 SPI.setSCK(SCK_PIN);

 Serial.println("Initializing MicroSD"); // Message whilst waiting for the SD card to be inserted
 if (!SD.begin(BUILTIN_SDCARD))          // If not inserted
 {
  Serial.println("MicroSD card not initialized"); // Infinite Loop so that it is clear no SD card has been inserted
  while (1)
   ;
 }
 Serial.println("MicroSD card initialized!"); // otherwise print message to show it has been inserted

 root = SD.open("/");

 printDirectory(root, 0);

 Serial.println("done!");
}


void printDirectory(File dir, int numTabs)
{

 Serial.begin(9600);

 while (true)
 {

  File entry = dir.openNextFile();

  if (!entry)
  {

   // no more files

   break;
  }

  for (uint8_t i = 0; i < numTabs; i++)
  {

   Serial.print('\t');
  }

  Serial.print(entry.name());

  if (entry.isDirectory())
  {

   Serial.println("/");

   printDirectory(entry, numTabs + 1);
  }
  else
  {

   // files have sizes, directories do not

   Serial.print("\t\t");

   Serial.println(entry.size(), DEC);
  }

  entry.close();
 }
}

void loop()
{
 // nothing
}


*/