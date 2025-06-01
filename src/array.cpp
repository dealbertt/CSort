#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <unistd.h>
#include "../include/array.hpp"
#include "../include/sound.hpp"

size_t compareCount = 0;

size_t accessesCount = 0;

void Delay::delay(){
     std::this_thread::sleep_for(std::chrono::microseconds(duration));
}

void ArrayItem::onAccess() const {
    std::cout << "[DEBUG] onAccess called. Value: " << this->getValue() << std::endl;
    std::cout << "[DEBUG] this pointer: " << static_cast<const void*>(this) << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(500)); pv


    SoundAccess(this->get());
}

void ArrayItem::onComparison(const ArrayItem &item) const {
    compareCount++;

    std::cout << "[DEBUG] onComparison called. Value: " << this->getValue() << std::endl;
    std::cout << "[DEBUG] onComparison second called. Value: " << item.getValue() << std::endl;
    SoundAccess(this->getValue());
    SoundAccess(item.getValue());
}

void Array::FillArray(){

    sArray.reserve(getSize());
    std::random_device rd;  // Seed for the random number engine
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<ArrayItem::valueType> dist(1,  getMaxValue()); // Generates 0 or 1
                                                                                 //
    size_t size = getSize();

    for(size_t i = 0; i < size; i++){
        ArrayItem::valueType guess = dist(gen);
        sArray[i] = ArrayItem(guess);
    }
}

void Array::onAccess(){
    needRepaint = true;
    //sortDelay->delay();
    //std::this_thread::sleep_for(std::chrono::microseconds(5));
}

void Array::mark(size_t index){
    sArray[index].setColor(1);
}

void Array::markDone(size_t index){
    sArray[index].setColor(2);
}
void Array::Unmark(size_t index){
    sArray[index].setColor(0);
}

