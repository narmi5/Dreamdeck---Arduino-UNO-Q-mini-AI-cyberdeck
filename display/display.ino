#include <SPI.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define HRES 320
#define VRES 240

const char* ssid     = "CYD_AI_Mesh";
const char* password = "ArduinoUnoQ123";
const char* serverIP = "10.42.0.1"; // IP dell'hotspot Linux dell'Uno Q

#define DOUT 39  
#define DIN  32  
#define DCS  33  
#define DCLK 25  

TFT_eSPI tft = TFT_eSPI();
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);

String promptUtente = "";
String rispostaAI = "Connecting to board . . .";

struct Pulsante {
  String etichetta;
  int x; int y; int w; int h;
};

Pulsante tasti[30] = {
  {"Q", 5, 120, 26, 26}, {"W", 36, 120, 26, 26}, {"E", 67, 120, 26, 26}, {"R", 98, 120, 26, 26}, {"T", 129, 120, 26, 26}, {"Y", 160, 120, 26, 26}, {"U", 191, 120, 26, 26}, {"I", 222, 120, 26, 26}, {"O", 253, 120, 26, 26}, {"P", 284, 120, 26, 26},
  {"A", 15, 150, 26, 26}, {"S", 46, 150, 26, 26}, {"D", 77, 150, 26, 26}, {"F", 108, 150, 26, 26}, {"G", 139, 150, 26, 26}, {"H", 170, 150, 26, 26}, {"J", 201, 150, 26, 26}, {"K", 232, 150, 26, 26}, {"L", 263, 150, 26, 26},
  {"Z", 30, 180, 26, 26}, {"X", 61, 180, 26, 26}, {"C", 92, 180, 26, 26}, {"V", 123, 180, 26, 26}, {"B", 154, 180, 26, 26}, {"N", 185, 180, 26, 26}, {"M", 216, 180, 26, 26}, {"DEL", 252, 180, 42, 26},
  {"SPACE", 40, 210, 130, 26}, {"SEND", 180, 210, 100, 26}, {"OFF", 285, 210, 30, 26}
};


void disegnaInterfaccia();
void aggiornaAreaTesto();
void drawStringWordWrap(String testo, int x, int y, int larghezzaMassima);
void inviaPromptCyberdeck(String testo);

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  touch.setRotation(1);
  touch.setCal(400, 3400, 250, 3600, 320, 240, 1);

  disegnaInterfaccia();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int t = 0;
  while (WiFi.status() != WL_CONNECTED && t < 30) {
    delay(500);
    t++;
    rispostaAI = (t % 2 == 0) ? "Connecting to board . ." : "Connecting to board .";
    aggiornaAreaTesto();
  }
 
  if (WiFi.status() == WL_CONNECTED) {
    rispostaAI = "Ready! Send a prompt";
  } else {
    rispostaAI = "Connection error. Reboot display through the top right button.";
  }
  aggiornaAreaTesto();
}

void loop() {
  if (touch.Pressed()) {
    int xt = touch.X();
    int yt = touch.Y();
    
    for (int i = 0; i < 30; i++) { // Changed 29 to 30
      if (xt >= tasti[i].x && xt <= (tasti[i].x + tasti[i].w) &&
          yt >= tasti[i].y && yt <= (tasti[i].y + tasti[i].h)) {
        
        if (tasti[i].etichetta == "SEND") {
          if (promptUtente.length() > 0 && WiFi.status() == WL_CONNECTED) {
            inviaPromptCyberdeck(promptUtente);
            promptUtente = "";
          }
        }
        else if (tasti[i].etichetta == "DEL") {
          if (promptUtente.length() > 0) {
            promptUtente.remove(promptUtente.length() - 1);
          }
        }
        else if (tasti[i].etichetta == "SPACE") {
          promptUtente += " ";
        }
        else if (tasti[i].etichetta == "OFF") {
          inviaComandoSpegnimento();
        }
        else {
          promptUtente += tasti[i].etichetta;
        }
        
        aggiornaAreaTesto();
        delay(250);
        break;
      }
    }
  }
}

