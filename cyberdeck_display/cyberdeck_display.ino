#include <SPI.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define HRES 320
#define VRES 240

const char* ssid     = "CYD_AI_Mesh";
const char* password = "ArduinoUnoQ123";
const char* serverIP = "10.42.0.1";

const uint16_t NOTE_DURATION = 200;

#define DOUT 39
#define DIN  32
#define DCS  33
#define DCLK 25

// ============================================================
// PIANO NOTES
// ============================================================

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523


TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);


// ============================================================
// SCREENS
// ============================================================

enum Screen {
  SCREEN_HOME,
  SCREEN_AI,
  SCREEN_SYSTEM,
  SCREEN_SENSORS,
  SCREEN_GPIO,
  SCREEN_BUZZER,
  SCREEN_NETWORK,
  SCREEN_TERMINAL
};

Screen currentScreen = SCREEN_HOME;


// ============================================================
// GLOBAL VARIABLES
// ============================================================

String inputText = "";
String outputText = "";
String statusText = "Connecting...";

bool wifiConnected = false;


// ============================================================
// BUTTON SYSTEM
// ============================================================

struct Button {
  String label;
  int x;
  int y;
  int w;
  int h;
};

#define MAX_BUTTONS 40

Button buttons[MAX_BUTTONS];
int buttonCount = 0;


// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

void drawScreen();

void drawHome();
void drawAI();
void drawSystem();
void drawSensors();
void drawGPIO();
void drawBuzzer();
void drawNetwork();
void drawTerminal();

void handleTouch();

String sendCommand(String endpoint, String json);
String jsonValue(String json, String key);

void drawButton(
  String label,
  int x,
  int y,
  int w,
  int h,
  uint16_t bg = TFT_BLUE
);

void addButton(
  String label,
  int x,
  int y,
  int w,
  int h
);

// Hitbox without drawing a visible button
void addHitbox(
  String label,
  int x,
  int y,
  int w,
  int h
);

void drawWrappedText(
  String text,
  int x,
  int y,
  int width,
  int maxLines,
  int textSize
);

void sendAI();
void sendTerminal();

void playNote(int frequency);

void goHome();


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  touch.setRotation(1);

  touch.setCal(
    400,
    3400,
    250,
    3600,
    320,
    240,
    1
  );


  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.drawString(
    "DREAMDECK",
    90,
    100
  );


  // ==========================================================
  // WIFI
  // ==========================================================

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    ssid,
    password
  );

  int attempts = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < 30
  ) {

    delay(500);

    attempts++;

    tft.fillRect(
      0,
      130,
      320,
      30,
      TFT_BLACK
    );

    String dots = "";

    for (
      int i = 0;
      i < (attempts % 4);
      i++
    ) {
      dots += ".";
    }

    tft.drawString(
      "Connecting" + dots,
      90,
      130
    );
  }


  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    wifiConnected = true;

    Serial.println(
      "Connected to WiFi"
    );

    Serial.print(
      "CYD IP: "
    );

    Serial.println(
      WiFi.localIP()
    );

    statusText =
      "UNO Q ONLINE";

  } else {

    wifiConnected = false;

    statusText =
      "WIFI ERROR";
  }


  drawScreen();
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    wifiConnected = false;

  } else {

    wifiConnected = true;
  }


  handleTouch();

  delay(50);
}


// ============================================================
// SCREEN ROUTER
// ============================================================

void drawScreen() {

  tft.fillScreen(
    TFT_BLACK
  );

  buttonCount = 0;


  switch (
    currentScreen
  ) {

    case SCREEN_HOME:
      drawHome();
      break;

    case SCREEN_AI:
      drawAI();
      break;

    case SCREEN_SYSTEM:
      drawSystem();
      break;

    case SCREEN_SENSORS:
      drawSensors();
      break;

    case SCREEN_GPIO:
      drawGPIO();
      break;

    case SCREEN_BUZZER:
      drawBuzzer();
      break;

    case SCREEN_NETWORK:
      drawNetwork();
      break;

    case SCREEN_TERMINAL:
      drawTerminal();
      break;
  }
}


