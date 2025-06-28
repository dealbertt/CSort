#include <SDL3_ttf/SDL_ttf.h>
#include <cstdio>
#include <iostream>
#include <csignal>
#include <print>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_error.h>

#include "../include/config.hpp" 
#include "../include/array.hpp" 
#include "../include/sortView.hpp" 
#include "../include/sound.hpp" 

//TODO: Allow window resizing, define event to resize the array and text aswell
//TODO: Fix comparison on Insertion Sort (or maybe in general, not sure if they are correct)
//TODO: Allow user to control volume throuhg shortcuts

Config config;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *font = nullptr;

SDL_AudioDeviceID gAudioDevice = 0;
SDL_AudioStream *gAudioStream = nullptr;

ViewObject *globalObject = nullptr;
void signalHandler(int signum);
void cleanUp();

int initProgram(){
    signal(SIGINT, signalHandler);
    std::cout << "Inside initProgram!" << std::endl;

    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        std::cerr << "Error trying to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    config = *readConfiguration("config/config.txt");
    

    if(!SDL_CreateWindowAndRenderer("CSort", config.windowWidth, config.windowHeigth,SDL_WINDOW_VULKAN, &window, &renderer)){
        std::cout << "Error on SDL_CreateWindowAndRenderer: " << SDL_GetError() << std::endl;
    }
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

    /*
    int numDevices = SDL_GetNumAudioDevices(0);

    if(config.debug) std::cout << "[DEBUG] Number of audio playback devices: " << numDevices << std::endl;

    for (int i = 0; i < numDevices; i++) {
        const char* device_name = SDL_GetAudioDeviceName(i, 0);
        if(config.debug) std::cout << "[DEBUG] Audio device " << i << ": " << device_name << std::endl;
    }
    */

    SDL_AudioSpec desired;
    desired.freq = 44100;
    desired.format = SDL_AUDIO_S16;
    desired.channels = 1;
    //desired.samples = 4096;
    //desired.callback = SoundCallBack;
    //desired.userdata = nullptr;


    gAudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, AudioStreamNotificationCallback, nullptr);
    if(!gAudioStream){
        std::cout << "Error on gAudioStream: " << SDL_GetError() << std::endl;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return -1;
    }
    gAudioDevice = SDL_GetAudioStreamDevice(gAudioStream);
    if(gAudioDevice == 0){
        SDL_Log("Could not find an SDL_AudioDeviceID for the stream");
        return -1;
    }
    std::cout << "gAudioDeviceID:" << gAudioDevice << std::endl;
    SDL_ResumeAudioDevice(gAudioDevice);

    /*
    if(SDL_OpenAudio(&desired, &obtained) < 0){
        std::cerr << "Error opening SDL_Audio: " << SDL_GetError() << std::endl;
        return -1;
    }
    */


    //SDL_PauseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);

    if(!TTF_Init()){
        std::cout << "Error on TTF_Init: " << SDL_GetError();
        return -1;
    }

    int fontSize = 20;

    std::string fontPath = "fonts/FiraCodeNerdFont-Regular.ttf";

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if(font == NULL){
        std::cout << "Error trying to open the font: " << SDL_GetError() << std::endl;
        return -1;
    }
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
    //SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

