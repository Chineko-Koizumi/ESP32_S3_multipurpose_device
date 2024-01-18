#include "SdBmpReader.h"

BMP::BMP(const char *fname) 
{
    read(fname);
}

void BMP::read(const char *fname) 
{
    std::ifstream inp{ fname, std::ios_base::binary };
    if (inp) 
    {
        inp.read((char*)&m_FileHeader, sizeof(m_FileHeader));
        if(m_FileHeader.file_type != 0x4D42) 
        {
            Serial.println("Error! Unrecognized file format.");
            inp.close();
            return;
        }
        inp.read((char*)&m_BmpInfoHeader, sizeof(m_BmpInfoHeader));

        // The BMPColorHeader is used only for transparent images
        if(m_BmpInfoHeader.bit_count == 32) 
        {
            // Check if the file has bit mask color information
            if(m_BmpInfoHeader.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) 
            {
                inp.read((char*)&m_BmpColorHeader, sizeof(m_BmpColorHeader));
                // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                check_color_header(m_BmpColorHeader);
            } 
            else 
            {
                Serial.printf("Error! The file %s does not seem to contain bit mask information \n", fname);
                Serial.println("Error! Unrecognized file format.");
                inp.close();
                return;
            }
        }

        // Jump to the pixel data location
        inp.seekg(m_FileHeader.offset_data, inp.beg);

        // Adjust the header fields for output.
        // Some editors will put extra info in the image file, we only save the headers and the data.
        if(m_BmpInfoHeader.bit_count == 32) 
        {
            m_BmpInfoHeader.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            m_FileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        } 
        else 
        {
            m_BmpInfoHeader.size = sizeof(BMPInfoHeader);
            m_FileHeader.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        m_FileHeader.file_size = m_FileHeader.offset_data;

        if (m_BmpInfoHeader.height < 0) 
        {
            Serial.println("The program can treat only BMP images with the origin in the bottom left corner!");
            inp.close();
            return;
        }
        m_DataCount = m_BmpInfoHeader.width * m_BmpInfoHeader.height * m_BmpInfoHeader.bit_count / 8;
        Serial.print("BMP size: ");
        Serial.println(m_DataCount);
        m_pData = (uint8_t*)alloca(m_DataCount);

        // Here we check if we need to take into account row padding
        if (m_BmpInfoHeader.width % 4 == 0) 
        {
            inp.read((char*)m_pData, m_DataCount);
            m_FileHeader.file_size += m_DataCount;
        }
        else 
        {
            m_RowStride = m_BmpInfoHeader.width * m_BmpInfoHeader.bit_count / 8;
            uint32_t new_stride = make_stride_aligned(4);
            uint32_t padding_row_count = new_stride - m_RowStride;
            uint8_t* padding_row = (uint8_t*)alloca(padding_row_count);

            for (int y = 0; y < m_BmpInfoHeader.height; ++y)
            {
                inp.read((char*)(m_pData + m_RowStride * y), m_RowStride);
                inp.read((char*)padding_row, padding_row_count);
            }
            m_FileHeader.file_size += m_DataCount + m_BmpInfoHeader.height * padding_row_count;
        }
    }
    else 
    {
        Serial.println("Unable to open the input image file.");
        inp.close();
        return;
    }

    inp.close();
    return;
}

// Add 1 to the m_RowStride until it is divisible with align_stride
uint32_t BMP::make_stride_aligned(uint32_t align_stride) 
{
    uint32_t new_stride = m_RowStride;
    while (new_stride % align_stride != 0) 
    {
        new_stride++;
    }
    return new_stride;
}

// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void BMP::check_color_header(BMPColorHeader &bmp_color_header) 
{
    BMPColorHeader expected_color_header;
    if(expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask) 
    {
        Serial.println("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
        return;
    }

    if(expected_color_header.color_space_type != bmp_color_header.color_space_type) 
    {
        Serial.println("Unexpected color space type! The program expects sRGB values");
        return;
    }
}
