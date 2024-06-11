#include <string> // Added to use std::string
#include <map>
#include <ArduinoJson.h>
#include <TJpg_Decoder.h>

#include "displaydata.h"
#include "globals.h"
#include "GfxUi.h"

GfxUi gfxUi(&tft, &ofr);

#define NUM_IMAGES 12 // Number of images

// Image file paths
const char* imagePaths[NUM_IMAGES] = {
    
    "/account-modify.bmp",
    "/nft-accept-offer.bmp",
    "/nft-create-offer-mint.bmp",
    "/nft-offer-cancel.bmp",
    "/offer-cancel.bmp",
    "/offer-create.bmp",
    "/payment.bmp",
    "/ticket-create.bmp",
    "/trustset-modify.bmp",
    "/offer-cancel.bmp",
    "/offer-create.bmp",
    "/payment.bmp",
};

// Function to draw images on the screen
void drawImages() {
    for (int i = 0; i < NUM_IMAGES; i++) {
        int randX = random(tft.width());
        int randY = random(tft.height());
        gfxUi.drawBmp(imagePaths[i], randX, randY);
    }
}

void displaySplashScreen() {
    unsigned long startTime = millis(); // Get start time

    while (millis() - startTime < 5000) { // Display images for 10 seconds
        drawImages();   // Draw images on the screen
        //delay(100); // Adjust animation speed
    }
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

    displayTransactions(transactions);

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

// Function to display transactions for a ledger
void displayTransactions(const JsonArray& transactions) {
    // Define the mapping between transaction types and BMP images
    std::map<std::string, std::string> transactionImages = {
        {"Payment", "/payment.bmp"},
        {"OfferCreate", "/offer-create.bmp"},
        {"OfferCancel", "/offer-cancel.bmp"},
        {"TicketCreate", "/ticket-create.bmp"},
        {"TrustSet", "/trustset-modify.bmp"},
        {"NFTokenCreateOffer", "/nft-create-offer-mint.bmp"},
        {"NFTokenAcceptOffer", "/nft-accept-offer.bmp"},
        {"NFTokenCancelOffer", "/nft-offer-cancel.bmp"},
        {"AccountSet", "/account-modify.bmp"},
        {"AccountDelete", "/account-delete.bmp"},
        {"AMMBid", "/offer-create.bmp"},
        {"AMMCreate", "/offer-create.bmp"},
        {"AMMDelete", "/offer-cancel.bmp"},
        {"NFTokenBurn", "/nft-offer-cancel.bmp"},
        // Add more mappings as needed
    };

    tft.fillRect(0, 130, 320, 330, TFT_BLACK);

    int x = 35; // Initial x-coordinate for BMP image
    int y = 130; // Initial y-coordinate for BMP image
    int imageWidth = 12;
    int imageHeight = 12;
    
    int spacing = 10; // Spacing between BMP images

    // Iterate through each transaction
    for (JsonObject transaction : transactions) {
        // Retrieve transaction type
        std::string transactionType = transaction["TransactionType"];

        // Check if the transaction type has a corresponding BMP image
        if (transactionImages.find(transactionType) != transactionImages.end()) {
            // Retrieve BMP image path for the transaction type
            std::string imagePath = transactionImages[transactionType];

            // Draw BMP image on the main screen
            gfxUi.drawBmp(imagePath.c_str(), x, y);

            x += imageWidth + spacing;

            // Check if reached the end of the screen width
            if (x + imageWidth > (tft.width() -25)  ) {
                // Move to the next row
                x = 35;
                y += imageHeight + spacing;
            }
        } else {
            // Handle cases where there is no corresponding BMP image for the transaction type
            Serial.println("No BMP image found for transaction type: " + String(transactionType.c_str()));


            gfxUi.drawBmp("/unknown.bmp", x, y);
        }
    }
}