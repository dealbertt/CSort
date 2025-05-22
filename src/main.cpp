#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <SDL3/SDL.h>

#define SDL_HINT_NO_SIGNAL_HANDLERS   "SDL_NO_SIGNAL_HANDLERS"
#define WINDOW_WIDTH 1600 
#define WINDOW_HEIGHT 900

// draw | | | |  bars: each bar is width w, separation is w/2
// thus n bars need n * w + (n-1) * w/2 width



SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int initProgram(){
    if(!SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "Error trying to initialize SDL" << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("CSort", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if(window == NULL){
        std::cout << "Error trying to create SDL_Window" << std::endl;
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    std::cout << "Window created successfully!" << std::endl;

    renderer = SDL_CreateRenderer(window, NULL);
    if(renderer == NULL){
        std::cout << "Error while trying to create the renderer" << std::endl;
        return -1;
        
    }
    SDL_RenderClear(renderer);
    std::cout << "Renderer created successfully!" << std::endl;

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    return 0;
}
int main(){

    if(initProgram() == -1){
        std::cout << "Error initializing the components of the program" << std::endl;
        exit(1);
    }

    return 0;
}
