#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define ESP8266_LED 5

const char* ssid = "";
const char* password = "";

const int DISPLAY_TIME = 1500;
const int PAUSE_TIME = 700;

const int LATCH_PIN = 4; // Green
const int CLOCK_PIN = 0; // Yellow
const int DATA_PIN = 13; // Blue

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  delay(50);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDone");


  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  write_bits(0, 0, 0, 0);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("making request");
    HTTPClient http;

    http.begin("http://jeremyfgsharp.pythonanywhere.com/pull");
    int code = http.GET();
    Serial.println(code);
    if (code > 0) {
      String payload = http.getString();
      payload.toLowerCase();
      Serial.println(payload);
      http.end();
      if (payload.length() != 0) {
        for (int i = 0; i < payload.length(); i++) {
          unsigned int bits = letter_to_bits(payload[i]);
          byte r1 = bits & 0xFF;
          byte r2 = (bits >> 8) & 0xFF;
          byte r3 = (bits >> 16) & 0xFF;
          byte r4 = (bits >> 24) & 0xFF;
          write_bits(r1, r2, r3, r4);
          Serial.print(payload[i]);
          Serial.print(" ");
          Serial.println(bits);
          delay(DISPLAY_TIME);

          write_bits(0, 0, 0, 0);
          delay(PAUSE_TIME);
        }
      }
    } else {
      http.end();
    }
  } else {
    Serial.println("not connected");
  }
  delay(3000);
}

unsigned int letter_to_bits(char letter) {
  char lower = tolower(letter);
  unsigned int result = 0;
  if (lower >= 'a' &&  lower <= 'z') {
    // the wiring is messed up so this is kinda nasty
    if (lower == 'c') {
      result = 1 << 26;
    } else if (lower == 'g') {
      result = 1 << 7;
    } else if (lower == 'h') {
      result = 1 << 6;
    } else if (lower == 'i') {
      result = 1 << 8;
    } else if (lower == 'd') {
      result = 1 << 2;
    } else if (lower == 'e') {
      result = 1 << 3;
    } else if (lower == 'f') {
      result = 1 << 4;
    } else if (lower == 'a' || lower == 'b') {
      result = 1 << (lower - 'a');
    } else {
      result = 1 << (lower - 'a');
    }
  }
  return result;
}

void write_bits(byte r1, byte r2, byte r3, byte r4) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, r4);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, r3);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, r2);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, r1);
  digitalWrite(LATCH_PIN, HIGH);
}