// ============================================================
// HOME
// ============================================================

void drawHome() {

  tft.setTextColor(
    TFT_WHITE
  );

  tft.setTextSize(2);

  tft.drawString(
    "DREAMDECK",
    110,
    8
  );


  tft.setTextSize(1);

  tft.drawString(
    "PORTABLE AI CYBERDECK",
    101,
    31
  );


  tft.drawFastHLine(
    10,
    45,
    300,
    TFT_WHITE
  );


  addButton(
    "AI",
    15,
    60,
    135,
    40
  );

  addButton(
    "SYSTEM",
    170,
    60,
    135,
    40
  );

  addButton(
    "SENSORS",
    15,
    110,
    135,
    40
  );

  addButton(
    "GPIO",
    170,
    110,
    135,
    40
  );

  addButton(
    "NETWORK",
    15,
    160,
    135,
    40
  );

  addButton(
    "TERMINAL",
    170,
    160,
    135,
    40
  );


  tft.setTextSize(1);

  tft.setTextColor(
    wifiConnected
      ? TFT_GREEN
      : TFT_RED
  );

  tft.drawString(
    wifiConnected
      ? "● UNO Q ONLINE"
      : "● UNO Q OFFLINE",
    15,
    220
  );


  tft.setTextColor(
    TFT_WHITE
  );

  tft.drawString(
    "DREAMDECK v0.1",
    215,
    220
  );
}


// ============================================================
// AI
// ============================================================

void drawAI() {

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.drawString(
    "AI",
    10,
    7
  );

  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );

  tft.drawFastHLine(
    0,
    35,
    320,
    TFT_WHITE
  );


  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);

  tft.drawString(
    "YOU:",
    5,
    42
  );

  drawWrappedText(
    inputText,
    5,
    53,
    310,
    2,
    1
  );


  tft.setTextColor(TFT_WHITE);

  tft.drawString(
    "UNO Q:",
    5,
    72
  );

  drawWrappedText(
    outputText,
    5,
    83,
    310,
    3,
    1
  );

 // ==========================================================
  // MORSE CHARACTERS
  // ==========================================================

  addButton(
    ".",
    225,
    60,
    28,
    20
  );

  addButton(
    "-",
    254,
    60,
    28,
    20
  );

  addButton(
    "/",
    283,
    60,
    28,
    20
  );

  // ==========================================================
  // KEYBOARD
  // ==========================================================

  String keys1[] = {
    "Q","W","E","R","T",
    "Y","U","I","O","P"
  };

  String keys2[] = {
    "A","S","D","F","G",
    "H","J","K","L"
  };

  String keys3[] = {
    "Z","X","C","V",
    "B","N","M"
  };


  int x = 4;

  for (int i = 0; i < 10; i++) {

    addButton(
      keys1[i],
      x,
      120,
      28,
      20
    );

    x += 31;
  }


  x = 18;

  for (int i = 0; i < 9; i++) {

    addButton(
      keys2[i],
      x,
      143,
      28,
      20
    );

    x += 31;
  }


  x = 45;

  for (int i = 0; i < 7; i++) {

    addButton(
      keys3[i],
      x,
      166,
      28,
      20
    );

    x += 31;
  }


  addButton(
    "DEL",
    270,
    166,
    40,
    20
  );


  // ==========================================================
  // COMMAND ROW
  // ==========================================================

  addButton(
    "SPACE",
    5,
    194,
    95,
    30
  );

  addButton(
    "SEND",
    105,
    194,
    95,
    30
  );

  addButton(
    "CLR",
    205,
    194,
    50,
    30
  );

  addButton(
    "<",
    260,
    194,
    55,
    30
  );
}


// ============================================================
// SYSTEM
// ============================================================

