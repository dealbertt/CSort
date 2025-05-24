#include <iostream>
#include <memory>
#include <csignal>
#include <thread>
#include <functional>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

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

    if(!SDL_Init(SDL_INIT_VIDEO)){
        std::cout << "Error trying to initialize SDL" << std::endl;
        return -1;
    }

    config = *readConfiguration("config/config.txt");
    
    std::cout << "Window Width: " << config.windowWidth << std::endl;
    std::cout << "Window Height: " << config.windowHeigth << std::endl;


    SDL_CreateWindowAndRenderer("CSort", config.windowWidth, config.windowHeigth, 0, &window, &renderer);
    if(window == NULL){
        std::cout << "Error trying to create SDL_Window" << std::endl;
        return -1;
    }
    std::cout << "Window created successfully!" << std::endl;

    if(renderer == NULL){
        std::cout << "Error while trying to create the renderer" << std::endl;
        return -1;
        
    }
    std::cout << "Renderer created successfully!" << std::endl;

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
    return 0;
}
int main(){

    if(initProgram() == -1){
        std::cout << "Error initializing the components of the program" << std::endl;
        exit(1);
    }

    ViewObject object; 

    object.sArray = std::make_unique<Array>(config.numberElements, config.windowHeigth);
    object.executeSort();
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
