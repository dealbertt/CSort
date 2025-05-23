#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include <memory>
#include <vector>
#include "array.hpp"
#include <SDL3/SDL_rect.h>
class ViewObject{

    public:
        std::unique_ptr<Array>sArray;
        std::vector<SDL_FRect> rectArray;

    void paint();
};

#endif
