#ifndef SDBMPREADER_H
#define SDBMPREADER_H

#include <fstream>
#include "InlineDefinitions.h"
#include "HardwareSerial.h"

class BMP 
{
public:
    uint8_t* m_pData;

    BMP(const char *fname);
    void read(const char *fname);

private:

    BMPFileHeader m_FileHeader;
    BMPInfoHeader m_BmpInfoHeader;
    BMPColorHeader m_BmpColorHeader;

    uint32_t m_DataCount;

    uint32_t m_RowStride{ 0 };

    // Add 1 to the m_RowStride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride);

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void check_color_header(BMPColorHeader &bmp_color_header);
};

#endif