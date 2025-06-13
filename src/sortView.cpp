#include <chrono>
#include <climits>
#include <cstdint>
#include <iostream>
#include <thread>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_keyboard.h>

#include "../include/sortView.hpp"

const int MAX_DELAY = 1000000;
extern ViewObject *globalObject;
extern SDL_Renderer *renderer;
extern SDL_Window *window;
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
        SDL_RenderFillRectF(&renderer, &rect);

         
        x += wbar + spacing;
    }

    array.MtxArray.unlock();

    SDL_RenderPresent(&renderer);
}

void ViewObject::executeSort(void (*func)(class Array&)){
    std::thread sortThread(func, std::ref(array));
    sortThread.detach();

    SDL_Event event;
    while(!array.isSorted()){
        while(SDL_PollEvent(&event)) handleKeyboard(event);
        if(array.needRepaint){
            paint();
            array.needRepaint = false;
        }
    }
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
    const int target = 1000;
    for(size_t i = 0; i < array.getSize(); i++){
        array.sortDelay->setDelay((target / array.getSize()) * 1000);
        array.markDone(i);
        paint();
        array[i].onAccess();
        array.sortDelay->delay();
    }
}

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

int ViewObject::handleKeyboard(SDL_Event &event){

    if(event.type == SDL_KEYDOWN){
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:{
                                     int delay = array.sortDelay->getDuration();
                                     if(delay < MAX_DELAY){
                                         delay += 1000;
                                         array.sortDelay->setDelay(delay);
                                         std::cout << "Duration increased to: " << array.sortDelay->getDuration() << std::endl;
                                     }else{
                                         std::cout << "Max delay value reached!" << std::endl;
                                     }
                                     break;
                                 }
            case SDL_SCANCODE_DOWN:{
                                       int delay = array.sortDelay->getDuration();
                                       delay -= 1000;
                                       array.sortDelay->setDelay(delay);
                                       std::cout << "Duration decreased to: " << array.sortDelay->getDuration() << std::endl;
                                       break;
                                   } 
            case SDL_SCANCODE_ESCAPE:{

                                         cleanUp();
                                         break;
                                     }
            default:
                                     break;
        }
    }

    return 0;

}

void cleanUp(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_PauseAudio(1);
    SDL_CloseAudio();
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
