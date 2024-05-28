// SPDX-FileCopyrightText: 2023 ThingPulse Ltd., https://thingpulse.com
// SPDX-License-Identifier: MIT

// Based on https://github.com/Bodmer/OpenWeather/blob/main/examples/TFT_eSPI_OpenWeather_LittleFS/GfxUi.cpp

#include "GfxUi.h"

#define XRPL_LOGO "/logo.jpeg"

GfxUi::GfxUi(TFT_eSPI *tft, OpenFontRender *ofr) {
  _tft = tft;
  _ofr = ofr;
}

// Bodmer's streamlined x2 faster "no seek" version
void GfxUi::drawBmp(String filename, uint16_t x, uint16_t y) {

  if ((x >= _tft->width()) || (y >= _tft->height()))
    return;

  fs::File bmpFS;

  // Note: ESP32 passes "open" test even if file does not exist, whereas ESP8266
  // returns NULL
  if (!LittleFS.exists(filename)) {
    log_e(" File not found");
    return;
  }

  // Open requested file
  bmpFS = LittleFS.open(filename, "r");

  uint32_t seekOffset;
  uint16_t w, h, row;
  uint8_t r, g, b;
  bool oldSwap = false;

  if (read16(bmpFS) == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
      y += h - 1;

      oldSwap = _tft->getSwapBytes();
      _tft->setSwapBytes(true);
      bmpFS.seek(seekOffset);

      // Calculate padding to avoid seek
      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {

        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t *bptr = lineBuffer;
        uint16_t *tptr = (uint16_t *)lineBuffer;
        // Convert 24 to 16 bit colours using the same line buffer for results
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        _tft->pushImage(x, y--, w, 1, (uint16_t *)lineBuffer);
      }
    } else
      log_e("BMP format not recognized.");
  }
  _tft->setSwapBytes(oldSwap);
  bmpFS.close();
}

void GfxUi::drawLogo() {
  if (LittleFS.exists(XRPL_LOGO)) {
    uint16_t w = 0, h = 0;
    TJpgDec.getFsJpgSize(&w, &h, XRPL_LOGO, LittleFS);
    TJpgDec.drawFsJpg((_tft->width() - w) / 2, 30, XRPL_LOGO, LittleFS);
  }
}

void GfxUi::drawProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                            uint8_t percentage, uint16_t frameColor,
                            uint16_t barColor) {
  if (percentage == 0) {
    _tft->fillRoundRect(x0, y0, w, h, 3, TFT_BLACK);
  }
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth = w - 2 * margin;
  _tft->drawRoundRect(x0, y0, w, h, 3, frameColor);
  _tft->fillRect(x0 + margin, y0 + margin, barWidth * percentage / 100.0,
                 barHeight, barColor);
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t GfxUi::read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t GfxUi::read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
