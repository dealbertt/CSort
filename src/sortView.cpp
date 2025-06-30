#include <chrono>
#include <csignal>
#include <cstdint>
#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>

#include "../include/sortView.hpp"
#include "../include/sound.hpp"

const int MAX_DELAY = 1000000;
extern ViewObject *globalObject;
extern SDL_Renderer *renderer;
extern SDL_Window *window;
extern TTF_Font *font;

extern std::mutex MtxAccess;


size_t globalIndex = 0;

//List containing all the implemented algorithms, incluiding the delay after each swap, a description, and the amount of elements to sort
const struct Algorithm algoList[] = {
    {"Bubble Sort", &BubbleSort,  100, "Bien", 8000}, //8 ms
    {"Cocktail Sort", &CocktailSort,  100, "que tal", 8000}, // 8 ms
    {"Insertion Sort", &InsertionSort,  100, "Hola", 12000}, // 12 ms
    {"Selection Sort", &SelectionSort,  500, "Hola", 50000}, //50 ms
    {"Quick Sort", &QuickSortInit,  5000, "Bien", 1250}, // 1,25 ms
};

const size_t algoListSize = sizeof(algoList) / sizeof(algoList[0]);
float ViewObject::calculateWidthofBar(size_t size){
    float totalSpacing = spacing * (size - 1);
    float availableWidth = config.windowWidth - totalSpacing;

    wbar = availableWidth / static_cast<float>(size);
    return wbar;
}


//Function in charge of painting the array
void ViewObject::paint(){
    size_t size = array.getSize();
    if(size == 0){
        return;
    }

    wbar = calculateWidthofBar(size);

    float x = 0;

    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderClear(&renderer);

    array.MtxArray.lock(); 
    SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 255);
    for(size_t i = 0; i < size; i++){
        float heigh = (static_cast<float>(array.getItemMutable(i).getValue()) /  static_cast<float>(array.getMaxValue())) * config.windowHeigth;
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "MaxValue: %d", array.getMaxValue());
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Current Value: %d", array.getItemMutable(i).getValue());
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG, "Height: %.2f", heigh);

        SDL_FRect rect = {x, static_cast<float>(config.windowHeigth - heigh), wbar, heigh};

        
        SDL_Color color = configureColor(array.getItemMutable(i));
        SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(&renderer, &rect);

         
        x += wbar + spacing;
    }

    array.MtxArray.unlock();

    if(textNeedsUpdate){
        updateText();
    }
    SDL_RenderPresent(&renderer);
}

SDL_Color ViewObject::configureColor(ArrayItem &item){
    uint8_t colorCode = item.getColor();
    SDL_Color color;
    switch (colorCode) {
        case 0:
            color = {255, 255, 255, 255};
            break;

        case 1:
            color = {255, 0, 0, 255};
            break;

        case 2:
            color = {0, 255, 0, 255};
            break;
        default:
            color = {255, 255, 255, 255};
            
    
    }
    return color;
}
//Function in charge of launching the sorting logic thread
void ViewObject::executeSort(void (*func)(class Array&)){
    std::thread sortThread(func, std::ref(array));
    sortThread.detach();

    textNeedsUpdate = true;
    while(!array.isSorted()){
        handleEvents();
        if(array.isSkipped()){

            break;
        }
        if(array.needRepaint){
            paint();
            array.needRepaint = false;
        }
    }
    if(array.isSorted()){
        finishArray();
        markArrayDone();
    }
}


void ViewObject::markArrayDone(){
    const float target = 1000.0f;
    for(size_t i = 0; i< array.getSize(); i++){
        array.sortDelay->setDelay((target / array.getSize()) * 1000.0f);
        array.markDone(i);
        paint();
        array[i].onAccess();
        array.sortDelay->delay();
    }
}