void drawSystem() {

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.drawString(
    "SYSTEM",
    10,
    7
  );


  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  String response =
    sendCommand(
      "system",
      "{\"action\":\"status\"}"
    );


  String cpu =
    jsonValue(
      response,
      "cpu"
    );

  String ram =
    jsonValue(
      response,
      "ram"
    );

  String uptime =
    jsonValue(
      response,
      "uptime"
    );

  String ai =
    jsonValue(
      response,
      "ai"
    );


  tft.setTextSize(1);

  tft.drawString(
    "ARDUINO UNO Q",
    10,
    50
  );

  tft.drawString(
    "CPU:",
    10,
    75
  );

  tft.drawString(
    cpu + "%",
    100,
    75
  );

  tft.drawString(
    "RAM:",
    10,
    95
  );

  tft.drawString(
    ram + "%",
    100,
    95
  );

  tft.drawString(
    "UPTIME:",
    10,
    115
  );

  tft.drawString(
    uptime,
    100,
    115
  );

  tft.drawString(
    "AI:",
    10,
    135
  );


  tft.setTextColor(
    ai == "true"
      ? TFT_GREEN
      : TFT_RED
  );

  tft.drawString(
    ai == "true"
      ? "ONLINE"
      : "OFFLINE",
    100,
    135
  );


  tft.setTextColor(
    TFT_WHITE
  );


  addButton(
    "REFRESH",
    90,
    170,
    140,
    35
  );

  addButton(
    "SHUTDOWN",
    90,
    210,
    140,
    25
  );
}


// ============================================================
// SENSORS
// ============================================================

void drawSensors() {

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.drawString(
    "SENSORS",
    10,
    7
  );


  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  String response =
    sendCommand(
      "sensors",
      "{\"action\":\"read\"}"
    );


  String temp =
    jsonValue(
      response,
      "temperature"
    );

  String humidity =
    jsonValue(
      response,
      "humidity"
    );


  tft.setTextSize(1);

  tft.drawString(
    "DHT11 (connect sensor to A0 to see data)",
    10,
    55
  );

  tft.drawString(
    "TEMPERATURE",
    10,
    85
  );


  tft.setTextSize(2);

  tft.drawString(
    temp + " C",
    160,
    80
  );


  tft.setTextSize(1);

  tft.drawString(
    "HUMIDITY",
    10,
    120
  );


  tft.setTextSize(2);

  tft.drawString(
    humidity + " %",
    160,
    115
  );


  tft.setTextSize(1);


  addButton(
    "REFRESH",
    90,
    170,
    140,
    35
  );
}


// ============================================================
// GPIO
// ============================================================

void drawGPIO() {

  tft.setTextColor(
    TFT_WHITE
  );

  tft.setTextSize(2);

  tft.drawString(
    "GPIO",
    10,
    7
  );


  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  tft.setTextSize(1);

  tft.drawString(
    "UNO Q BUILT-IN LED",
    10,
    55
  );


  addButton(
    "LED ON",
    20,
    75,
    120,
    40
  );

  addButton(
    "LED OFF",
    180,
    75,
    120,
    40
  );


  tft.drawString(
    "BUZZER / PIANO",
    10,
    130
  );


  addButton(
    "OPEN PIANO",
    20,
    145,
    280,
    45
  );


  tft.drawString(
    "Connect a buzzer to D3 and GND on the UNO Q to play",
    10,
    205
  );
}


// ============================================================
// BUZZER / PIANO
// ============================================================

