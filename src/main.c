#include "encode.h"
#include "decoder.h"
#include "imageViewer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  

zsFormt ZsImage;
PixelCompressed *RlePixelData;

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Falta o caminho da imagem, porra! Uso: %s <imagem> [--lossy <nivel>]\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    int level = 5;
    int i = 2;
    while (i < argc) {
        if (strcmp(argv[i], "--lossy") == 0) {
            i++;
            if (i < argc) {
                level = atoi(argv[i]);
            } else {
                printf("Faltou o número depois de --lossy, caralho!\n");
                return 1;
            }
        }
        i++;
    }
    
    //se o arquvo que vier no argv[1] tiver .zs chama o viewer tlgd
    char *ext = strrchr(input, '.');
    if (ext && strcmp(ext, ".zs") == 0) {
        strcpy(ZsImage.signature, "ZsImage");
        ZsImageReader(input, &ZsImage);
        Viewer(ZsImage);
    } else {
        strcpy(ZsImage.signature, "ZsImage");

        if(!ReadFileImage(&ZsImage, input)){return 1;}

        if(!RleCompression(&ZsImage, &RlePixelData, level)){return 1;}

        
        if(!WriteFileImage(&ZsImage, RlePixelData, input)){return 1;}
    }

    return 0;
}

