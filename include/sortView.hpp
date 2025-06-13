#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include "array.hpp"
#include "sorting.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <chrono>
class ViewObject{
    public:
        Array array;
        std::chrono::steady_clock::time_point a;
        std::chrono::steady_clock::time_point b;

    public:
        ViewObject(size_t maxSize, int windowHeight, SDL_Renderer &renderer): array(maxSize, windowHeight), renderer(renderer){
            wbar = 0;
            spacing = 1.0f;
            array.setSize(maxSize);
            array.FillArray();
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
        int handleKeyboard(SDL_Event &event);
        void startArray();
        void finishArray();

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

void SoundCallBack(void *udata, Uint8 *stream, int len);
void cleanUp();
#endif
