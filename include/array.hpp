#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <mutex>

#include "config.hpp"

extern size_t compareCount;

extern size_t accessesCount;

extern Config config;

class Delay{
    protected:
        int duration;

    public:
        void setDelay(int newDuration){ duration = newDuration;}
        int getDuration() const {return duration;}
        void delay();

};

class ArrayItem{
    public:
        typedef int valueType;

    protected:
        valueType value;

    public:
        ArrayItem() {}
        explicit ArrayItem(const valueType& value) : value(value) {}
        const valueType &getValue() const { return value;}

        //DEFINE OPERATORS TO ALLOW TO PLAY SOUDS
        bool operator == (const ArrayItem &item) const{
            //onComparisons

            return value == item.value;
        } 

        bool operator != (const ArrayItem &item) const{
            //onComparisons
            return value != item.value;
        } 
        
        bool operator < (const ArrayItem &item) const{
            //onComparisons
            return value < item.value;
        } 

        bool operator <= (const ArrayItem &item) const{
            //onComparisons
            return value <= item.value;
        } 

        bool operator > (const ArrayItem &item) const{
            //onComparisons
            return value > item.value;
        } 

        bool operator >= (const ArrayItem &item) const{
            //onComparisons
            return value >= item.value;
        } 

        static void onComparison(ArrayItem &first, ArrayItem &second);
        static void onAccess(const ArrayItem &item);
};

class Array{
    protected:
        std::vector<ArrayItem> sArray;
        ArrayItem::valueType sArray_maxSize; //The max value of the array will match the type of the array, of course

        struct Access{
            uint32_t index;
            uint8_t color; //Index that is used to then show the color
        };

        Access access1, access2;
        std::vector<Access> listAccesses;

        bool sorted;

    public:
        std::mutex MtxArray;
        Delay *sortDelay;
        bool needRepaint;

    public:
        Array(size_t size, ArrayItem::valueType maxValue) {
            sArray.resize(size);
            sArray_maxSize = maxValue;
            sorted = false;
            needRepaint = false;
        }


    public: //Functions
        Array();

        bool isSorted() const {return sorted;}
        void setSorted(bool newSorted){sorted = newSorted;}
        void onAccess();

        size_t getSize() const {return sArray.size();}
        ArrayItem::valueType getMaxValue() const {return sArray_maxSize;}

        //The caller cannot modify the returned value
        //The function itself cannot modify the value 
        const ArrayItem &getItemConst(size_t index) const{
            assert(index < sArray.size());
            return sArray[index];
        }

        ArrayItem getItemMutable(size_t index){
            assert(index < sArray.size());

            return sArray[index];
        }

        void setItem(ArrayItem &item, size_t index){
            assert(index < sArray.size());

            sArray[index] = item;
        }

        void swap(size_t firstIndex, size_t secondIndex){
            MtxArray.lock();
            assert(firstIndex < sArray.size());
            assert(secondIndex < sArray.size());

            onAccess();
            std::swap(sArray[firstIndex], sArray[secondIndex]);
            onAccess();
            MtxArray.unlock();
        }

        const ArrayItem &operator [](size_t i){
            assert(i < sArray.size());

            return sArray[i];
        } 

        void FillArray();

};
#endif
