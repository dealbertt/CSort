#include <SDL3/SDL_render.h>
#include <iostream>
#include "../include/sortView.hpp"
#include "../include/config.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

extern Config config;
extern SDL_Renderer *renderer;
extern SDL_Window *window;

void ViewObject::paint(){
    if(sArray.getSize() == 0){
        return;
    } 

    size_t size = sArray.getSize();
    size_t width = config.windowWidth;
    //size_t heigth = config.windowHeigth;

    
    //float wbar = static_cast<float>(config.windowWidth) / size;
    float wbar = (width - (size - 1)) / (double)size;

    float x = 0;
    for(size_t i = 0; i < size; i++){
        SDL_FRect rect = {x, static_cast<float>(config.windowHeigth - sArray.getItemMutable(i).getValue()), wbar, static_cast<float>(sArray.getItemMutable(i).getValue())};
        rectArray.push_back(rect);
        
        std::cout << "Value:" << sArray.getItemMutable(i).getValue() << std::endl;
        x += wbar + 2;
    }

    
    for (const auto& member : rectArray) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &member);
        //rectsToRender.push_back(member.rect);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    std::cout << "Painted" << std::endl;
}


