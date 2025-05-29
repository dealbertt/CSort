#include <SDL2/SDL_audio.h>
#include <iostream>
#include <csignal>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_mixer.h>

#include "../include/config.hpp" 
#include "../include/array.hpp" 
#include "../include/sorting.hpp" 
#include "../include/sortView.hpp" 
#include "../include/sound.hpp" 


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
    

    SDL_CreateWindowAndRenderer(config.windowWidth, config.windowHeigth,SDL_WINDOW_VULKAN, &window, &renderer);
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

    //Typeshit
    ViewObject object(array, *renderer);

    SDL_AudioSpec audiospec;
    audiospec.freq = 44100;
    audiospec.format = AUDIO_S16SYS;
    audiospec.channels = 1;
    audiospec.samples = 4096;
    audiospec.callback = SoundCallBack;
    audiospec.userdata = &object;


    if(SDL_OpenAudio(&audiospec, NULL) < 0){
        std::cerr << "SDL_OpenAudio failed: " << SDL_GetError() << std::endl;
    }

    runList(renderer);

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
