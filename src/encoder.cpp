#include "encoder.h"

#define GET_BYTE(x, n) ((unsigned char)((x >> (n * 8)) & 0xFF))

// Reference header table: https://en.wikipedia.org/wiki/BMP_file_format#Example_1

unsigned char *Encoder::encodeBmp24(size_t &_file_size, const unsigned char *_data, int _w, int _h, int _stride)
{
    // the length of each row must be a multiplier of 4, so add padding if necessary
    const int padding = (4 - (_w * 3) % 4) % 4;
    int arraySize = (3 * _w + padding) * _h;
    int filesize = 54 + arraySize;

    unsigned char *img = (unsigned char *)malloc(filesize);

    size_t written_size = 0;
    const auto append = [&](const unsigned char *data, int size) {
        memcpy(img + written_size, data, size);
        written_size += size;
    };

    unsigned char bmpFileHeader[14] = {
        'B', 'M',
        GET_BYTE(filesize, 0), GET_BYTE(filesize, 1), GET_BYTE(filesize, 2), GET_BYTE(filesize, 3), // file size
        0, 0, 0, 0, // reserved
        54, 0, 0, 0 // offset to start of pixel array
    };
    unsigned char bmpInfoHeader[40] = {
        40, 0, 0, 0, // header size
        GET_BYTE(_w, 0), GET_BYTE(_w, 1), GET_BYTE(_w, 2), GET_BYTE(_w, 3), // width
        GET_BYTE(_h, 0), GET_BYTE(_h, 1), GET_BYTE(_h, 2), GET_BYTE(_h, 3), // height
        1, 0, // planes
        24, 0, // bits per pixel
        0, 0, 0, 0, // BI_RGB, no pixel array compression used
        GET_BYTE(arraySize, 0), GET_BYTE(arraySize, 1), GET_BYTE(arraySize, 2), GET_BYTE(arraySize, 3), // size of pixel array
        0x13, 0x0B, 0, 0, // 2835 pixels/meter or 72 DPI
        0x13, 0x0B, 0, 0, // 2835 pixels/meter or 72 DPI
        0, 0, 0, 0, // colors in palette
        0, 0, 0, 0 // all colors are important
    };
    unsigned char bmpPad[3] = {0, 0, 0};

    append(bmpFileHeader, sizeof(bmpFileHeader));
    append(bmpInfoHeader, sizeof(bmpInfoHeader));

    for (int i = _h - 1; i >= 0; --i) {
        append(_data + _stride * i, 3 * _w);
        append(bmpPad, padding);
    }

    _file_size = written_size;
    return img;
}

unsigned char *Encoder::encodeBmp32(size_t &_file_size, const unsigned char *_data, int _w, int _h, int _stride)
{
    // the length of each row must be a multiplier of 4, so add padding if necessary
    const int padding = (4 - (_w * 4) % 4) % 4; // 4 bytes per pixel (RGBA)
    const int arraySize = (4 * _w + padding) * _h;
    int filesize = 122 + arraySize;

    unsigned char *img = (unsigned char *)malloc(filesize);

    size_t written_size = 0;
    const auto append = [&](const unsigned char *data, int size) {
        memcpy(img + written_size, data, size);
        written_size += size;
    };

    unsigned char bmpFileHeader[14] = {
        'B', 'M',
        GET_BYTE(filesize, 0), GET_BYTE(filesize, 1), GET_BYTE(filesize, 2), GET_BYTE(filesize, 3), // file size
        0, 0, 0, 0, // reserved
        122, 0, 0, 0 // offset to start of pixel array
    };


    unsigned char bmpInfoHeader[108] = {
        108, 0, 0, 0, // header size
        GET_BYTE(_w, 0), GET_BYTE(_w, 1), GET_BYTE(_w, 2), GET_BYTE(_w, 3), // width
        GET_BYTE(_h, 0), GET_BYTE(_h, 1), GET_BYTE(_h, 2), GET_BYTE(_h, 3), // height
        1, 0, // planes
        32, 0, // bits per pixel
        3, 0, 0, 0, // BI_BITFIELDS, no pixel array compression used
        GET_BYTE(arraySize, 0), GET_BYTE(arraySize, 1), GET_BYTE(arraySize, 2), GET_BYTE(arraySize, 3), // size of pixel array
        0x13, 0x0B, 0, 0, // 2835 pixels/meter or 72 DPI
        0x13, 0x0B, 0, 0, // 2835 pixels/meter or 72 DPI
        0, 0, 0, 0, // colors in palette
        0, 0, 0, 0, // all colors are important
        0, 0, 0, 0xFF, // red channel bit mask
        0, 0, 0xFF, 0, // green channel bit mask
        0, 0xFF, 0, 0, // blue channel bit mask
        0xFF, 0, 0, 0, // alpha channel bit mask
        0x20, 0x6E, 0x69, 0x57, // LCS_WINDOWS_COLOR_SPACE
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // CIEXYZTRIPLE
        0, 0, 0, 0, // Red Gamma
        0, 0, 0, 0, // Green Gamma
        0, 0, 0, 0, // Blue Gamma
    }; 

    unsigned char bmpPad[3] = {0, 0, 0};

    append(bmpFileHeader, sizeof(bmpFileHeader));
    append(bmpInfoHeader, sizeof(bmpInfoHeader));

    for (int i = _h - 1; i >= 0; --i) {
        append(_data + _stride * i, 4 * _w); // 4 bytes per pixel (ABGR)
        append(bmpPad, padding);
    }

    _file_size = written_size;
    return img;
}
