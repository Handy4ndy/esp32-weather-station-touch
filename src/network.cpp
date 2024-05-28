#include "network.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "displaydata.h"
#include "globals.h"

void fetchLedgerData() {
    HTTPClient http;
    String url = "https://fast-springs-18239-92ca9a93578c.herokuapp.com/ledger-data";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;  
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
