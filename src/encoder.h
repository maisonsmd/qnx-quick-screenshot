#ifndef ENCODER_H
#define ENCODER_H

#include <cstring>
#include <cstdlib>

namespace Encoder {
    /**
     * @brief  Encode the raw data into BMP format
     *          Requires the caller to free the buffer
     */
    unsigned char *encodeBmp24(size_t &_file_size, const unsigned char *_data, int _w, int _h, int _stride);

    /**
     * @brief  Similar to encodeBmp24, but return 32-bit BMP
     */
    unsigned char *encodeBmp32(size_t &_file_size, const unsigned char *_data, int _w, int _h, int _stride);
}

#endif // ENCODER_H