//This function goes throuhg each item of the algoList, creating a new ViewObject, which then creates a new array with the specified size
void runList(SDL_Renderer *renderer){
    ViewObject *object = nullptr;
    for(globalIndex = 0; globalIndex < algoListSize; globalIndex++){

        std::cout << "Iteration of runList: " << globalIndex << std::endl;

        object = new ViewObject(algoList[globalIndex].maxSize, config.windowHeigth, *renderer);
        object->array.sortDelay->setDelay(algoList[globalIndex].delay);

        globalObject = object;
        object->pressSpaceToContinue();
        object->paint();
        object->executeSort(algoList[globalIndex].func);

        delete object;
        globalObject = nullptr;
        compareCount = 0;
        accessesCount = 0;

    }
}

//Function in charge of handling all SDL_EVENTS such as keyboard events or window events
int ViewObject::handleEvents(){

    SDL_Event event;
    SDL_PollEvent(&event);

    const bool *pressed = SDL_GetKeyboardState(NULL);
    if(event.type == SDL_EVENT_QUIT){
        cleanUp();
    }
    if(event.type == SDL_EVENT_KEY_DOWN){

        if(pressed[SDL_SCANCODE_SPACE]){
            skipAlgorithm();
            return 0;
        }

        if(pressed[SDL_SCANCODE_ESCAPE]){
            cleanUp();
            return 0;
        }

        if(pressed[SDL_SCANCODE_LCTRL] && pressed[SDL_SCANCODE_UP] ){
            if(config.volume < 24000.0){
                config.volume += 1000;
                std::cout << "Vol increased to: " << config.volume << std::endl;
                return 0;
            }else{
                std::cout << "Max volume reached: " << config.volume << std::endl;
                return -1;
            }
        }else if(pressed[SDL_SCANCODE_LCTRL] &&  pressed[SDL_SCANCODE_DOWN]){
            if(config.volume > 0.0){
                config.volume -= 1000;
                std::cout << "Vol decreased to: " << config.volume << std::endl;
                return 0;
            }else{
                std::cout << "Already at 0 volume!" << std::endl;
                return -1;
            }
          } 

        if(pressed[SDL_SCANCODE_UP]){
            int delay = array.sortDelay->getDuration();
            if(delay < MAX_DELAY){
                delay += 1000;
                array.sortDelay->setDelay(delay);
                std::cout << "Duration increased to: " << array.sortDelay->getDuration() << std::endl;
                return 0;
            }else{
                std::cout << "Max delay value reached!" << std::endl;
                return -1;
            }
        }

        if(pressed[SDL_SCANCODE_DOWN]){
            int delay = array.sortDelay->getDuration();
            delay -= 1000;
            array.sortDelay->setDelay(delay);
            std::cout << "Duration decreased to: " << array.sortDelay->getDuration() << std::endl;
            return 0;
        } 
    }else if(event.type == SDL_EVENT_WINDOW_RESIZED){
        config.windowWidth = event.window.data1;
        config.windowHeigth = event.window.data2;
        SDL_LogDebug(SDL_LOG_PRIORITY_DEBUG,"New Width: %d\n New Height: %d", config.windowWidth, config.windowHeigth); 

        //rerender array with the new dimensions


    }

    return 0;

}

//Function for when the program exits, whether because there are no more algorithms to sort of because a signal has been received
void cleanUp(){
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
}


void ViewObject::startArray(){
    array.setSorted(false);
    a = std::chrono::steady_clock::now();
}

void ViewObject::finishArray(){
    b = std::chrono::steady_clock::now();
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>(b - a).count() << std::endl;
}

