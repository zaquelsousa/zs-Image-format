#include "imageViewer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>


SDL_Texture* CreateImageAsTexture(SDL_Renderer *renderer, zsFormt img){
    SDL_Texture* texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_BGRA32,
            SDL_TEXTUREACCESS_STREAMING,
            img.Width, img.Heigh);

    Uint32* pixel_Data = malloc(sizeof(Uint32) * img.Width * img.Heigh);
    SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_BGRA32);
    
    for(int i=0; i < img.Width * img.Heigh; i++){
        Pixel p = img.RawPixelData[i];
        pixel_Data[i] = SDL_MapRGBA(format, p.R, p.G, p.B, p.A);
    }

    SDL_UpdateTexture(texture, NULL, pixel_Data, img.Width * sizeof(Uint32));
    return texture;
}

void Zoom(SDL_Rect *textureRect, int imageWidht, int imageHeight, float zoom, SDL_Renderer *renderer){
    textureRect->w = (int)(imageWidht * zoom);
    textureRect->h = (int)(imageHeight * zoom);

    int ScreenW, ScreenH;
    SDL_GetRendererOutputSize(renderer, &ScreenW, &ScreenH);
    textureRect->x = (ScreenW - textureRect->w)/2;
    textureRect->y = (ScreenH - textureRect->h)/2;

}

void MoveImage(SDL_Rect *textureRect, int offsetX, int offsetY){
 //TODO impedir de mover a imagem pra fora do viewer
    textureRect->x +=  offsetX;
    textureRect->y +=  offsetY;
    
}


void Viewer(zsFormt img){


    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
            "SDL PORRA", 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            800, 600,
            0    
            );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 12, 13, 10, 255);

    SDL_Texture* imageTexture = CreateImageAsTexture(renderer, img);
    SDL_Rect imageTransformatorion;

    SDL_RenderCopy(renderer, imageTexture, NULL, &imageTransformatorion);
    SDL_RenderPresent(renderer);


    int running = 1;
    SDL_Event event;

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
                }
            } 
            else if(event.type == SDL_MOUSEBUTTONUP){
                if(event.button.button == SDL_BUTTON_LEFT){
                    isHoldingMouseButton = 0;
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
            Zoom(&imageTransformatorion, img.Width, img.Heigh, zoom, renderer);
            MoveImage(&imageTransformatorion, offsetX, offsetY);
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, imageTexture, NULL, &imageTransformatorion);
        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

