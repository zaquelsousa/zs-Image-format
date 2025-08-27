#include "decoder.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


bool ZsImageReader(char *FilePath, zsFormt *zsImage){
    FILE *file = fopen(FilePath, "rb");

    fread(zsImage->signature, sizeof(char), 8, file);
    fread(&zsImage->Width, sizeof(int), 1, file);
    fread(&zsImage->Heigh, sizeof(int), 1, file);
    fread(&zsImage->size, sizeof(uint32_t), 1, file);

    PixelCompressed *rleData = malloc(zsImage->size * sizeof(PixelCompressed));
    fread(rleData, sizeof(PixelCompressed), zsImage->size, file);

    zsImage->RawPixelData = (Pixel*)malloc((zsImage->Width * zsImage->Heigh) * sizeof(Pixel));

    int pixelIndex = 0;
    for(int i=0; i< zsImage->size; i++){
        for(int j=0; j< rleData[i].acc; j++){
            if(pixelIndex >= zsImage->Width * zsImage->Heigh){
                fprintf(stderr, "erro indice de pixel gora do limit\n");
                break;
            }

            zsImage->RawPixelData[pixelIndex].R =  rleData[i].rlePixelArr.R;
            zsImage->RawPixelData[pixelIndex].G =  rleData[i].rlePixelArr.G;
            zsImage->RawPixelData[pixelIndex].B =  rleData[i].rlePixelArr.B;
            zsImage->RawPixelData[pixelIndex].A =  255;

            pixelIndex++;
        }
    }
    
    fclose(file);
    return true;
}
