#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <thread>

#include "../include/sortView.hpp"

void ViewObject::paint(){
    size_t size = array.getSize();
    if(size == 0){
        return;
    }

    float width = static_cast<float>(config.windowWidth);
    wbar = (width - (size - 1)) / (double)size;
    if(static_cast<int>(size) >  config.windowWidth){
        wbar = 1;
    }
    float x = 0;

    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderClear(&renderer);

    array.MtxArray.lock(); 
    SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 255);
    for(size_t i = 0; i < size; i++){
        SDL_FRect rect = {x, static_cast<float>(config.windowHeigth - array.getItemConst(i).getValue()), wbar, static_cast<float>(array.getItemConst(i).getValue())};
        SDL_RenderFillRectF(&renderer, &rect);
        x += wbar + 1;
    }

    array.MtxArray.unlock();

    SDL_RenderPresent(&renderer);
}

void ViewObject::executeSort(){
    std::thread sortThread(BubbleSort, std::ref(array));
    sortThread.detach();

    while(!array.isSorted()){
        if(array.needRepaint){
            paint();
            array.needRepaint = false;
        }
    }
}
