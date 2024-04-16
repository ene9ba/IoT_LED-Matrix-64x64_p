#ifndef GFXMATRIX_H
#define GFXMATRIX_H

#include <Adafruit_GFX.h>

///  An Adafruit GFX implementation for the Matrix
class GFXMatrix : public Adafruit_GFX {
public:
  GFXMatrix(uint16_t w, uint16_t h);
  ~GFXMatrix();
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void display();
  void begin();
  void clear();
  void setBrightness(int brt);
private:
  static uint32_t LEDmx_565toRGB(uint16_t pix) {
    uint32_t r_gamma = pix & 0xf800u;
    r_gamma *= r_gamma;
    uint32_t g_gamma = pix & 0x07e0u;
    g_gamma *= g_gamma;
    uint32_t b_gamma = pix & 0x001fu;
    b_gamma *= b_gamma;
    return (r_gamma >> 24 << 16) | (g_gamma >> 14 << 8) | (b_gamma >> 2 << 0);
  }



  uint32_t *buffer = nullptr;
  uint8_t *overlay_buffer = nullptr;

};

#endif // GFXMATRIX_H