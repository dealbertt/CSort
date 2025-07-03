#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include "array.hpp"
#include "sorting.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <chrono>
const int64_t MAX_DELAY = 1000000;
const int MAX_VOLUME = 24000;

class ViewObject{
    private:
        bool textNeedsUpdate;
        std::thread sortThread;
        
    public:
        Array array;
        std::chrono::steady_clock::time_point a;
        std::chrono::steady_clock::time_point b;

    public:
        ViewObject(size_t maxSize, int windowHeight, SDL_Renderer &renderer): array(maxSize, windowHeight), renderer(renderer){
            textNeedsUpdate = false;
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
        int handleEvents();
        void startArray();
        void finishArray();
        int updateText();
        int previousAlgorithm();
        int skipAlgorithm();
        int pressSpaceToContinue();
        int printSpaceToContinue();
};

struct Algorithm{
    std::string name;
    void (*func)(class Array&);//pointer to a function that takes an array class as a parameter
    uint64_t maxSize;
    std::string description;
    int delay;
};

extern const struct Algorithm algoList[];

void runList(SDL_Renderer *renderer);

void SoundCallBack(void *udata, Uint8 *stream, int len);
void cleanUp();
void runAlgorithmAtIndex(size_t index);
void printList();

#endif
