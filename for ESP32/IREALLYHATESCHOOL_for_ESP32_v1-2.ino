// IWANTTOGOHOME INC.
// IREALLYHATESCHOOL for ESP32

// IREALLYHATESCHOOL for ESP32 v1.2
// IRHS100

// Library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// Pin
const int dit = 2;
const int dah = 3;
const int backspace = 4;
const int send = 5;
const int shift = 6;
const int space = 7;

// Display
const int display_text_size = 1;
const int display_width = 128;
const int display_height = 64;
const int display_reset = -1;
const int display_max_lines = 10;
const int display_visible_lines = 8;
const int current_size = 21;

String display_lines[display_max_lines];
int display_index = 0;

Adafruit_SSD1306 display(display_width, display_height, &Wire, display_reset);

// Morse Code
const char* morseTable[] = {
  // A–Z
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--..",

  // 0–9
  "-----", ".----", "..---", "...--", "....-", ".....",
  "-....", "--...", "---..", "----.",

  // Special characters
  ".-.-.-",   // . (period)
  "--..--",   // , (comma)
  "..--..",   // ? (question mark)
  ".----.",   // ' (apostrophe)
  "-.-.--",   // ! (exclamation mark)
  "-..-.",    // / (slash)
  "-.--.",    // ( (left parenthesis)
  "-.--.-",   // ) (right parenthesis)
  ".-...",    // & (ampersand)
  "---...",   // : (colon)
  "-.-.-.",   // ; (semicolon)
  "-...-",    // = (equals)
  ".-.-.",    // + (plus)
  "-....-",   // - (hyphen)
  "..--.-",   // _ (underscore)
  ".-..-.",   // " (quotation mark)
  "...-..-",  // $ (dollar)
  ".--.-."    // @ (at symbol)
};
const char character[] = "abcdefghijklmnopqrstuvwxyz0123456789.,?\'!/()&:;=+-_\"$@";
char morse_code[16];
String message;

// WiFi
WiFiClient client;
String ssid;
String password;
const char* host;
uint16_t port;

// System
String choose;

////////////////// Custom //////////////////
char* savedhost = "example.com";
uint16_t savedport = 888;
////////////////////////////////////////////

