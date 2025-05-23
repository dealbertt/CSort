#include <iostream>
#include "../include/array.hpp"
void BubbleSort(class Array &array){
    for(size_t i = 0; i < array.getSize(); i ++){
        for(size_t j = 0; j < array.getSize() - 1; i++){
            if(array[j] < array[j + 1]){
                array.swap(j, j + 1);
            }
        }
    }
} 


void CocktailSort(class Array &array); 
void SelectionSort(class Array &array); 
void InsertionSort(class Array &array); 
void QuickSort(class Array &array); 

