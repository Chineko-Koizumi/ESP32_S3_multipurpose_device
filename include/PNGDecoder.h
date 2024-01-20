#ifndef PNGDECODER_H
#define PNGDECODER_H

#include <PNGdec.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <TFT_eSPI.h> 

#include "InlineDefinitions.h"


class PNGDecoder
{
private:
   static PNG m_PNG;
   static File m_File;
   static TFT_eSPI* m_pScreen;

   static uint16_t m_aRowPixels[MAX_IMAGE_WIDTH];
   static uint16_t *m_pImagePixels;

   static uint16_t m_yImagePixel;

   static void* Open(const char *fileName, int32_t *size);
   static void Close(void *handle);
   static int32_t Read(PNGFILE *handle, uint8_t *buffer, int32_t length);
   static int32_t Seek(PNGFILE *handle, int32_t position);
   static void PNGDraw(PNGDRAW *pDraw);

   static void setRowPixels();

public:
    static void setBackground(TFT_eSPI* pScreen , const char* fileName);

    PNGDecoder() = delete;
    ~PNGDecoder() = delete;

};



#endif