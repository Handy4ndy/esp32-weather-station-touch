#include <LittleFS.h>
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <JsonListener.h>
#include <map>

#include "globals.h"
#include "GfxUi.h"
#include "display.h"
#include "displaydata.h"

#include "connectivity.h"
#include "network.h"
#include "jsonparse.h"

#include "persistence.h"
#include "settings.h"
#include "util.h"
#include "TransactionImage.h"


// Globals
OpenFontRender ofr;
GfxUi ui = GfxUi(&tft, &ofr);
TFT_eSPI tft = TFT_eSPI();
FT6236 ts = FT6236(TFT_HEIGHT, TFT_WIDTH);  // Add this line

// Previous data variables
String prevLedgerIndex = "";
String prevTimestamp = "";
int prevTotalTransactions = -1;

void setup() {
    Serial.begin(9600);

    initTouchScreen(&ts);
    initTft(&tft);
    initFileSystem();
    logMemoryStats();

    displaySplashScreen(); 

    startWiFi();
    Serial.println("Connected to WiFi network");

    tft.fillScreen(TFT_BLACK);
    drawTemplate();
}

void loop() {
    fetchLedgerData();
    delay(1000);
}
