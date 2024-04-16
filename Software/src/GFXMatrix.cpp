#include "GFXMatrix.h"
#include <hub75.h>

GFXMatrix::GFXMatrix(uint16_t w, uint16_t h): Adafruit_GFX(w,h)
{
    buffer=static_cast<uint32_t*>(malloc(WIDTH*HEIGHT*sizeof(uint32_t)));
    overlay_buffer=static_cast<uint8_t*>(malloc(WIDTH*HEIGHT*sizeof(uint8_t)));
    // Set overlay and image to black
    memset(overlay_buffer, 0, WIDTH*HEIGHT*sizeof(uint8_t));
    memset(buffer, 0, WIDTH*HEIGHT*sizeof(uint32_t));
}

GFXMatrix::~GFXMatrix()
{
    if(buffer)
        free(buffer);
    if(overlay_buffer)
        free(overlay_buffer);
    buffer = nullptr;
    overlay_buffer = nullptr;
}

void GFXMatrix::begin() {
    hub75_config(8);
    hub75_set_masterbrightness(20);
}


void GFXMatrix::setBrightness(int brt) {
        # define minbrightness 55
        # define maxbrightness 0

        // convert 0-100 to 57 - 0
        int scbr = (int)(brt * (minbrightness / -100.0f) + minbrightness);
        hub75_set_masterbrightness(scbr);

}



void GFXMatrix::clear()
{
    memset(buffer, 0, WIDTH*HEIGHT*sizeof(uint32_t));
}

void GFXMatrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    buffer[x + y*WIDTH] = LEDmx_565toRGB(color);
}

void GFXMatrix::display()
{
    // for(int i = 0; i < 64*64; i++)
    // {
    //     buffer[i] = 0xFF;
    // }
    // for(int i = 0; i < 64; i++)
    // {
    //     buffer[i + 64*i] = 0xFFFFFF;
    // }
    hub75_update(buffer, overlay_buffer);
}