// Internet Connection Status
void status(void *parameter) {
  while (true) {
    wl_status_t wifistatus = WiFi.status();
    if (wifistatus == WL_CONNECTED) {
      if (client.connected()) {
        led("green");
      }
      else {
        led("blue");
      }
    }
    else if (wifistatus == WL_IDLE_STATUS) {
      led("yellow");
    }
    else {
      led("red");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// LED
void led(String color) {
  if (color == "white") {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
  }
  else if (color == "red") {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
  }
  else if (color == "green") {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
  }
  else if (color == "blue") {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
  }
  else if (color == "off") {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
  }
  else if (color == "yellow") {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
  }
  else if (color == "input") {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (color == "inputOff") {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

// OMG
void error() {
  while (true) {
      led("red");
      delay(1000);
      led("off");
      delay(1000);
    }
}

// Display and Serial Output
void log(String log) { // GPT IS GOD
  int start = 0;

  while (start < log.length()) {
    int end = start + current_size;

    int newlineIndex = log.indexOf('\n', start);
    if (newlineIndex != -1 && newlineIndex < end) {
      String part = log.substring(start, newlineIndex);

      if (display_index >= display_max_lines) {
        for (int i = 0; i < display_max_lines - 1; i++) {
          display_lines[i] = display_lines[i + 1];
        }
        display_lines[display_max_lines - 1] = part;
      } else {
        display_lines[display_index++] = part;
      }

      start = newlineIndex + 1;
      continue;
    }

    int safeEnd = (int)log.length();
    String part = log.substring(start, (end < safeEnd) ? end : safeEnd);

    if (display_index >= display_max_lines) {
      for (int i = 0; i < display_max_lines - 1; i++) {
        display_lines[i] = display_lines[i + 1];
      }
      display_lines[display_max_lines - 1] = part;
    } else {
      display_lines[display_index++] = part;
    }

    start = end;
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  int display_start_line = max(0, display_index - display_visible_lines);
  for (int i = display_start_line; i < display_index; i++) {
    display.println(display_lines[i]);
  }

  display.display();
  Serial.println(log);
}

// Input
String input() {
  morse_code[0] = '\0';
  message = "";
  while (true) {
    while (true) {
      if (digitalRead(dit) == LOW) {
        strcat(morse_code, ".");
        led("input");
        delay(200);
        led("inputOff");
        while (digitalRead(dit) == LOW);
      }
      else if (digitalRead(dah) == LOW) {
        strcat(morse_code, "-");
        led("input");
        delay(200);
        led("inputOff");
        while (digitalRead(dah) == LOW);
      }
      else if (digitalRead(send) == LOW) {
        led("input");
        delay(200);
        led("inputOff");
        while (digitalRead(send) == LOW);
        break;
      }
      else if (digitalRead(backspace) == LOW) {
        led("input");
        delay(200);
        led("inputOff");
        while (digitalRead(backspace) == LOW);
        morse_code[0] = '\0';
        message.remove(message.length() - 1);
        log("> " + message);
      }
      else if (digitalRead(space) == LOW) {
        led("input");
        delay(200);
        led("inputOff");
        while (digitalRead(space) == LOW);
        morse_code[0] = '\0';
        message += " ";
        log("> " + message);
      }
    }
    if (morse_code[0] == '\0') {
      led("input");
      log("\n");
      delay(200);
      led("inputOff");
      delay(50);
      led("input");
      delay(200);
      led("inputOff");
      return message;
    }
    for (int i = 0; i < 54; i++) {
      if (strcmp(morse_code, morseTable[i]) == 0) {
        char c = character[i];

        if (digitalRead(shift) == LOW && character[i] >= 'a' && character[i] <= 'z') {
          c -= 32;
        }

        message += String(c);
        break;
      }
    }
    log("> " + message);
    morse_code[0] = '\0';
  }
}

// Receive
void receive(void *parameter) {
  while (true) {
    if (client.connected() && client.available()) {
      log(client.readStringUntil('\n'));
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // pinMode
  Serial.begin(9600);
  pinMode(dit, INPUT_PULLUP);
  pinMode(dah, INPUT_PULLUP);
  pinMode(backspace, INPUT_PULLUP);
  pinMode(send, INPUT_PULLUP);
  pinMode(shift, INPUT_PULLUP);
  pinMode(space, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Display Setting
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display initialization failed.");
    error();
  }
  display.clearDisplay();
  display.setTextSize(display_text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();

  // Thread Setting
  xTaskCreatePinnedToCore(receive, "receive", 4096, NULL, 0, NULL, 0);
  xTaskCreatePinnedToCore(status, "status", 4096, NULL, 1, NULL, 1);

  log("IWTGH INC.");
  log("IREALLYHATESCHOOL");
  log("\n");

  log("[Press Send Button]");
  input();

  ///////////////////////////////////////////////////////////////////

  // WiFi Connection
  log("Connect to the WiFi network.");
  log("\n");
  while (true) {
    int networksFound = WiFi.scanNetworks();

    if (networksFound == 0) {
      log("WiFi network not found.");
      log("[Send]: Research");
      input();
    }
    else {
      log("Found " + String(networksFound) + " WiFi networks.");
      
      for (int i = 0; i < networksFound; ++i) {
        log(WiFi.SSID(i));
        delay(500);
      }
      log("\n");

      break;
    }
  }
  
  while (true) {
    log("Enter SSID.");
    ssid = input();
    log("Enter WiFi Password.");
    password = input();
    
    log("==WiFi Info==");
    log("SSID: " + ssid);
    log("PW: " + password);
    log("[Send]: Connect");
    log("[R]: Re-input");
    choose = input();
    if (choose == "R") {
      ssid = "";
      password = "";
      choose = "";
    }
    else {
      choose = "";
      int status = WiFi.begin(ssid.c_str(), password.c_str());
      log("Connecting...");
      break;
    }
  }

  while (WiFi.status() != WL_CONNECTED);
  ssid = ""; // SAVE MEMORY +_+
  password = "";
  log("Connected to WiFi network.");
  log("\n");

  // Connecting to a Server
  while (true) {
    while (true) {
      log("Select how to connect to the server.");
      log("[Send]: Saved Server");
      log("[C]: Custom Server");
      
      choose = input();
      if (choose == "C") {
        log("Enter the address of the server to connect.");
        String inputhost = input();
        log("Enter the port of the server to connect.");
        String inputport = input();
        host = inputhost.c_str();
        port = (uint16_t)inputport.toInt();
        inputport = "";
      }
      else {
        host = savedhost;
        port = savedport;
      }
      choose = "";
      
      log("==Server Info==");
      log("Addr: " + String(host));
      log("Port: " + String(port));
      log("[Send]: Connect");
      log("[R]: Re-input");
      choose = input();
      if (choose == "R") {
        choose = "";
      }
      else {
        choose = "";
        break;
      }
    }

    if (!client.connect(host, port)) {
      log("Unable to connect to server.");
      log("\n");
      delay(1000);
    }
    else {
      log("Connected to server.");
      break;
    }
  }
}

void loop() {
  // Message send
  if (client.connected()) {
    client.print(input());
  }
}
