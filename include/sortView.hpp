#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include "array.hpp"
#include "sorting.hpp"
#include <SDL2/SDL_render.h>
class ViewObject{
    protected:
        Array &array;

    public:
        ViewObject(Array &array, SDL_Renderer &renderer): array(array), renderer(renderer){
            wbar = 0;
            spacing = 2.0f;
        }

    public:
        float wbar;
        float spacing;
        SDL_Renderer &renderer;

    public:
        float calculateWidthofBar(size_t size);
        void paint();
        void executeSort();
        SDL_Color configureColor(ArrayItem &item);
        void markArrayDone();

};

#endif
