#ifndef GLOBALS_H
#define GLOBALS_H

#include <TFT_eSPI.h>
#include "GfxUi.h"
#include "FT6236.h"

extern OpenFontRender ofr;
extern GfxUi ui;
extern TFT_eSPI tft;
extern FT6236 ts;

// Previous data variables
extern String prevLedgerIndex;
extern String prevTimestamp;
extern int prevTotalTransactions;

#endif // GLOBALS_H
