#include <LittleFS.h>
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "GfxUi.h"
#include <JsonListener.h>
#include "display.h"
#include "persistence.h"
#include "settings.h"
#include "TransactionImage.h"  // Include TransactionImage header

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
OpenFontRender ofr;
FT6236 ts = FT6236(TFT_HEIGHT, TFT_WIDTH);
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite timeSprite = TFT_eSprite(&tft);
GfxUi ui = GfxUi(&tft, &ofr);

// ----------------------------------------------------------------------------
// Function prototypes (declarations)
// ----------------------------------------------------------------------------
void fetchLedgerData();
void displayLedgerData(const String& ledgerIndex, const String& timestamp, int totalTransactions, const JsonArray& transactions);
String formatTimestamp(const String& timestamp);
void drawText(const char* text, int x, int y, uint16_t color, uint8_t size, bool clearBackground = false);
void drawTitleAndFooter();

const char* ssid = "Spencer-wifi";
const char* password = "fuckoffu";

// ----------------------------------------------------------------------------
// Previous data variables
// ----------------------------------------------------------------------------
String prevLedgerIndex = "";
String prevTimestamp = "";
int prevTotalTransactions = -1;

// ----------------------------------------------------------------------------
// setup() & loop()
// ----------------------------------------------------------------------------
void setup() {
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Attempting to connect to WiFi...");
        delay(1000);  // Avoid spamming
    }
    Serial.println("Connected to WiFi network");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    initTouchScreen(&ts);
    initTft(&tft);
    logDisplayDebugInfo(&tft);

    initFileSystem();
    tft.fillScreen(TFT_BLACK);

    // Draw title and footer
    drawTitleAndFooter();

    setupJpegDecoder();  // Initialize the JPEG decoder
}

void loop() {
    fetchLedgerData();
    delay(2000);  // Reduce the frequency of data fetching
}

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void drawTitleAndFooter() {
    // Draw title at the top
    drawText("Desktop Ledger", 40, 10, TFT_WHITE, 3);

    // Draw the data labels
    drawText("Ledger Index:", 35, 50, TFT_CYAN, 2, true);
    drawText("Timestamp:", 35, 70, TFT_YELLOW, 2, true);
    drawText("Total Tx:", 35, 90, TFT_GREEN, 2, true);

    // Draw footer at the bottom right
    drawText("Created by Handy_4ndy", tft.width() - 130, tft.height() - 20, TFT_WHITE, 1);
}

void fetchLedgerData() {
    HTTPClient http;
    String url = "https://fast-springs-18239-92ca9a93578c.herokuapp.com/ledger-data";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(2048);  // Adjust the size as necessary
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }

            const char* ledgerIndex = doc["ledgerIndex"];
            String timestamp = doc["timestamp"].as<String>();
            int totalTransactions = doc["totalTransactions"];
            JsonArray transactions = doc["transactions"].as<JsonArray>();

            displayLedgerData(ledgerIndex, timestamp, totalTransactions, transactions);
        } else {
            Serial.print("HTTP request failed with error code: ");
            Serial.println(httpCode);
        }
    } else {
        Serial.println("HTTP request failed");
    }

    http.end();
}

void displayLedgerData(const String& ledgerIndex, const String& timestamp, int totalTransactions, const JsonArray& transactions) {
    // Clear the result section
    tft.fillRect(195, 50, 100, 60, TFT_BLACK);

    // Draw the updated results
    String formattedTimestamp = formatTimestamp(timestamp);
    drawText(ledgerIndex.c_str(), 195, 50, TFT_CYAN, 2, false);
    drawText(formattedTimestamp.c_str(), 195, 70, TFT_YELLOW, 2, false);
    char totalTxBuffer[16];
    sprintf(totalTxBuffer, "%d", totalTransactions);
    drawText(totalTxBuffer, 195, 90, TFT_GREEN, 2, false);

    // Display transaction images
    for (JsonObject transaction : transactions) {
        const char* transactionType = transaction["TransactionType"];
        displayTransaction(transactionType);  // Display image based on transaction type
        delay(2000);  // Adjust delay as necessary
    }

    // Update previous data
    prevLedgerIndex = ledgerIndex;
    prevTimestamp = timestamp;
    prevTotalTransactions = totalTransactions;

    Serial.println("Data displayed");
}

String formatTimestamp(const String& timestamp) {
    // Extract time only, assuming format "YYYY-MM-DD HH:MM:SS"
    int spaceIndex = timestamp.indexOf(' ');
    if (spaceIndex > 0) {
        String timePart = timestamp.substring(spaceIndex + 1, spaceIndex + 9);  // Get HH:MM:SS

        // Parse hour, minute, second
        int hour = timePart.substring(0, 2).toInt();
        int minute = timePart.substring(3, 5).toInt();
        int second = timePart.substring(6, 8).toInt();

        // Add one hour for BST
        hour = (hour + 1) % 24;  // Wrap around if hour is 23

        // Format back to HH:MM:SS
        char formattedTime[9];
        sprintf(formattedTime, "%02d:%02d:%02d", hour, minute, second);
        return String(formattedTime);
    }
    return timestamp;  // Return the original if format is unexpected
}

void drawText(const char* text, int x, int y, uint16_t color, uint8_t size, bool clearBackground) {
    tft.setTextSize(size);
    int16_t textWidth = tft.textWidth(text);
    int16_t textHeight = tft.fontHeight(size);
    if (clearBackground) {
        tft.fillRect(x, y, textWidth, textHeight, TFT_BLACK);  // Clear background before drawing
    }
    tft.setTextColor(color);
    tft.setCursor(x, y);
    tft.print(text);
}
