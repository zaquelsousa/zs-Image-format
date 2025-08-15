#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>


#include <strings.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"



struct Pixel {
    Uint8 R;
    Uint8 G;
    Uint8 B;
    Uint8 A;
};

struct zsFormt {
    char signature[8];
    int Width;
    int Height;
    //pointer for struct pixel its needed because we are doing a dynamic arr of pixels
    //im going to need to do molloc 😰😰😰😰😰😰
    //remember this is a var that its type points to struct pixel, bro this shit is hard as fuuuk
    struct Pixel *PixelValue;
};


SDL_Texture* CreateTexture(SDL_Renderer *renderer, struct zsFormt img) {
    SDL_Texture* texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            img.Width, img.Height
            );

    Uint32* pixel_data = malloc(sizeof(Uint32) * img.Width * img.Height);
    SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

    for (int y = 0; y < img.Height; y++) {
        for (int x = 0; x < img.Width; x++) {
            int idx = y * img.Width + x;
            struct Pixel p = img.PixelValue[idx];

            //precisso pesquisar melhor sa porra de casting de ponteiros
            //Uint8* pixel_bytes = (Uint8* ) &pixel_data[idx];
            //pixel_bytes[0] = p.R;
            //pixel_bytes[1] = p.G;
            //pixel_bytes[2] = p.B;
            //pixel_bytes[3] = p.A;
            //printf("index of pixel: %d\n", idx);
            //printf("R: %d, G: %d, B: %d\n", p.R, p.G, p.B);
            //
            pixel_data[idx] = SDL_MapRGBA(format, p.R, p.G, p.B, p.A);
        }
    }
    SDL_UpdateTexture(texture, NULL, pixel_data, img.Width * sizeof(Uint32));


    return texture;
}


void Zoom(SDL_Renderer* renderer, SDL_Texture* texture,  int w, int h, float zoom, int sw, int sh){
    
   SDL_Rect Rect;
   Rect.w = (int)(w * zoom);
   Rect.h = (int)(h * zoom);

   int Sw, Sh;
   SDL_GetRendererOutputSize(renderer, &Sw, &Sh);
   
   //centrraliza sa porra na tela
   Rect.x = (Sw - Rect.w) /2;
   Rect.y = (Sh - Rect.h) /2;
   
   //move ela pela tela
   Rect.x += sw;
   Rect.y += sh;

   SDL_RenderCopy(renderer, texture, NULL ,&Rect);
}


