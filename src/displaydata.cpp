#include "displaydata.h"
#include "globals.h"
#include <ArduinoJson.h>

void displaySplashScreen() {
    ui.drawLogo();
}

void drawTemplate() {
    drawText("Desktop Ledger", 40, 10, TFT_WHITE, 3);
    drawText("Ledger Index:", 35, 50, TFT_CYAN, 2, true);
    drawText("Timestamp:", 35, 70, TFT_YELLOW, 2, true);
    drawText("Total Tx:", 35, 90, TFT_GREEN, 2, true);
    drawText("Created by Handy_4ndy", tft.width() - 130, tft.height() - 20, TFT_WHITE, 1);
}

void displayLedgerData(const String& ledgerIndex, const String& timestamp, int totalTransactions, const JsonArray& transactions) {
    tft.fillRect(195, 50, 100, 60, TFT_BLACK);

    String formattedTimestamp = formatTimestamp(timestamp);
    drawText(ledgerIndex.c_str(), 195, 50, TFT_CYAN, 2, false);
    drawText(formattedTimestamp.c_str(), 195, 70, TFT_YELLOW, 2, false);
    char totalTxBuffer[16];
    sprintf(totalTxBuffer, "%d", totalTransactions);
    drawText(totalTxBuffer, 195, 90, TFT_GREEN, 2, false);

    /*for (JsonObject transaction : transactions) {
        const char* transactionType = transaction["TransactionType"];
        displayTransaction(transactionType);  // Ensure this function is properly declared and defined
    }*/

    prevLedgerIndex = ledgerIndex;
    prevTimestamp = timestamp;
    prevTotalTransactions = totalTransactions;

    Serial.println("Data displayed");
}

String formatTimestamp(const String& timestamp) {
    int spaceIndex = timestamp.indexOf(' ');
    if (spaceIndex > 0) {
        String timePart = timestamp.substring(spaceIndex + 1, spaceIndex + 9);
        int hour = timePart.substring(0, 2).toInt();
        int minute = timePart.substring(3, 5).toInt();
        int second = timePart.substring(6, 8).toInt();

        hour = (hour + 1) % 24;
        char formattedTime[9];
        sprintf(formattedTime, "%02d:%02d:%02d", hour, minute, second);
        return String(formattedTime);
    }
    return timestamp;
}

void drawText(const char* text, int x, int y, uint16_t color, uint8_t size, bool clearBackground) {
    tft.setTextSize(size);
    int16_t textWidth = tft.textWidth(text);
    int16_t textHeight = tft.fontHeight(size);
    if (clearBackground) {
        tft.fillRect(x, y, textWidth, textHeight, TFT_BLACK);
    }
    tft.setTextColor(color);
    tft.setCursor(x, y);
    tft.print(text);
}

void displayLoadingScreen(int progress) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 10);
    tft.println("Loading...");

    int barWidth = 200;
    int barHeight = 20;
    int x = (tft.width() - barWidth) / 2;
    int y = (tft.height() - barHeight) / 2;
    tft.drawRect(x, y, barWidth, barHeight, TFT_WHITE);
    int progressWidth = map(progress, 0, 100, 0, barWidth);
    tft.fillRect(x, y, progressWidth, barHeight, TFT_GREEN);
}

void updateProgress(int progress) {
    displayLoadingScreen(progress);
}
