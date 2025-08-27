#include "encode.h"
#include <SDL2/SDL_render.h>



SDL_Texture* CreateImageAsTexture(SDL_Renderer *renderer, zsFormt img);

void Zoom(SDL_Rect* textureRect, int imageWidht, int imageHeight, float zoom, SDL_Renderer* renderer);

void MoveImage(SDL_Rect* textureRect, int offsetX, int offsetY);

void Viewer(zsFormt img);
