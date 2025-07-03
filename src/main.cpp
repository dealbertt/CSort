#include <SDL3/SDL_log.h>
#include <iostream>
#include <csignal>
#include <getopt.h>
#include <unistd.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_error.h>

#include "../include/config.hpp" 
#include "../include/array.hpp" 
#include "../include/sortView.hpp" 
#include "../include/sound.hpp" 

//TODO: Adjust text to the width of the screen, so when its resized, it adjusts to the new sizes accordingly
Config config;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *font = nullptr;
extern const int MAX_VOLUME;
extern const int64_t MAX_DELAY;

SDL_AudioDeviceID gAudioDevice = 0;
SDL_AudioStream *gAudioStream = nullptr;

ViewObject *globalObject = nullptr;
void signalHandler(int signum);
void cleanUp();
int loadConfig();
int printCommands();


// ------------------------------------------
int loadConfig(){
    config = *readConfiguration("config/config.txt");
    return 0;
}

int printCommands(){
    std::cout << "--run / --r : Run command to execute the default list of algorithms" << std::endl;
    std::cout << "--delay / --d : Adjusts the delay after each swap happens in the algorithm, in order to be able to see it. Please input the delay in milliseconds" << std::endl;
    std::cout << "--volume / --v : You can adjust the volume, only accepts positive numbers up to 24000" << std::endl;
    std::cout << "--elements / --e : The total amount of elements in the arrays, this number will be the same across all algorithms" << std::endl;
    std::cout << "--list / --l : Lists all of the algorithms implemented so far, which are in a default list" << std::endl;
    return 0;
}
int initProgram(){
    signal(SIGINT, signalHandler);

    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        //std::cerr << "Error trying to initialize SDL: " << SDL_GetError() << std::endl;
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Error trying to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    

    if(!SDL_CreateWindowAndRenderer("CSort", config.windowWidth, config.windowHeigth, SDL_WINDOW_RESIZABLE, &window, &renderer)){
        //std::cout << "Error on SDL_CreateWindowAndRenderer: " << SDL_GetError() << std::endl;
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Error on SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    }
    if(window == NULL){
        //std::cerr << "Error trying to create SDL_Window: " << SDL_GetError() << std::endl;
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Error trying to create SDL_Window: %s\n", SDL_GetError());
        return -1;
    }else{
        SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Window created successfully!");
    }

    if(renderer == NULL){
        //std::cerr << "Error while trying to create the renderer: " << SDL_GetError() << std::endl;
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Error while trying to create renderer: %s\n", SDL_GetError());
        return -1;
    }else{
        //std::cout << "Renderer created successfully!" << std::endl;
        SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Renderer created successfully!");
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    //SDL_RenderPresent(renderer);

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
    //std::cout << "gAudioDeviceID:" << gAudioDevice << std::endl;
    SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "gAudioDeviceID: %d\n", gAudioDevice);
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

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);
    return 0;
}


int main(int argc, char *argv[]){
    loadConfig();

    if(initProgram() == -1){
        std::cout << "Error initializing the components of the program" << std::endl;
        exit(1);
    }
    static struct option long_options[] = {
        {"run", no_argument, 0, 'r'},
        {"delay", required_argument, 0, 'd'},
        {"volume", required_argument, 0, 'v'},
        {"elements", required_argument, 0, 'e'},
        {"list", no_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
         {0, 0, 0, 0}
    };

    int longIndex = 0;
    int opt = 0;
    while((opt = getopt_long(argc, argv, "r:d:v:e", long_options, &longIndex)) != -1){
        switch (opt) {
            case 'r':              
                std::cout << "Command of type run!" << std::endl;
                runList(renderer);
                break;

            case 'd':
                if(atoi(optarg) <= 0 || atoi(optarg) > MAX_DELAY / 1000){
                    std::cout << "Wrong number, falling back to default of each algoritm" << std::endl;
                    break;
                }
                config.delay = atoi(optarg) * 1000; //Convert from milliseconds to microseconds
                std::cout << "Command of type delay!" << std::endl;
                break;

            case 'v':
                if(atoi(optarg) <= 0 || atoi(optarg) > MAX_VOLUME){
                    std::cout << "Wrong number, falling back to default of: " << config.volume << std::endl;
                }else{
                    config.volume = atoi(optarg);
                }
                std::cout << "Command of type volume!" << std::endl;
                break;

            case 'e':
                if(atoi(optarg) == 0){
                    std::cout << "Please introduce a valid number!" << std::endl;
                    std::cout << "No amount specified, falling back to the defualt amount of each algorithm" << std::endl;
                    break;
                }

                config.numberElements = atoi(optarg);
                std::cout << "Command of type elements!" << std::endl;
                break;

            case 'l':
                printList();
                return 1;

            case 'h':
                printCommands();
                break;

        }
    }


    //testAudioWithSimpleTone(); 


    cleanUp();
    return 0;
}

void signalHandler(int signum){
    (void)signum;
    cleanUp();
    //SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

