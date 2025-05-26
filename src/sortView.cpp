#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <thread>

#include "../include/sortView.hpp"
float ViewObject::calculateWidthofBar(size_t size){
    float totalSpacing = spacing * (size - 1);
    float availableWidth = config.windowWidth - totalSpacing;

    return availableWidth / static_cast<float>(size);

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

void ViewObject::executeSort(){
    std::thread sortThread(SelectionSort, std::ref(array));
    sortThread.detach();

    while(!array.isSorted()){
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
    for(size_t i = 0; i < array.getSize(); i++){
        array.markDone(i);
        paint();
        array.sortDelay->setDelay(5000);
        array.sortDelay->delay();
    }
}