void drawBuzzer() {

  tft.fillScreen(
    TFT_BLACK
  );


  tft.setTextColor(
    TFT_WHITE
  );

  tft.setTextSize(2);

  tft.drawString(
    "PIANO",
    10,
    7
  );


  // HOME
  addButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  // ==========================================================
  // PIANO DIMENSIONS
  // ==========================================================

  int startX = 8;
  int startY = 65;

  int whiteW = 38;
  int whiteH = 145;

  int blackW = 25;
  int blackH = 85;


  // ==========================================================
  // DRAW WHITE KEYS
  // ==========================================================

  const char* whiteNotes[] = {
    "C",
    "D",
    "E",
    "F",
    "G",
    "A",
    "B",
    "C"
  };


  for (
    int i = 0;
    i < 8;
    i++
  ) {

    int x =
      startX +
      i * whiteW;


    // White key
    tft.fillRect(
      x,
      startY,
      whiteW - 2,
      whiteH,
      TFT_WHITE
    );


    tft.drawRect(
      x,
      startY,
      whiteW - 2,
      whiteH,
      TFT_DARKGREY
    );


    // Note label
    tft.setTextColor(
      TFT_BLACK
    );

    tft.setTextSize(1);

    tft.setTextDatum(
      MC_DATUM
    );

    tft.drawString(
      whiteNotes[i],
      x + (whiteW / 2),
      startY + whiteH - 15
    );
  }


  // ==========================================================
  // DRAW BLACK KEYS
  // ==========================================================

  int blackPositions[] = {
    35,
    73,
    149,
    187,
    225
  };


  const char* blackNotes[] = {
    "C#",
    "D#",
    "F#",
    "G#",
    "A#"
  };


  for (
    int i = 0;
    i < 5;
    i++
  ) {

    int x =
      blackPositions[i];


    tft.fillRect(
      x,
      startY,
      blackW,
      blackH,
      TFT_BLACK
    );


    tft.drawRect(
      x,
      startY,
      blackW,
      blackH,
      TFT_DARKGREY
    );


    tft.setTextColor(
      TFT_WHITE
    );

    tft.setTextSize(1);

    tft.setTextDatum(
      MC_DATUM
    );

    tft.drawString(
      blackNotes[i],
      x + blackW / 2,
      startY + 12
    );
  }


  tft.setTextDatum(
    TL_DATUM
  );


  // ==========================================================
  // TOUCH HITBOXES
  // ==========================================================
  //
  // BLACK KEYS ARE ADDED FIRST so they get priority.
  //
  // These are invisible hitboxes; the piano graphics above
  // remain visible.
  // ==========================================================

  // Black keys

  addHitbox(
    "C#",
    35,
    startY,
    blackW,
    blackH
  );

  addHitbox(
    "D#",
    73,
    startY,
    blackW,
    blackH
  );

  addHitbox(
    "F#",
    149,
    startY,
    blackW,
    blackH
  );

  addHitbox(
    "G#",
    187,
    startY,
    blackW,
    blackH
  );

  addHitbox(
    "A#",
    225,
    startY,
    blackW,
    blackH
  );


  // White keys

  addHitbox(
    "C4",
    startX + 0 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "D",
    startX + 1 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "E",
    startX + 2 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "F",
    startX + 3 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "G",
    startX + 4 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "A",
    startX + 5 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "B",
    startX + 6 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );

  addHitbox(
    "C5",
    startX + 7 * whiteW,
    startY,
    whiteW - 2,
    whiteH
  );
}


// ============================================================
// NETWORK
// ============================================================

void drawNetwork() {

  tft.setTextColor(
    TFT_WHITE
  );

  tft.setTextSize(2);

  tft.drawString(
    "NETWORK",
    10,
    7
  );


  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  tft.setTextSize(1);


  tft.drawString(
    "CYD STATUS",
    10,
    50
  );

  tft.drawString(
    wifiConnected
      ? "CONNECTED"
      : "DISCONNECTED",
    130,
    50
  );


  tft.drawString(
    "CYD IP",
    10,
    75
  );

  tft.drawString(
    WiFi.localIP().toString(),
    130,
    75
  );


  String response =
    sendCommand(
      "network",
      "{\"action\":\"status\"}"
    );


  String unoIP =
    jsonValue(
      response,
      "ip"
    );


  tft.drawString(
    "UNO Q IP",
    10,
    100
  );

  tft.drawString(
    unoIP,
    130,
    100
  );


  tft.drawString(
    "SERVER",
    10,
    125
  );

  tft.drawString(
    "PORT 7000",
    130,
    125
  );


  addButton(
    "REFRESH",
    90,
    170,
    140,
    35
  );
}


