#include <iostream>
#include "../include/array.hpp"
void BubbleSort(Array &array){
    for(size_t i = 0; i < array.getSize(); i ++){
        for(size_t j = 0; j < array.getSize() - 1; j++){
            if(array[j] > array[j + 1]){
                array.swap(j, j + 1);
            }
        }
    }
    array.setSorted(true);
    std::cout << "DOne sorting" << std::endl;
} 


void CocktailSort(class Array &array); 
void SelectionSort(class Array &array); 
void InsertionSort(class Array &array); 
void QuickSort(class Array &array); 

