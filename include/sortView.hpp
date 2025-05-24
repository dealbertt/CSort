#ifndef SORTVIEW_HPP
#define SORTVIEW_HPP

#include "array.hpp"
class ViewObject{
    protected:
        Array &array;

    public:
        ViewObject(Array &array): array(array){}

};

#endif