// ============================================================
// TERMINAL
// ============================================================

void drawTerminal() {

  tft.setTextColor(
    TFT_WHITE
  );

  tft.setTextSize(2);

  tft.drawString(
    "TERMINAL",
    10,
    7
  );


  drawButton(
    "< HOME",
    225,
    3,
    85,
    30
  );


  tft.drawFastHLine(
    0,
    35,
    320,
    TFT_WHITE
  );


  tft.setTextSize(1);


  // Current command

  tft.setTextColor(
    TFT_GREEN
  );

  tft.drawString(
    ">",
    5,
    43
  );


  drawWrappedText(
    inputText,
    15,
    43,
    300,
    2,
    1
  );


  // Output

  tft.setTextColor(
    TFT_WHITE
  );

  drawWrappedText(
    outputText,
    5,
    63,
    310,
    5,
    1
  );


  // Keyboard

  String keys1[] = {
    "Q","W","E","R","T",
    "Y","U","I","O","P"
  };

  String keys2[] = {
    "A","S","D","F","G",
    "H","J","K","L"
  };

  String keys3[] = {
    "Z","X","C","V",
    "B","N","M"
  };


  int x = 4;

  for (
    int i = 0;
    i < 10;
    i++
  ) {

    addButton(
      keys1[i],
      x,
      125,
      28,
      20
    );

    x += 31;
  }


  x = 18;

  for (
    int i = 0;
    i < 9;
    i++
  ) {

    addButton(
      keys2[i],
      x,
      148,
      28,
      20
    );

    x += 31;
  }


  x = 45;

  for (
    int i = 0;
    i < 7;
    i++
  ) {

    addButton(
      keys3[i],
      x,
      171,
      28,
      20
    );

    x += 31;
  }


  addButton(
    "DEL",
    270,
    171,
    40,
    20
  );


  // Command row

  addButton(
    "SPACE",
    5,
    198,
    85,
    30
  );

  addButton(
    "RUN",
    95,
    198,
    65,
    30
  );

  addButton(
    "CLR",
    165,
    198,
    55,
    30
  );

  addButton(
    "<",
    225,
    198,
    40,
    30
  );

  addButton(
    "/",
    270,
    198,
    20,
    30
  );

  addButton(
    "-",
    295,
    198,
    20,
    30
  );
}


// ============================================================
// TOUCH HANDLING
// ============================================================

