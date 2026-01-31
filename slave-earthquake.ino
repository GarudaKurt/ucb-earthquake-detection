#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial dfSerial(11, 10); // RX = 11, TX = 10
DFRobotDFPlayerMini dfPlayer;

const unsigned long PLAY_DURATION = 15000; // 15 seconds
bool isPlaying = false;
unsigned long playStartTime = 0;

void setup() {
  Serial.begin(9600);
  dfSerial.begin(9600);

  Serial.println("Initializing DFPlayer...");
  delay(3000);
  if (!dfPlayer.begin(dfSerial)) {
    Serial.println("DFPlayer Mini not detected!");
    while (true);
  }

  Serial.println("DFPlayer Mini ready");
  dfPlayer.volume(30);   // Volume: 0–30
  dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
}

void loop() {
  // Check for command from master
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'S' && !isPlaying) {
      Serial.println("Received 'S', playing MP3...");
      dfPlayer.play(1);  // play track 1
      playStartTime = millis();
      isPlaying = true;
    }
  }

  // Stop after 15 seconds
  if (isPlaying && millis() - playStartTime >= PLAY_DURATION) {
    dfPlayer.stop();
    isPlaying = false;
    Serial.println("Finished 15 seconds playback");
  }
}
