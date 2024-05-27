#include <TFT_eSPI.h>  // Include the graphics library (this includes the sprite functions)

extern TFT_eSPI tft; // Use the TFT object created in main.cpp

void displayTransaction(const char* transactionType) {
    if (strcmp(transactionType, "Payment") == 0) {
        tft.drawBmp("/data/xrpl-transactions/payment.bmp", 0, 0); // Display payment.bmp at coordinates (0, 0)
    } else if (strcmp(transactionType, "NFTokenCreateOffer") == 0) {
        tft.drawBmp("/data/xrpl-transactions/nft-create-offer-mint.bmp", 0, 0); // Display nft_offer_create.bmp at coordinates (0, 0)
    } else {
        tft.drawBmp("/default.bmp", 0, 0); // Display default.bmp for other transaction types
    }
}