void handleTouch() {

  if (!touch.Pressed())
    return;


  int x = touch.X();
  int y = touch.Y();


  Serial.print(
    "TOUCH X="
  );

  Serial.print(
    x
  );

  Serial.print(
    " Y="
  );

  Serial.println(
    y
  );


  // ==========================================================
  // GLOBAL HOME
  // ==========================================================

  if (
    currentScreen != SCREEN_HOME &&
    x >= 205 &&
    x <= 319 &&
    y >= 0 &&
    y <= 45
  ) {

    Serial.println(
      "GLOBAL HOME"
    );

    goHome();

    delay(300);

    return;
  }


  // ==========================================================
  // BUTTON SEARCH
  // ==========================================================

  for (
    int i = 0;
    i < buttonCount;
    i++
  ) {

    Button &b =
      buttons[i];


    if (
      x >= b.x &&
      x <= b.x + b.w &&
      y >= b.y &&
      y <= b.y + b.h
    ) {

      String label =
        b.label;


      Serial.print(
        "BUTTON: "
      );

      Serial.println(
        label
      );


      // ======================================================
      // HOME
      // ======================================================

      if (
        currentScreen ==
        SCREEN_HOME
      ) {

        if (
          label == "AI"
        ) {

          currentScreen =
            SCREEN_AI;

          inputText = "";
          outputText = "";

          drawScreen();
        }

        else if (
          label == "SYSTEM"
        ) {

          currentScreen =
            SCREEN_SYSTEM;

          drawScreen();
        }

        else if (
          label == "SENSORS"
        ) {

          currentScreen =
            SCREEN_SENSORS;

          drawScreen();
        }

        else if (
          label == "GPIO"
        ) {

          currentScreen =
            SCREEN_GPIO;

          drawScreen();
        }

        else if (
          label == "NETWORK"
        ) {

          currentScreen =
            SCREEN_NETWORK;

          drawScreen();
        }

        else if (
          label == "TERMINAL"
        ) {

          currentScreen =
            SCREEN_TERMINAL;

          inputText = "";
          outputText = "";

          drawScreen();
        }
      }


      // ======================================================
      // AI
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_AI
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "SEND"
        ) {

          sendAI();
        }

        else if (
          label == "DEL"
        ) {

          if (
            inputText.length() > 0
          ) {

            inputText.remove(
              inputText.length() - 1
            );
          }

          drawScreen();
        }

        else if (
          label == "SPACE"
        ) {

          inputText += " ";

          drawScreen();
        }

        else if (
          label == "CLR"
        ) {

          inputText = "";

          drawScreen();
        }

        else if (
          label == "<"
        ) {

          goHome();
        }

        else {

          inputText += label;

          drawScreen();
        }
      }


      // ======================================================
      // SYSTEM
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_SYSTEM
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "REFRESH"
        ) {

          drawScreen();
        }

        else if (
          label == "SHUTDOWN"
        ) {

          sendCommand(
            "shutdown",
            "{}"
          );

          outputText =
            "UNO Q shutting down...";

          drawScreen();
        }
      }


      // ======================================================
      // SENSORS
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_SENSORS
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "REFRESH"
        ) {

          drawScreen();
        }
      }


      // ======================================================
      // GPIO
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_GPIO
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "LED ON"
        ) {

          sendCommand(
            "gpio",
            "{\"action\":\"led\",\"state\":true}"
          );
        }

        else if (
          label == "LED OFF"
        ) {

          sendCommand(
            "gpio",
            "{\"action\":\"led\",\"state\":false}"
          );
        }

        else if (
          label == "OPEN PIANO"
        ) {

          currentScreen =
            SCREEN_BUZZER;

          drawScreen();
        }
      }


      // ======================================================
      // BUZZER / PIANO
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_BUZZER
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "C4"
        ) {

          playNote(
            NOTE_C4
          );
        }

        else if (
          label == "C#"
        ) {

          playNote(
            NOTE_CS4
          );
        }

        else if (
          label == "D"
        ) {

          playNote(
            NOTE_D4
          );
        }

        else if (
          label == "D#"
        ) {

          playNote(
            NOTE_DS4
          );
        }

        else if (
          label == "E"
        ) {

          playNote(
            NOTE_E4
          );
        }

        else if (
          label == "F"
        ) {

          playNote(
            NOTE_F4
          );
        }

        else if (
          label == "F#"
        ) {

          playNote(
            NOTE_FS4
          );
        }

        else if (
          label == "G"
        ) {

          playNote(
            NOTE_G4
          );
        }

        else if (
          label == "G#"
        ) {

          playNote(
            NOTE_GS4
          );
        }

        else if (
          label == "A"
        ) {

          playNote(
            NOTE_A4
          );
        }

        else if (
          label == "A#"
        ) {

          playNote(
            NOTE_AS4
          );
        }

        else if (
          label == "B"
        ) {

          playNote(
            NOTE_B4
          );
        }

        else if (
          label == "C5"
        ) {

          playNote(
            NOTE_C5
          );
        }
      }


      // ======================================================
      // NETWORK
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_NETWORK
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "REFRESH"
        ) {

          drawScreen();
        }
      }


      // ======================================================
      // TERMINAL
      // ======================================================

      else if (
        currentScreen ==
        SCREEN_TERMINAL
      ) {

        if (
          label == "< HOME"
        ) {

          goHome();
        }

        else if (
          label == "RUN"
        ) {

          sendTerminal();
        }

        else if (
          label == "DEL"
        ) {

          if (
            inputText.length() > 0
          ) {

            inputText.remove(
              inputText.length() - 1
            );
          }

          drawScreen();
        }

        else if (
          label == "SPACE"
        ) {

          inputText += " ";

          drawScreen();
        }

        else if (
          label == "CLR"
        ) {

          inputText = "";
          outputText = "";

          drawScreen();
        }

        else if (
          label == "<"
        ) {

          goHome();
        }

        else {

          inputText += label;

          drawScreen();
        }
      }


      delay(250);

      return;
    }
  }
}


