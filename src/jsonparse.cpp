#include "jsonparse.h"
#include <ArduinoJson.h>
#include "globals.h"

void parseTransactions(const char* jsonResponse) {
    JsonDocument doc;  // Use DynamicJsonDocument
    DeserializationError error = deserializeJson(doc, jsonResponse);

    if (error) {
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());
        return;
    }

    /*JsonArray transactions = doc["transactions"];
    for (JsonObject transaction : transactions) {
        const char* transactionType = transaction["TransactionType"];
        displayTransaction(transactionType);  // Ensure this function is properly declared and defined
    }*/
}
