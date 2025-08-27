#include "encode.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


bool ReadFileImage(zsFormt *image, char *imagePath){
    unsigned char *Img = stbi_load(imagePath, &image->Width, &image->Heigh, (int*)&image->channels, 0);

    image->RawPixelData = (Pixel*) malloc((image->Width * image->Heigh) * sizeof(Pixel));
    if(image->RawPixelData == NULL){
        printf("fuuuck molloc for rawPixelData fail");
        return false;
    }
    
    if(Img == NULL){
        return false;
    }
    
    for(int i=0; i < image->Width * image->Heigh; i++){
        image->RawPixelData[i].R = Img[i * image->channels + 0];
        image->RawPixelData[i].G = Img[i * image->channels + 1];
        image->RawPixelData[i].B = Img[i * image->channels + 2];
        if(image->channels  == 4){
            image->RawPixelData[i].A = Img[i * image->channels + 3];
        }
        else{
            image->RawPixelData[i].A = 255;
        }
    }
    stbi_image_free(Img);
    return true;
}


bool RleCompression(zsFormt *image, PixelCompressed **rleArr, int lossy){
    *rleArr = NULL;

    PixelCompressed *pixelsCompressed = NULL;
    uint32_t size = 0;
    uint32_t accumulator = 1;

    int length = image->Width * image->Heigh;

    for(int i=0; i < length - 1; i++){
        pixelNoalfa current, next;
        current.R = image->RawPixelData[i].R;
        current.G = image->RawPixelData[i].G;
        current.B = image->RawPixelData[i].B;

        next.R = image->RawPixelData[i+1].R;
        next.G = image->RawPixelData[i+1].G;
        next.B = image->RawPixelData[i+1].B;
        
        if(current.R == next.R && current.G == next.G && current.B == next.B){
            accumulator++;
        }
        else{
            pixelsCompressed = realloc(pixelsCompressed, (size + 1) * sizeof(PixelCompressed));
            if(pixelsCompressed == NULL){
                printf("shit could'nt realloc this shit");
                return false;
            }

            pixelsCompressed[size].acc = accumulator;
            pixelsCompressed[size].rlePixelArr = current;

            size++;
            accumulator = 1;
        }
    }
    
        
    pixelsCompressed = realloc(pixelsCompressed, (size + 1) * sizeof(PixelCompressed));
    if(pixelsCompressed == NULL){return false;}
    
    pixelNoalfa lastPixelGroup;
    lastPixelGroup.R = image->RawPixelData[length - 1].R;
    lastPixelGroup.G = image->RawPixelData[length - 1].G;
    lastPixelGroup.B = image->RawPixelData[length - 1].B;

    pixelsCompressed[size].acc = accumulator;
    pixelsCompressed[size].rlePixelArr = lastPixelGroup;
    size++;

    //por que no primeiro RLE o current e next sao criados dentro do for?
    //por que o for começa com int i = 1?
    PixelCompressed *pixelsCompressedWithLoss = NULL;
    uint32_t idx = 0;

    pixelNoalfa current = pixelsCompressed[0].rlePixelArr;
    uint32_t sumR = current.R * pixelsCompressed[0].acc;
    uint32_t sumG = current.G * pixelsCompressed[0].acc;
    uint32_t sumB = current.B * pixelsCompressed[0].acc;
    accumulator = pixelsCompressed[0].acc;

    for(int i = 1; i < size; i++){
        pixelNoalfa next = pixelsCompressed[i].rlePixelArr;

        uint8_t deltaR = abs(current.R - next.R);
        uint8_t deltaG = abs(current.G - next.G);
        uint8_t deltaB = abs(current.B - next.B);

        if(deltaR <= lossy && deltaG <= lossy && deltaB <= lossy){
            sumR += next.R * pixelsCompressed[i].acc;
            sumG += next.G * pixelsCompressed[i].acc;
            sumB += next.B * pixelsCompressed[i].acc;

            accumulator += pixelsCompressed[i].acc;
        }
        else{
            pixelsCompressedWithLoss = realloc(pixelsCompressedWithLoss, (idx + 1) * sizeof(PixelCompressed));
            if(pixelsCompressedWithLoss == NULL){return false;}

            pixelNoalfa avg;
            avg.R = sumR / accumulator;
            avg.G = sumG / accumulator;
            avg.B = sumB / accumulator;

            pixelsCompressedWithLoss[idx].acc = accumulator;
            pixelsCompressedWithLoss[idx].rlePixelArr = avg;
            idx++;

            sumR = next.R * pixelsCompressed[i].acc;
            sumG = next.G * pixelsCompressed[i].acc;
            sumB = next.B * pixelsCompressed[i].acc;

            current = next;
            accumulator = pixelsCompressed[i].acc;
        }
    }
    
    pixelsCompressedWithLoss = realloc(pixelsCompressedWithLoss, (idx + 1) * sizeof(PixelCompressed));
    if(pixelsCompressedWithLoss == NULL){return false;}

    pixelNoalfa avg;
    avg.R = sumR / accumulator;
    avg.G = sumG / accumulator;
    avg.B = sumB / accumulator;

    pixelsCompressedWithLoss[idx].acc = accumulator;
    pixelsCompressedWithLoss[idx].rlePixelArr = avg;
    idx++;
    
    free(pixelsCompressed);
    *rleArr = pixelsCompressedWithLoss;
    image->size = idx;
    return true;
}

bool WriteFileImage(zsFormt *image, PixelCompressed *rleArr, char fileName[]){
    strcpy(image->signature, "ZsImage");
    //char fileName[50];
    strcpy(fileName,  fileName);
    char *dot = strrchr(fileName, '.');
    if(dot != NULL){
        *dot = '\0';
    }
    
    strcat(fileName, ".zs");
    FILE *file = fopen(fileName, "wb");

    fwrite(&image->signature, sizeof(char), 8, file);
    fwrite(&image->Width, sizeof(int), 1, file);
    fwrite(&image->Heigh, sizeof(int), 1, file);
    fwrite(&image->size, sizeof(uint32_t), 1, file);
    printf("size: %d", image->size);


    fwrite(rleArr, sizeof(PixelCompressed), image->size, file);

    fclose(file);
    return true;
}
