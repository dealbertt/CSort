#include "../include/config.hpp"
#include <SDL3/SDL_log.h>

Config *readConfiguration(const char *path){
    FILE *ptr = fopen(path, "r");
    if(ptr == NULL){
        perror("Error while trying to open the config file");
        exit(1);
    }
    Config *config = (Config *)malloc(sizeof(Config));
    char line[100];
    while(fgets(line, sizeof(line), ptr)){
        if(line[0] == '#' || strlen(line) < 3) continue; 
        if(strstr(line, "NUMBER_ELEMENTS")) sscanf(line, "NUMBER_ELEMENTS=%d", &config->numberElements);
        else if(strstr(line, "WINDOW_WIDTH")) sscanf(line, "WINDOW_WIDTH=%d", &config->windowWidth);
        else if(strstr(line, "WINDOW_HEIGHT")) sscanf(line, "WINDOW_HEIGHT=%d", &config->windowHeigth);
        else if(strstr(line, "DELAY")) sscanf(line, "DELAY=%d", &config->delay);
        else if(strstr(line, "VOLUME")) sscanf(line, "VOLUME=%d", &config->volume);
        else if(strstr(line, "DEBUG")) sscanf(line, "DEBUG=%d", &config->debug);
    }
    fclose(ptr);
    //std::cout << "Number of elements: " << config->numberElements << "\n";
    SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Number of elements: %d\n", config->numberElements);
    //std::cout << "Window Width: " << config->windowWidth << "\n";
    SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Window Width: %d\n", config->windowWidth);
    //std::cout << "Window Height: " << config->windowHeigth << "\n";
    SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Window Height: %d\n", config->windowHeigth);
    //std::cout << "Volume: " << config->volume << "\n";
    SDL_LogInfo(SDL_LOG_PRIORITY_INFO, "Volume: %d\n", config->volume);
    if(config->volume > 24000 || config->volume < 0) config->volume = 24000;

    return config;
}

void debug(std::string message, Config *config){
    if(config->debug){
        std::cout << "[DEBUG] " << message << std::endl;
    }
}
