#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "TransactionImage.h"

// Reference to the global TFT_eSPI object
extern TFT_eSPI tft;

bool tftOutputJpeg(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    tft.pushImage(x, y, w, h, bitmap);
    return true;
}

void setupJpegDecoder() {
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(tftOutputJpeg);
}

void displayTransaction(const char* transactionType) {
    if (strcmp(transactionType, "Payment") == 0) {
        TJpgDec.drawJpg(0, 0, "/payment.jpg");
    } else if (strcmp(transactionType, "CreateOffer") == 0) {
        TJpgDec.drawJpg(0, 0, "/offer-create.jpg");
    } else if (strcmp(transactionType, "NFTCreateOffer") == 0) {
        TJpgDec.drawJpg(0, 0, "/nft-offer-create.jpg");
    } else if (strcmp(transactionType, "OfferCancel") == 0) {
        TJpgDec.drawJpg(0, 0, "/offer-cancel.jpg");
    } else if (strcmp(transactionType, "NFTokenAcceptOffer") == 0) {
        TJpgDec.drawJpg(0, 0, "/nft-accept-offer.jpg");
    } else if (strcmp(transactionType, "NFTokenCancelOffer") == 0) {
        TJpgDec.drawJpg(0, 0, "/nft-offer-cancel.jpg");
    } else if (strcmp(transactionType, "NFTokenMint") == 0) {
        TJpgDec.drawJpg(0, 0, "/nft-offer-create.jpg");
    } else if (strcmp(transactionType, "AccountSet") == 0) {
        TJpgDec.drawJpg(0, 0, "/ticket-create.jpg");
    } else if (strcmp(transactionType, "AccountDelete") == 0) {
        TJpgDec.drawJpg(0, 0, "/account-delete.jpg");
    } else if (strcmp(transactionType, "SetRegularKey") == 0) {
        TJpgDec.drawJpg(0, 0, "/set-regular-key.jpg");
    } else if (strcmp(transactionType, "SignerListSet") == 0) {
        TJpgDec.drawJpg(0, 0, "/set-regular-key.jpg");
    } else if (strcmp(transactionType, "TicketCreate") == 0) {
        TJpgDec.drawJpg(0, 0, "/ticket-create.jpg");
    } else if (strcmp(transactionType, "TrustSet") == 0) {
        TJpgDec.drawJpg(0, 0, "/trust-set.jpg");
    } else {
        TJpgDec.drawJpg(0, 0, "/unknown.jpg");
    }
}