int main(int argc, char *argv[]){
    struct zsFormt image;
    if (argc < 2){
        printf("need to pass a image as argument");
        return 1;
    }
    
    //se .zs for nulo entao quer dizeer que recebos um outro formato de imagem entao bora converter sa porra
    if(strstr(argv[1], ".zs") == NULL){
        strcpy(image.signature, "zsImage");
        int channels;
        unsigned char *img = stbi_load(argv[1], &image.Width, &image.Height, &channels, 0);
        image.PixelValue = (struct Pixel*)malloc((image.Width * image.Height) * sizeof(struct Pixel));

        if(img == NULL){
            printf("erro na hora de carregar sa porra de imagem\n");
            exit(1);
        }
        for (int i=0; i < image.Height; i++){
            for (int j=0; j < image.Width; j++){
                int idx = i * image.Width + j;
                //passando os valores de RGB uhhhh
                image.PixelValue[idx].R = img[idx * channels + 0]; 
                image.PixelValue[idx].G = img[idx * channels + 1]; 
                image.PixelValue[idx].B = img[idx * channels + 2];
                //kkkkk eu nao execultaria esses print ai pra imagens grande nao em adskasdkkdsa
                //printf("index of pixel: %d\n", idx);
                //printf("R: %d, G: %d, B: %d\n", image.PixelValue[idx].R ,image.PixelValue[idx].G, image.PixelValue[idx].B);
                if(channels == 4){
                    image.PixelValue[idx].A = img[idx * channels + 3]; 
                    //printf("canal alfa dessa miseria: %d\n", img[idx * channels + 3]);
                }
                else{
                    //printf("essa imagem nao tem alfa channel");
                    image.PixelValue[idx].A = 255; 
                }
            }
        }

        char fileName[50];
        strcpy(fileName,  argv[1]);
        char fileFormat[4] = ".zs";
        strcat(fileName, fileFormat);
        FILE *file = fopen(fileName, "wb");//o w quer dizer que o arquyivo esta aberto para escrita e o b indica queo arquivo svai ser um binario
                                              //tem que passar o endereco do que vai ser escrito
        fwrite(&image.signature, sizeof(char), 8, file);
        fwrite(&image.Width, sizeof(int), 1, file);
        fwrite(&image.Height, sizeof(int), 1, file);
        //this mdf no need to & because he already are a ointer
        fwrite(image.PixelValue, sizeof(struct Pixel), image.Width * image.Height, file);//image.Width * image.Height sa porra ai é a quantidade de bytes que vai ter nessa porra



        //tem que libverar a memoria que aloca com malloc se nao explode tudo
        free(image.PixelValue);
    }
    else {
        //tem que abrir o arquivo ne paizao
        FILE *file = fopen(argv[1], "rb");//rb de read binay
                                          
        struct zsFormt img;

        //sa parada ai le  o arquivo na ordem que os bits esta organizado peloq ue ue enmtendi, ai tipo
        //se passar 8 bytes ele vai ler adivinnha 8 bytes
        fread(img.signature, sizeof(char), 8, file);
        fread(&img.Width, sizeof(int), 1, file);
        fread(&img.Height, sizeof(int), 1, file);

        printf("Signature: %.8s\n", img.signature);
        printf("value of img.W: %d\n", img.Width);
        printf("value of img.H: %d\n", img.Height);

        img.PixelValue = (struct Pixel*)malloc((img.Width * img.Height) * sizeof(struct Pixel));
        fread(img.PixelValue, sizeof(struct Pixel), img.Width * img.Height, file);
        /*
        for (int i=0; i < 9; i++){
            printf("img.PixelValue[%d].R: %d\n", i, img.PixelValue[i].R);
            printf("img.PixelValue[%d].G: %d\n", i, img.PixelValue[i].G);
            printf("img.PixelValue[%d].B: %d\n", i, img.PixelValue[i].B);
        }*/
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window* window = SDL_CreateWindow("SDL PORRA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,SDL_WINDOW_RESIZABLE);
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        int running = 1;
        SDL_Event event;

        
        SDL_Texture* texture = CreateTexture(renderer, img);


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        float zoom = 1.0f;
        int isHoldingMouseButton = 0;
        int lastMouseX, lastMouseY;
        int offsetX = 0; int offsetY = 0;

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
                else if(event.type == SDL_MOUSEWHEEL){
                    if(event.wheel.y > 0) {
                        zoom /= 1.1f;
                    }
                    else if (event.wheel.y < 0) {
                        zoom *= 1.1f;
                    }
                }
                else if(event.type == SDL_MOUSEBUTTONDOWN){
                    if(event.button.button == SDL_BUTTON_LEFT){
                       isHoldingMouseButton = 1;
                       lastMouseX = event.button.x;
                       lastMouseY = event.button.y;
                       //printf("mouse down: %d lastMx: %d lastMy: %d\n\n", isHoldingMouseButton, lastMouseX, lastMouseY);
                    }
                } 
                else if(event.type == SDL_MOUSEBUTTONUP){
                    if(event.button.button == SDL_BUTTON_LEFT){
                       isHoldingMouseButton = 0;
                       //printf("solto foi? mouse down: %d\n", isHoldingMouseButton);
                    }
                }
                else if(event.type == SDL_MOUSEMOTION){
                    if(isHoldingMouseButton){
                        int dx = event.motion.x - lastMouseX;
                        int dy = event.motion.y - lastMouseY;

                        offsetX += dx;
                        offsetY += dy;

                        lastMouseX = event.motion.x;
                        lastMouseY = event.motion.y;
                    }
                }

            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            Zoom(renderer, texture, img.Width, img.Height, zoom, offsetX, offsetY);
            SDL_RenderPresent(renderer);//sa porra aqui que mostra as porra na tela
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        free(img.PixelValue);
    }
    return 0;
}



