#include <iostream>
#include <memory>
#include <csignal>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_error.h>

#include "../include/config.hpp" 
#include "../include/array.hpp" 
#include "../include/sorting.hpp" 
#include "../include/sortView.hpp" 


// draw | | | |  bars: each bar is width w, separation is w/2
// thus n bars need n * w + (n-1) * w/2 width

Config config;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

void signalHandler(int signum);
void cleanUp();

int initProgram(){
    signal(SIGINT, signalHandler);

    if(SDL_Init(SDL_INIT_VIDEO)){
        std::cerr << "Error trying to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    config = *readConfiguration("config/config.txt");
    

    window = SDL_CreateWindow("CSort", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.windowWidth, config.windowHeigth,SDL_WINDOW_VULKAN);
    if(window == NULL){
        std::cout << "Error trying to create SDL_Window" << std::endl;
        return -1;
    }
    std::cout << "Window created successfully!" << std::endl;

    renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        std::cout << "Error while trying to create the renderer" << std::endl;
        return -1;
        
    }
    std::cout << "Renderer created successfully!" << std::endl;

    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
    return 0;
}
int main(){

    if(initProgram() == -1){
        std::cout << "Error initializing the components of the program" << std::endl;
        exit(1);
    }

    Array array(config.numberElements, config.windowHeigth);
    array.FillArray();
    BubbleSort(array);

    cleanUp();
    return 0;
}

void signalHandler(int signum){
    (void)signum;
    cleanUp();
}

void cleanUp(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
}
