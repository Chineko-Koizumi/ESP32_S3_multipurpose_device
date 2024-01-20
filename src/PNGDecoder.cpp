#include "PNGDecoder.h"

PNG PNGDecoder::m_PNG;
File PNGDecoder::m_File;
TFT_eSPI *PNGDecoder::m_pScreen;
uint16_t PNGDecoder::m_aRowPixels[MAX_IMAGE_WIDTH];
uint16_t *PNGDecoder::m_pImagePixels;

uint16_t PNGDecoder::m_yImagePixel = 0U;

void PNGDecoder::setBackground(TFT_eSPI* pScreen , const char* fileName)
{   
    m_pImagePixels = (uint16_t*)malloc(PIXEL_ARRAY_SIZE);

    m_pScreen = pScreen;
    int rc = m_PNG.open((const char *)fileName, Open, Close, Read, Seek, PNGDraw);

    if (rc == PNG_SUCCESS) 
    {
        Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", m_PNG.getWidth(), m_PNG.getHeight(), m_PNG.getBpp(), m_PNG.getPixelType());
        rc = m_PNG.decode(NULL, 0);
        m_PNG.close();
    }

    m_pScreen->pushRect(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, m_pImagePixels);

    free(m_pImagePixels);
    m_yImagePixel = 0;

    memset(m_aRowPixels, 0, MAX_IMAGE_WIDTH);
}

void* PNGDecoder::Open(const char *fileName, int32_t *size)
{
    m_File = SD_MMC.open(fileName);
    *size = m_File.size();
    return & m_File;
}

void PNGDecoder::Close(void *handle)
{
    if(m_File) m_File.close();
}

int32_t PNGDecoder::Read(PNGFILE *handle, uint8_t *buffer, int32_t length)
{
    if (!m_File) return 0;

    return m_File.read(buffer, length);
}

int32_t PNGDecoder::Seek(PNGFILE *handle, int32_t position)
{
    if (!m_File) return 0;

    return m_File.seek(position);
}

void PNGDecoder::setRowPixels()
{
    memcpy( (uint8_t*)m_pImagePixels + m_yImagePixel * MAX_IMAGE_WIDTH * 2U /*memcpy uses bytes and pixel is 2bytes*/, 
            m_aRowPixels, 
            MAX_IMAGE_WIDTH * 2U /*memcpy uses bytes and pixel is 2bytes*/ );

    ++m_yImagePixel;
}

void PNGDecoder::PNGDraw(PNGDRAW *pDraw)
{
    m_PNG.getLineAsRGB565(pDraw, m_aRowPixels, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    setRowPixels();
}