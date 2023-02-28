#include <Arduino.h>
#include <SD.h>
#include <Audio.h>
#include <SerialFlash.h>

#include "Sampler.h"

#include "SDHeader.h"

// Sampler details

const long maxSampleLength = 1200000;
/*EXTMEM*/ samples samps[numSampleFiles];
/*EXTMEM*/ char fileNameBuff[80];
/*EXTMEM*/ char newfileNameBuff[80];
/*EXTMEM*/ char previewfileNameBuff[80];
/*EXTMEM*/ char rootSampleDir[25];

// Display objects
extern bool directoryDisplayed;

File currentDirectory;

#define MOSI_PIN 11
#define SCK_PIN 13
#define CS_PIN = BUILTIN_SDCARD

void Sampler::init()
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

  // Initialize Sample File Names
  for (int i = 0; i < numSampleFiles; i++)
  {
    sprintf(samps[i].fileName, "S%d.RAW", i + 1);
    if (!SD.exists(samps[i].fileName))
    {
      SerialFlash.createErasable(samps[i].fileName, maxSampleLength);
    }
  }

  // See what's in our root Sample directory
  sprintf(fileNameBuff, "/Samples");
  sprintf(rootSampleDir, "/Samples");
  currentDirectory = SD.open(fileNameBuff);
  while (true)
  {
    File entry = currentDirectory.openNextFile();
    if (!entry)
    {
      currentDirectory.rewindDirectory();
      break;
    }
    else
    {
      numItems = numItems + 1;
    }
    delay(1);
    entry.close();
    delay(1);
  }
  currentDirectory.close();
}

void Sampler::enterDirectory(char fileName[])
{
  AudioNoInterrupts();
  currentDirectory = SD.open(fileName);
  numItems = 0;
  while (true)
  {
    File entry = currentDirectory.openNextFile();
    if (!entry)
    {
      currentDirectory.rewindDirectory();
      break;
    }
    else
    {
      numItems = numItems + 1;
    }
    delay(1);
    entry.close();
    delay(1);
  }
  // oled.fillScreen(BLACK);
  directoryDisplayed = 0;
  currentDirectory.close();
  AudioInterrupts();
}

void setup()
{
}
void loop()
{
}