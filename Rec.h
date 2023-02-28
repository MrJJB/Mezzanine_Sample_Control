

// ----------------------- THIS FILE HANDLES THE AUDIO DATA RECORDED INTO THE MIC (TEENSY) ------------------

#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Audio.h>
#include <Bounce.h>
#include <Wire.h>
// #include <Menu_2>

// #include <AudioZero.h>
//  #include <fstream>

//------------------------ Hypothetical Hardware connections ------------------------//
AudioInputI2S i2s2; // Mic input?
// AudioAnalyzePeak peak1;  //
AudioRecordQueue queue1; // Audio recording queue?

// AudioPlaySdRaw playRaw8; // Channel 8
// AudioPlaySdRaw playRaw7; // Channel 7
// AudioPlaySdRaw playRaw6; // Channel 6
// AudioPlaySdRaw playRaw5; // Channel 5
// AudioPlaySdRaw playRaw4; // Channel 4
// AudioPlaySdRaw playRaw3; // Channel 3
// AudioPlaySdRaw playRaw2; // Channel 2

AudioPlaySdRaw playRaw1; // Channel 1

AudioOutputI2S i2s1; // Speaker output?

// AudioConnection patchCord2(i2s2, 0, peak1, 0);  //
AudioConnection patchCord1(i2s2, 0, queue1, 0);   // Mono recording?
AudioConnection patchCord3(playRaw1, 0, i2s1, 0); // is this for stereo playback? - I think this is mono setup
AudioConnection patchCord4(playRaw1, 0, i2s1, 1); // is this for stereo playback? - ''(same signal to left and right)

AudioControlSGTL5000 sgtl5000_1; // xy=265,212

// Three pushbuttons need to be connected:
//   Record Button: pin 0 to GND
//   Stop Button:   pin 1 to GND
//   Play Button:   pin 2 to GND

Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop = Bounce(1, 8);
Bounce buttonPlay = Bounce(2, 8); // 8 = 8ms debounce time

void startRecording();
void continueRecording();
void stopRecording();
void startPlaying_Recording();
void startPlaying_Preset();
void adjustMicLevel();
void nameRecording();

const int myInput = AUDIO_INPUT_LINEIN; // Input on Audio Shield.

#define MOSI_PIN 11
#define SCK_PIN 13
#define CS_PIN = BUILTIN_SDCARD

int mode = 0; // 0 = paused, 1 = recording, 2 = playing (back?)

File frec; // File where audio data is recorded into.

void setup()
{
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  // Buffer for incoming audio
  AudioMemory(60); // 1 unit = 128 Audio samples ~ 3 ms seconds. Therefore: 60 = 3 minutes.

  // Enable Teensy audio shield, select input (this will change?), enable output.
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  // Initialize SD card
  SPI.setMOSI(MOSI_PIN);
  SPI.setSCK(SCK_PIN);
  if (!(SD.begin(BUILTIN_SDCARD)))
  {
    // stop here if no SD card, but print a message
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void loop()
{
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  if (millis() > 5000 && mode == 0)
  {

    startRecording();
  }

  if (mode == 1)
  {
    continueRecording();
  }

  if (millis() > 15000 && mode == 1)
  {

    stopRecording();
    mode = 5;
  }

  if (buttonPlay.read() == HIGH)
  {
    startPlaying();
  }

  // when using a microphone, continuously adjust gain
  if (myInput == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void startRecording()
{
  Serial.println("startRecording");
  if (SD.exists("RECORD_01.RAW")) // CHANGE THIS TO NAME RECORDING FUNCTION?
  {

    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written. -----> CHANGE THIS SO THAT NEW FILES CAN BE WRITTEN -> and then stored in directory "Recordings"

    SD.remove("RECORD_01.RAW");
  }
  frec = SD.open("RECORD_01.RAW", FILE_WRITE);
  if (frec)
  {
    queue1.begin();
    mode = 1;
  }
}

void continueRecording()
{
  if (queue1.available() >= 2)
  {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();

    // write all 512 bytes to the SD card
    elapsedMicros usec = 0;

    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    // Serial.print("SD write, us=");
    // Serial.println(usec);
  }
}

void stopRecording()
{
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1)
  {
    while (queue1.available() > 0)
    {
      frec.write((byte *)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
}

void adjustMicLevel()
{
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}

void startPlaying_Recording()
{
  Serial.println("startPlaying");
  playRaw1.play("RECORD_01.RAW");
  mode = 2;
}
void startPlaying_Preset()
{
  Serial.println("startPlaying");
  playRaw1.play(presetChosen);
  mode = 2;
}

void continuePlaying()
{
  if (!playRaw1.isPlaying())
  {
    playRaw1.stop();
    mode = 0;
  }
}

void stopPlaying()
{
  Serial.println("stopPlaying");
  if (mode == 2)
    playRaw1.stop();
  mode = 0;
}

void nameRecording()
{
  String fileName = "Recording_";
  String extension = ".RAW";

  int fileNum = 1; // This should increment +1 each time?

  while (true)
  {
    ifstream file(fileName + extension); // reads the data from the file

    if (!file)
    {
      // File does not exist, create it and break the loop
      ofstream newFile(fileName + extension); // writes the data to the file
      newFile.close();
      break;
    }

    file.close();
    fileNum++;                                    // This increments it by 1 each time
    fileName = "Recording_" + to_string(fileNum); // converts numerical values to string
  }

  std::cout << "File: " << fileName + extension << std::endl;

  return 0;
}