// ============================================================
// AI REQUEST
// ============================================================

void sendAI() {

  if (
    inputText.length() == 0
  )
    return;


  outputText =
    "Thinking...";

  drawScreen();


  String json =
    "{\"prompt\":\"" +
    inputText +
    "\"}";


  String response =
    sendCommand(
      "message",
      json
    );


  String result =
    jsonValue(
      response,
      "text"
    );


  if (
    result.length() == 0
  )
    result = response;


  outputText =
    result;

  inputText = "";


  drawScreen();
}


// ============================================================
// TERMINAL REQUEST
// ============================================================

void sendTerminal() {

  if (
    inputText.length() == 0
  )
    return;


  outputText =
    "Running...";

  drawScreen();


  String json =
    "{\"command\":\"" +
    inputText +
    "\"}";


  String response =
    sendCommand(
      "terminal",
      json
    );


  String result =
    jsonValue(
      response,
      "output"
    );


  if (
    result.length() == 0
  )
    result = response;


  outputText =
    result;

  inputText = "";


  drawScreen();
}


// ============================================================
// PLAY NOTE
// ============================================================

void playNote(int frequency) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("BUZZER: WIFI OFF");
    return;
  }

  String body =
    "{\"frequency\":" +
    String(frequency) +
    ",\"duration\":" +
    String(NOTE_DURATION) +
    "}";

  WiFiClient client;

  Serial.print("BUZZER SEND: ");
  Serial.print(frequency);
  Serial.println(" Hz");

  if (!client.connect(serverIP, 7000)) {

    Serial.println("BUZZER: CONNECTION FAILED");
    return;
  }

  client.print(
    "POST /api/buzzer HTTP/1.1\r\n"
  );

  client.print(
    "Host: "
  );
  client.print(serverIP);
  client.print(
    ":7000\r\n"
  );

  client.print(
    "Content-Type: application/json\r\n"
  );

  client.print(
    "Content-Length: "
  );
  client.print(body.length());
  client.print(
    "\r\n"
  );

  client.print(
    "Connection: close\r\n"
  );

  client.print(
    "\r\n"
  );

  client.print(body);

  // We deliberately DO NOT wait for the HTTP response.
  client.stop();

  Serial.println("BUZZER: SENT");
}


// ============================================================
// HOME
// ============================================================

void goHome() {

  currentScreen =
    SCREEN_HOME;

  inputText = "";
  outputText = "";

  drawScreen();
}


// ============================================================
// HTTP
// ============================================================

String sendCommand(
  String endpoint,
  String json
) {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    return
      "{\"error\":\"WiFi disconnected\"}";
  }


  HTTPClient http;


  String url =
    "http://" +
    String(serverIP) +
    ":7000/api/" +
    endpoint;


  Serial.print(
    "HTTP POST: "
  );

  Serial.println(
    url
  );


  http.begin(
    url
  );

  http.setTimeout(
    50000
  );

  http.addHeader(
    "Content-Type",
    "application/json"
  );


  int code =
    http.POST(
      json
    );


  if (
    code <= 0
  ) {

    Serial.print(
      "HTTP ERROR: "
    );

    Serial.println(
      code
    );

    http.end();

    return
      "{\"error\":\"HTTP request failed\"}";
  }


  String response =
    http.getString();


  Serial.print(
    "RESPONSE: "
  );

  Serial.println(
    response
  );


  http.end();


  return response;
}


