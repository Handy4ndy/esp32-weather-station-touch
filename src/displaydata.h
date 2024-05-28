#ifndef DISPLAYDATA_H
#define DISPLAYDATA_H

#include <TFT_eSPI.h>
#include <ArduinoJson.h>

extern TFT_eSPI tft;

void displaySplashScreen();
void drawTemplate();
void displayLedgerData(const String& ledgerIndex, const String& timestamp, int totalTransactions, const JsonArray& transactions);
String formatTimestamp(const String& timestamp);
void drawText(const char* text, int x, int y, uint16_t color, uint8_t size, bool clearBackground = false);
void displayLoadingScreen(int progress);
void updateProgress(int progress);

#endif // DISPLAYDATA_H
