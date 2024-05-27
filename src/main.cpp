#include <LittleFS.h>

#include <TJpg_Decoder.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "GfxUi.h"
#include <JsonListener.h>

//#include "connectivity.h"
#include "display.h"
#include "persistence.h"
#include "settings.h"
#include "TransactionDisplay.h" 

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
void getLedgerData();
String formatTimestamp(const String& timestamp);
void drawCenteredText(const char* text, int y, uint16_t color, uint8_t size);

const char* ssid = "Spencer-wifi";
const char* password = "fuckoffu";

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
    tft.fillScreen(TFT_WHITE);
    tft.fillScreen(TFT_BLACK);
}

void loop() {
    getLedgerData();
}

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void getLedgerData() {
    HTTPClient http;
    String url = "https://fast-springs-18239-92ca9a93578c.herokuapp.com/ledger-data";

    //Serial.println("Fetching data...");

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;  // Increase the size if necessary
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }

            const char* ledgerIndex = doc["ledgerIndex"];
            String timestamp = doc["timestamp"].as<String>();
            int totalTransactions = doc["totalTransactions"];
            JsonObject counts = doc["counts"];

            // Format the timestamp to a more readable form
            String formattedTimestamp = formatTimestamp(timestamp);

            // Display the data with improved layout and colors
            drawCenteredText("Ledger Index:", 10, TFT_CYAN, 2);
            drawCenteredText(ledgerIndex, 40, TFT_CYAN, 2);

            drawCenteredText("Timestamp:", 70, TFT_YELLOW, 2);
            drawCenteredText(formattedTimestamp.c_str(), 100, TFT_YELLOW, 2);

            drawCenteredText("Total Tx:", 130, TFT_GREEN, 2);
            char totalTxBuffer[16];
            sprintf(totalTxBuffer, "%d", totalTransactions);
            drawCenteredText(totalTxBuffer, 160, TFT_GREEN, 2);

            drawCenteredText("Transaction Counts:", 190, TFT_WHITE, 1);

            int y = 210;
            for (JsonPair count : counts) {
                char countBuffer[32];
                sprintf(countBuffer, "%s: %d", count.key().c_str(), count.value().as<int>());
                drawCenteredText(countBuffer, y, TFT_WHITE, 1);
                y += 20;
            }

            for (JsonObject transaction : transactions) {
                const char* transactionType = transaction["TransactionType"];
                displayTransaction(transactionType);  // Call the function to display the image based on transaction type
                delay(2000);  // Adjust delay as necessary
            }

            
            Serial.println("Data displayed");

            delay(2000);
            drawCenteredText(ledgerIndex, 40, TFT_BLACK, 2);
            drawCenteredText(formattedTimestamp.c_str(), 100, TFT_BLACK, 2);
            sprintf(totalTxBuffer, "%d", totalTransactions);
            drawCenteredText(totalTxBuffer, 160, TFT_BLACK, 2);
            for (JsonPair count : counts) {
                char countBuffer[32];
                sprintf(countBuffer, "%s: %d", count.key().c_str(), count.value().as<int>());
                drawCenteredText(countBuffer, y, TFT_BLACK, 1);
                y += 20;
            }


            Serial.println("Data displayed");
        } else {
            Serial.print("HTTP request failed with error code: ");
            Serial.println(httpCode);
        }
    } else {
        Serial.println("HTTP request failed");
    }
}

String formatTimestamp(const String& timestamp) {
    // Extract date and time only, ignore fractional seconds and timezone
    int spaceIndex = timestamp.indexOf(' ');
    int dotIndex = timestamp.indexOf('.');
    if (spaceIndex > 0 && dotIndex > 0) {
        return timestamp.substring(0, dotIndex);
    }
    return timestamp;  // Return the original if format is unexpected
}

void drawCenteredText(const char* text, int y, uint16_t color, uint8_t size) {
    tft.setTextSize(size);
    int16_t textWidth = tft.textWidth(text);
    int16_t x = (tft.width() - textWidth) / 2;
    tft.setTextColor(color);
    tft.setCursor(x, y);
    tft.print(text);
}