int ViewObject::updateText(){
    SDL_Color color = {255, 255, 255, 255};

    MtxAccess.lock();
    std::string strComparison = "Comparisons: " + std::to_string(compareCount);
    std::string strAccesses = "Accesses: " + std::to_string(accessesCount);
    std::string strName = algoList[globalIndex].name; 
    std::string strDelay =  "Delay: " +  std::to_string(algoList[globalIndex].delay / 1000.f) + " ms";

    SDL_Surface *NameSurface = TTF_RenderText_Solid(font, strName.c_str(), strName.length(), color);
    if(NameSurface == NULL){
        std::cout << "Error trying to create NameSurface: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *NameTexture = SDL_CreateTextureFromSurface(&renderer, NameSurface);
    if(NameTexture == NULL){
        std::cout << "Error creating NameTexture: " << SDL_GetError() << std::endl;
    }
    SDL_FRect NameRect;
    NameRect.x = 0; 
    NameRect.y = 0;
    NameRect.w = 200;
    NameRect.h = 30;

    SDL_Surface *AccSurface = TTF_RenderText_Solid(font, strAccesses.c_str(), strAccesses.length(), color);
    if(AccSurface == NULL){
        std::cout << "Error creating AccSurface: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *AccTexture = SDL_CreateTextureFromSurface(&renderer, AccSurface);
    if(AccTexture == NULL){
        std::cout << "Error creating AccTexture: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_FRect AccRect;
    AccRect.x = NameRect.x + NameRect.w + 50;
    AccRect.y = 0; 
    AccRect.w = 200; 
    AccRect.h = 30; 

    SDL_Surface *CompSurface = TTF_RenderText_Solid(font, strComparison.c_str(), strComparison.length(), color);
    if(CompSurface == NULL){
        std::cout << "Error creating CompSurface: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *CompTexture = SDL_CreateTextureFromSurface(&renderer, CompSurface);
    if(CompTexture == NULL){
        std::cout << "Error creating CompTexture: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_FRect CompRect;
    CompRect.x = AccRect.x + AccRect.w + 50;
    CompRect.y = 0;
    CompRect.w = 200;
    CompRect.h = 30;

    SDL_Surface *DelaySurface = TTF_RenderText_Solid(font, strDelay.c_str(), strDelay.length(), color);
    if(DelaySurface == NULL){
        std::cout << "Error creating DelaySurface: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *DelayTexture = SDL_CreateTextureFromSurface(&renderer, DelaySurface);
    if(DelayTexture == NULL){
        std::cout << "Error creating DelayTexture: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_FRect DelayRect;
    DelayRect.x = CompRect.x + CompRect.w + 50;
    DelayRect.y = 0;
    DelayRect.w = 200;
    DelayRect.h = 30;


    SDL_RenderTexture(&renderer, NameTexture, NULL, &NameRect);
    SDL_RenderTexture(&renderer, AccTexture, NULL, &AccRect);
    SDL_RenderTexture(&renderer, CompTexture, NULL, &CompRect);
    SDL_RenderTexture(&renderer, DelayTexture, NULL, &DelayRect);
    MtxAccess.unlock();

    SDL_DestroyTexture(NameTexture);
    SDL_DestroySurface(NameSurface);

    SDL_DestroyTexture(AccTexture);
    SDL_DestroySurface(AccSurface);

    SDL_DestroyTexture(CompTexture);
    SDL_DestroySurface(CompSurface);

    SDL_DestroyTexture(DelayTexture);
    SDL_DestroySurface(DelaySurface);
    return 0;
}
int ViewObject::skipAlgorithm(){
    SDL_RenderClear(&renderer);
    finishArray();
    array.setSkipped(true);
    array.clearArray();
    SoundReset();
    kill(getpid(), SIGUSR1);
    return 0;
}

int ViewObject::pressSpaceToContinue(){
    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderClear(&renderer);
    SDL_RenderPresent(&renderer);

    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                std::exit(0);
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                const bool *keys = SDL_GetKeyboardState(nullptr);
                if (keys[SDL_SCANCODE_SPACE]) {
                    waiting = false;
                    resumeAudio();
                    break;
                }
            }
        }
        SDL_Delay(10); // Avoid busy-waiting
    }

    return 0;
}
