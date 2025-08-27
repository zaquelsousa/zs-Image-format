#include <stdint.h>
#include <stdbool.h>


#ifndef ENCODE_H
#define ENCODE_H

typedef struct Pixel{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;

}Pixel;

typedef struct{
    char signature[8];
    int Width;
    int Heigh;
    uint8_t channels;
    uint32_t size;
    Pixel *RawPixelData;
}zsFormt;

typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
}pixelNoalfa;

typedef struct{
    uint32_t acc;
    pixelNoalfa rlePixelArr;
}PixelCompressed;


bool ReadFileImage(zsFormt *image, char imagePath[]);
bool RleCompression(zsFormt *image, PixelCompressed **rleArr, int lossy);
bool WriteFileImage(zsFormt *image, PixelCompressed *rleArr, char fileName[]);

#endif
