#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include "array.hpp"
#include "sorting.hpp"
#include <SDL2/SDL_render.h>
#include <chrono>
class ViewObject{
    protected:
        Array &array;

    public:
        ViewObject(Array &array, SDL_Renderer &renderer): array(array), renderer(renderer){
            wbar = 0;
            spacing = 1.0f;
        }

    public:
        float wbar;
        float spacing;
        SDL_Renderer &renderer;

    public:
        float calculateWidthofBar(size_t size);
        void paint();
        void executeSort(void (*func)(class Array&));
        SDL_Color configureColor(ArrayItem &item);
        void markArrayDone();

};

struct Algorithm{
    std::string name;
    void (*func)(class Array&);
    uint64_t maxSize;
    std::string description;
    int delay;
    //pointer to a function that takes an array class as a parameter
};

extern const struct Algorithm algoList[];

void runList(SDL_Renderer *renderer);
#endif
