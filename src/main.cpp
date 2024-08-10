#include "screenmanager.h"
#include "encoder.h"
#include "config.h"
#include "fpng.h"

void saveImage(void *_data, int *rect, int _stride);
void saveBmp(unsigned char *_data, int _w, int _h, int _stride, bool use32bpp);
void savePng(char *_data, int _w, int _h, int _stride);

void saveBmp(unsigned char *_data, int _w, int _h, int _stride, bool use32bpp)
{
    size_t fsize = 0;

    unsigned char *img = use32bpp 
    ? Encoder::encodeBmp32(fsize, _data, _w, _h, _stride)
    : Encoder::encodeBmp24(fsize, _data, _w, _h, _stride);

    auto path = Config::instance()->getFileName("bmp");  

    FILE *f = fopen(path.c_str(), "wb");
    if (!f) {
        printf("Failed opening file: %s\n", path.c_str());
        std::free(img);
        return;
    }

    fwrite(img, fsize, 1, f);
    fclose(f);

    std::free(img);
    
    return;
}

void savePng(char *_data, int _w, int _h, int _stride)
{
    char *img = (char *)std::malloc(3  * _w * _h);

    for (int y = 0; y < _h; ++y) {
        memcpy(img + y * _w * 3, _data + y * _stride, _w * 3);
    }

    auto path = Config::instance()->getFileName("png");  

    if (!fpng::fpng_encode_image_to_file(path.c_str(), img, _w, _h, 3)) {
        printf("Error encoding to file\n");
    }

    std::free(img);
}

void saveImage(void *_data, int *rect, int _stride)
{
    int x = rect[0];
    int y = rect[1];
    int w = rect[2];
    int h = rect[3];

    if (Config::instance()->getImageType() == Config::ImageType::PNG) {
        savePng((char *)_data, w, h, _stride);
    } else if (Config::instance()->getImageType() == Config::ImageType::BMP32) {
        saveBmp((unsigned char *)_data, w, h, _stride, true);
    } else {
        saveBmp((unsigned char *)_data, w, h, _stride, false);
    }
}

int main(int argc, char *argv[])
{
    const auto config = Config::instance();
    config->parseParameters(argc, argv);

    fpng::fpng_init();
    Screen screen;

    screen.setCaptureCallback(saveImage);

    screen.captureScreen(
        config->getDisplayIndex(),
        config->getX(), config->getY(),
        config->getW(), config->getH(),
        config->getImageType() == Config::ImageType::BMP32 ? SCREEN_FORMAT_RGBA8888 : SCREEN_FORMAT_RGB888
    );

    return 0;
}
