#include <SDL3/SDL_render.h>
#include <iostream>
#include <thread>
#include "../include/sortView.hpp"
#include "../include/sorting.hpp"
#include "../include/config.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

extern Config config;
extern SDL_Renderer *renderer;
extern SDL_Window *window;

void ViewObject::paint(){
if(sArray->getSize() == 0){
        return;
    } 

    rectArray.clear();
    size_t size = sArray->getSize();
    size_t width = config.windowWidth;
    //size_t heigth = config.windowHeigth;

    

    //float wbar = static_cast<float>(config.windowWidth) / size;
    float wbar = (width - (size - 1)) / (double)size;
    if(static_cast<int>(size) >  config.windowWidth){
        wbar = 1;
    }

    float x = 0;
    
    sArray->MtxArray.lock();
    for(size_t i = 0; i < size; i++){
        SDL_FRect rect = {x, static_cast<float>(config.windowHeigth - sArray->getItemMutable(i).getValue()), wbar, static_cast<float>(sArray->getItemMutable(i).getValue())};
        rectArray.push_back(rect);
        
        x += wbar + 2;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    for (const auto& member : rectArray) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &member);
        //rectsToRender.push_back(member.rect);
    }
    SDL_RenderPresent(renderer);
    sArray->MtxArray.unlock();
}


void ViewObject::executeSort(){
    sArray->FillArray();
    std::thread sortThread(BubbleSort, std::ref(*sArray)); 
    sortThread.detach();
    while(!sArray->isSorted()){

        if(sArray->needRepaint){
            this->paint();
            sArray->needRepaint = false;
        }
    }
}
