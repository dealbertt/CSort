#include <iostream>
#include <csignal>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_error.h>

#include "../include/config.hpp" 
#include "../include/array.hpp" 
#include "../include/sortView.hpp" 

Config config;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

ViewObject *globalObject = nullptr;
void signalHandler(int signum);
void cleanUp();

int initProgram(){
    signal(SIGINT, signalHandler);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        std::cerr << "Error trying to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    config = *readConfiguration("config/config.txt");
    

    SDL_CreateWindowAndRenderer("CSort", config.windowWidth, config.windowHeigth,SDL_WINDOW_VULKAN, &window, &renderer);
    if(window == NULL){
        std::cerr << "Error trying to create SDL_Window: " << SDL_GetError() << std::endl;
        return -1;
    }else{
        std::cout << "Window created successfully!" << std::endl;
    }

    if(renderer == NULL){
        std::cerr << "Error while trying to create the renderer: " << SDL_GetError() << std::endl;
        return -1;
    }else{
        std::cout << "Renderer created successfully!" << std::endl;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_RenderPresent(renderer);

    int numDrivers = SDL_GetNumAudioDrivers();
    if(config.debug) std::cout << "[DEBUG] Available audio drivers: " << numDrivers << std::endl;

    for (int i = 0; i < numDrivers; i++) {
        if(config.debug) std::cout << "[DEBUG] Driver " << i << ": " << SDL_GetAudioDriver(i) << std::endl;
    }

    const char* currentDriver = SDL_GetCurrentAudioDriver(); 
    if (currentDriver) {
        if(config.debug) std::cout << "[DEBUG] Current audio driver: " << currentDriver << std::endl;
    } else {
        if(config.debug) std::cout << "[DEBUG] No audio driver initialized" << std::endl;
    }

    int numDevices = SDL_GetNumAudioDevices(0);

    if(config.debug) std::cout << "[DEBUG] Number of audio playback devices: " << numDevices << std::endl;

    for (int i = 0; i < numDevices; i++) {
        const char* device_name = SDL_GetAudioDeviceName(i, 0);
        if(config.debug) std::cout << "[DEBUG] Audio device " << i << ": " << device_name << std::endl;
    }

    SDL_AudioSpec desired, obtained;
    desired.freq = 44100;
    desired.format = SDL_AUDIO_S16;
    desired.channels = 1;
    //desired.samples = 4096;
    //desired.callback = SoundCallBack;
    //desired.userdata = nullptr;

    /*
    if(SDL_OpenAudio(&desired, &obtained) < 0){
        std::cerr << "Error opening SDL_Audio: " << SDL_GetError() << std::endl;
        return -1;
    }
    */
    SDL_OpenAudioDeviceStream(SDL_AudioDeviceID devid, const SDL_AudioSpec *spec, SDL_AudioStreamCallback callback, void *userdata)

    if(config.debug){
        std::cout << "[DEBUG] Desired: freq=" << desired.freq << ", format=" << desired.format 
            << ", channels=" << (int)desired.channels << ", samples=" << desired.samples << std::endl;
        std::cout << "[DEBUG] Obtained: freq=" << obtained.freq << ", format=" << obtained.format 
            << ", channels=" << (int)obtained.channels << ", samples=" << obtained.samples << std::endl;
    }

    SDL_PauseAudio(0);

    return 0;
}


int main(){

    if(initProgram() == -1){
        std::cout << "Error initializing the components of the program" << std::endl;
        exit(1);
    }
    //testAudioWithSimpleTone(); 

    runList(renderer);

    cleanUp();
    return 0;
}

void signalHandler(int signum){
    (void)signum;
    cleanUp();
}

