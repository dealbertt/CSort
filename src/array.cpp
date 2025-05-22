#include <iostream>
#include "../include/array.hpp"

size_t compareCount = 0;

size_t accessesCount = 0;

void ArrayItem::onAccess(const ArrayItem &item){
    std::cout << "Play some sound here based on item of value: " << item.value << std::endl;
}

void ArrayItem::onComparison(ArrayItem &first, ArrayItem &second){
    compareCount++;

    std::cout << "Play some sound here based on item of value: " << first.value << std::endl;
    std::cout << "Play some sound here based on item of value: " << second.value << std::endl;
}
