#ifndef SORTING_HPP
#define SORTING_HPP

#include "array.hpp"
void BubbleSort(class Array &array); 
void CocktailSort(class Array &array); 
void SelectionSort(class Array &array); 
void InsertionSort(class Array &array); 
void QuickSortInit(class Array &array); 
void QuickSort(class Array &array, uint32_t low, int32_t high);
uint32_t partition(class Array &array, uint32_t low, int32_t high);

#endif 