void inviaPromptCyberdeck(String testo) {
  HTTPClient http;
  rispostaAI = "is thinking...";
  aggiornaAreaTesto();

  // Porta nativa 7000 usata dal WebUI Brick di App Lab
  String url = "http://" + String(serverIP) + ":7000/api/message";
  
  http.begin(url);
  http.setTimeout(60000); // Mantiene la connessione aperta fino a 60s per la risposta del LLM
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"prompt\":\"" + testo + "\"}";
 
  int httpResponseCode = http.POST(jsonPayload);
 
  if (httpResponseCode > 0) {
    String payload = http.getString();
    
    // Parsing per estrarre la chiave "text"
    int index = payload.indexOf("\"text\":\"");
    if (index != -1) {
      String sub = payload.substring(index + 8);
      int endIndex = sub.indexOf("\"}");
      if (endIndex == -1) endIndex = sub.indexOf("\"");
      rispostaAI = sub.substring(0, endIndex);
    } else {
      rispostaAI = payload;
    }
  } else {
    rispostaAI = "HTTP Error: " + String(httpResponseCode);
    Serial.printf("[HTTP] Fallito, errore: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  
  http.end();
  aggiornaAreaTesto();
}

void disegnaInterfaccia() {
  tft.fillScreen(TFT_BLACK);
  tft.drawFastHLine(0, 115, 320, TFT_WHITE);
  
  for (int i = 0; i < 30; i++) { // Changed 29 to 30
    // Paint OFF button RED, keep all other keys BLUE
    uint16_t coloreSfondo = (tasti[i].etichetta == "OFF") ? TFT_RED : TFT_BLUE;

    tft.fillRect(tasti[i].x, tasti[i].y, tasti[i].w, tasti[i].h, coloreSfondo);
    tft.drawRect(tasti[i].x, tasti[i].y, tasti[i].w, tasti[i].h, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);

    if (tasti[i].etichetta.length() == 1) {
      tft.drawString(tasti[i].etichetta, tasti[i].x + 9, tasti[i].y + 9);
    } else if (tasti[i].etichetta == "DEL") {
      tft.drawString(tasti[i].etichetta, tasti[i].x + 12, tasti[i].y + 9);
    } else if (tasti[i].etichetta == "SEND") {
      tft.drawString(tasti[i].etichetta, tasti[i].x + 38, tasti[i].y + 9);
    } else if (tasti[i].etichetta == "SPACE") {
      tft.drawString(tasti[i].etichetta, tasti[i].x + 42, tasti[i].y + 9);
    } else if (tasti[i].etichetta == "OFF") {
      tft.drawString(tasti[i].etichetta, tasti[i].x + 6, tasti[i].y + 9);
    }
  }
}

void drawStringWordWrap(String testo, int x, int y, int larghezzaMassima, int maxRighe) {
  int rigaCorrente = 0;
  String parola = "";
  String rigaAccumulata = "";
  
  for (unsigned int i = 0; i < testo.length(); i++) {
    char c = testo[i];
    
    if (c == ' ') {
      // Controllo se la riga corrente supera la larghezza massima del display
      if (tft.textWidth(rigaAccumulata + parola) > larghezzaMassima) {
        // Se raggiungiamo l'ultima riga visibile prima della tastiera, tronca con "..."
        if (rigaCorrente >= maxRighe - 1) {
          tft.drawString(rigaAccumulata + "...", x, y + (rigaCorrente * 16), 2);
          return;
        }
        tft.drawString(rigaAccumulata, x, y + (rigaCorrente * 16), 2);
        rigaCorrente++;
        rigaAccumulata = parola + " ";
      } else {
        rigaAccumulata += parola + " ";
      }
      parola = "";
    } else {
      parola += c;
    }
  }

  // Stampa l'ultima parola accumulata
  if (parola.length() > 0) {
    if (tft.textWidth(rigaAccumulata + parola) > larghezzaMassima) {
      if (rigaCorrente >= maxRighe - 1) {
        tft.drawString(rigaAccumulata + "...", x, y + (rigaCorrente * 16), 2);
        return;
      }
      tft.drawString(rigaAccumulata, x, y + (rigaCorrente * 16), 2);
      rigaCorrente++;
      rigaAccumulata = parola;
    } else {
      rigaAccumulata += parola;
    }
  }

  if (rigaAccumulata.length() > 0 && rigaCorrente < maxRighe) {
    tft.drawString(rigaAccumulata, x, y + (rigaCorrente * 16), 2);
  }
}

void aggiornaAreaTesto() {
  // Pulisce solo l'area di visualizzazione del testo sopra la tastiera (0-114px)
  tft.fillRect(0, 0, 320, 114, TFT_BLACK);
  
  // Input utente (Riga 1)
  tft.setTextColor(TFT_GREEN);
  tft.drawString("YOU: " + promptUtente, 10, 5, 2);
  
  // Risposta AI (Righe 2-5, max 4 righe per non sovrapporsi ai tasti)
  tft.setTextColor(TFT_WHITE);
  drawStringWordWrap("UNO: " + rispostaAI, 10, 28, 300, 4);
}

void inviaComandoSpegnimento() {
  HTTPClient http;
  rispostaAI = "Shutting off UNO Q";
  aggiornaAreaTesto();

  String url = "http://" + String(serverIP) + ":7000/api/shutdown";
  
  http.begin(url);
  http.setTimeout(5000);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST("{}");
 
  if (httpResponseCode > 0) {
    rispostaAI = "AI terminated. Now able to shut down. To do it, unplug the UNO Q";
  } else {
    rispostaAI = "AI terminated. Now able to shut down. To do it, unplug the UNO Q";
  }
  
  http.end();
  aggiornaAreaTesto();
}