// ============================================================
// JSON
// ============================================================

String jsonValue(
  String json,
  String key
) {

  String search =
    "\"" +
    key +
    "\":";


  int start =
    json.indexOf(
      search
    );


  if (
    start == -1
  )
    return "";


  start +=
    search.length();


  while (
    start < json.length() &&
    json[start] == ' '
  ) {

    start++;
  }


  bool quoted =
    start < json.length() &&
    json[start] == '"';


  if (quoted) {

    start++;


    int end =
      json.indexOf(
        "\"",
        start
      );


    if (
      end == -1
    )
      return "";


    return json.substring(
      start,
      end
    );
  }


  int end = start;


  while (
    end < json.length() &&
    json[end] != ',' &&
    json[end] != '}'
  ) {

    end++;
  }


  return json.substring(
    start,
    end
  );
}


// ============================================================
// BUTTON SYSTEM
// ============================================================

void addButton(
  String label,
  int x,
  int y,
  int w,
  int h
) {

  if (
    buttonCount >=
    MAX_BUTTONS
  )
    return;


  buttons[
    buttonCount
  ] = {
    label,
    x,
    y,
    w,
    h
  };


  buttonCount++;


  drawButton(
    label,
    x,
    y,
    w,
    h,
    TFT_BLUE
  );
}


// ============================================================
// INVISIBLE HITBOX
// ============================================================

void addHitbox(
  String label,
  int x,
  int y,
  int w,
  int h
) {

  if (
    buttonCount >=
    MAX_BUTTONS
  )
    return;


  buttons[
    buttonCount
  ] = {
    label,
    x,
    y,
    w,
    h
  };


  buttonCount++;
}


// ============================================================
// DRAW BUTTON
// ============================================================

void drawButton(
  String label,
  int x,
  int y,
  int w,
  int h,
  uint16_t bg
) {

  tft.fillRoundRect(
    x,
    y,
    w,
    h,
    5,
    bg
  );


  tft.drawRoundRect(
    x,
    y,
    w,
    h,
    5,
    TFT_WHITE
  );


  tft.setTextColor(
    TFT_WHITE,
    bg
  );


  tft.setTextDatum(
    MC_DATUM
  );


  tft.drawString(
    label,
    x + w / 2,
    y + h / 2
  );


  tft.setTextDatum(
    TL_DATUM
  );
}


// ============================================================
// TEXT WRAPPING
// ============================================================

void drawWrappedText(
  String text,
  int x,
  int y,
  int width,
  int maxLines,
  int textSize
) {

  tft.setTextSize(
    textSize
  );


  String line = "";

  int lineNumber = 0;


  for (
    unsigned int i = 0;
    i < text.length();
    i++
  ) {

    char c =
      text[i];


    if (
      c == '\n'
    ) {

      tft.drawString(
        line,
        x,
        y + lineNumber * 10
      );


      line = "";

      lineNumber++;


      if (
        lineNumber >=
        maxLines
      )
        return;


      continue;
    }


    String test =
      line +
      String(c);


    if (
      tft.textWidth(
        test
      ) > width
    ) {

      tft.drawString(
        line,
        x,
        y + lineNumber * 10
      );


      line =
        String(c);

      lineNumber++;


      if (
        lineNumber >=
        maxLines
      )
        return;

    } else {

      line += c;
    }
  }


  if (
    line.length() > 0 &&
    lineNumber < maxLines
  ) {

    tft.drawString(
      line,
      x,
      y + lineNumber * 10
    );
  }
}