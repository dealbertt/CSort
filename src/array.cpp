#include <iostream>
#include <random>
#include "../include/array.hpp"

size_t compareCount = 0;

size_t accessesCount = 0;
Array::Array(){

}

void ArrayItem::onAccess(const ArrayItem &item){
    std::cout << "Play some sound here based on item of value: " << item.value << std::endl;
}

void ArrayItem::onComparison(ArrayItem &first, ArrayItem &second){
    compareCount++;

    std::cout << "Play some sound here based on item of value: " << first.value << std::endl;
    std::cout << "Play some sound here based on item of value: " << second.value << std::endl;
}

void Array::FillArray(){

    std::random_device rd;  // Seed for the random number engine
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<ArrayItem::valueType> dist(1,  getMaxValue()); // Generates 0 or 1
                                                                                 //
    size_t size = getSize();

    MtxArray.lock();
    for(size_t i = 0; i < size; i++){
        ArrayItem::valueType guess = dist(gen);
        sArray[i] = ArrayItem(guess);
    }
    MtxArray.unlock();
}
