#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define BREAKOUT_RESET  8
#define BREAKOUT_CS     6
#define BREAKOUT_DCS    7
#define CARDCS          9
#define DREQ            2

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

void setup() {
  Serial.begin(9600);
  Serial.println("MP3 Player Initializing...");

  if (!musicPlayer.begin()) {
    Serial.println("VS1053 not found!");
    while (1);
  }
  Serial.println("VS1053 OK!");

  if (!SD.begin(CARDCS)) {
    Serial.println("SD Card failed!");
    while (1);
  }
  Serial.println("SD Card OK!");

  musicPlayer.setVolume(0, 0);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  Serial.println("Ready for commands...");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    Serial.print("MP3 Player received: ");
    Serial.println(c);

    if (c == '0') {
      Serial.println("Playing song...");
      if (!musicPlayer.startPlayingFile("/0001.mp3")) {
        Serial.println("Failed to play file!");
      }
    } else if (c == '1') {
        if (!musicPlayer.startPlayingFile("/0002.mp3")) {
          Serial.println("Failed to play file!");
      }
    }
      else if (c == '2') {
        if (!musicPlayer.startPlayingFile("/0003.mp3")) {
          Serial.println("Failed to play file!");
      }
      else if (c == 's') {
      musicPlayer.stopPlaying();
      Serial.println("Stopped playback.");
    } else if (c == 'p') {
      if (!musicPlayer.paused()) {
        musicPlayer.pausePlaying(true);
        Serial.println("Paused.");
      } else {
        musicPlayer.pausePlaying(false);
        Serial.println("Resumed.");
      }
    }
  }

  if (musicPlayer.stopped()) {
    Serial.println("Playback finished.");
    delay(1000);  // Small delay before next command
  }
  delay(10);
}
  }