#include <chrono>
#include <climits>
#include <cstdint>
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

//List containing all the implemented algorithms, incluiding the delay after each swap, a description, and the amount of elements to sort
const struct Algorithm algoList[] = {
    {"Bubble Sort", &BubbleSort,  100, "Bien", 8000}, //8 ms
    {"Cocktail Sort", &CocktailSort,  100, "que tal", 8000}, // 8 ms
    {"Insertion Sort", &InsertionSort,  100, "Hola", 12000}, // 12 ms
    {"Selection Sort", &SelectionSort,  500, "Hola", 50000}, //50 ms
    {"Quick Sort", &QuickSortInit,  5000, "Bien", 5000}, // 5 ms
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
        SDL_FRect rect = {x, static_cast<float>(config.windowHeigth - array.getItemConst(i).getValue()), wbar, static_cast<float>(array.getItemConst(i).getValue())};
        
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

//Function in charge of launching the sorting logic thread
void ViewObject::executeSort(void (*func)(class Array&)){
    std::thread sortThread(func, std::ref(array));
    sortThread.detach();

    textNeedsUpdate = true;
    while(!array.isSorted()){
        handleKeyboard();
        if(array.needRepaint){
            paint();
            array.needRepaint = false;
        }
    }
    finishArray();
    markArrayDone();
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

void ViewObject::markArrayDone(){
    textNeedsUpdate = false;
    const int target = 1000;
    for(index = 0; index < array.getSize(); index++){
        array.sortDelay->setDelay((target / array.getSize()) * 1000);
        array.markDone(index);
        paint();
        array[index].onAccess();
        array.sortDelay->delay();
    }
}

//This function goes throuhg each item of the algoList, creating a new ViewObject, which then creates a new array with the specified size
void runList(SDL_Renderer *renderer){
    ViewObject *object = nullptr;
    for(size_t i = 0; i < algoListSize; i++){

        std::cout << "Iteration of runList: " << i << std::endl;

        object = new ViewObject(algoList[i].maxSize, config.windowHeigth, *renderer);
        object->array.sortDelay->setDelay(algoList[i].delay);

        globalObject = object;
        object->paint();
        object->executeSort(algoList[i].func);

        delete object;
        globalObject = nullptr;

    }
}

//Function in charge of all the keyboard shortcuts, for now there are not that many, hopefully will add more in the future
int ViewObject::handleKeyboard(){

    SDL_Event event;
    SDL_PollEvent(&event);

    const bool *pressed = SDL_GetKeyboardState(NULL);
    if(event.type == SDL_EVENT_KEY_DOWN){
        if(pressed[SDL_SCANCODE_UP]){
            int delay = array.sortDelay->getDuration();
            if(delay < MAX_DELAY){
                delay += 1000;
                array.sortDelay->setDelay(delay);
                std::cout << "Duration increased to: " << array.sortDelay->getDuration() << std::endl;
            }else{
                std::cout << "Max delay value reached!" << std::endl;
            }

        }

        if(pressed[SDL_SCANCODE_DOWN]){
            int delay = array.sortDelay->getDuration();
            delay -= 1000;
            array.sortDelay->setDelay(delay);
            std::cout << "Duration decreased to: " << array.sortDelay->getDuration() << std::endl;
        } 

        if(pressed[SDL_SCANCODE_ESCAPE]){
            cleanUp();
        }
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
    std::string strAccesses = "Accesses:" + std::to_string(accessesCount);
    std::string strName = algoList[index].name; 

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
    NameRect.x = (static_cast<float>(config.windowWidth) / 2) - 200;
    NameRect.y = 0;
    NameRect.w = 400;
    NameRect.h = 50;

    SDL_RenderTexture(&renderer, NameTexture, NULL, &NameRect);
    MtxAccess.unlock();

    SDL_DestroyTexture(NameTexture);
    SDL_DestroySurface(NameSurface);

    return 0;
}

