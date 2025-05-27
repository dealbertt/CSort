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


void CocktailSort(class Array &array){
    size_t start = 0;
    size_t end = array.getSize() - 1;

    while (start < end) {
        // loop from left to right
        for (size_t i = start; i < end; i++) {
            if (array[i] > array[i + 1]) {

                array.swap(i, i + 1);

            }
        }
        end--;

        // loop from right to left
        for (size_t i = end; i > start; i--) {
            if (array[i] < array[i - 1]) { // Note the change here: comparing with the element to the left

                array.swap(i, i - 1); // Note the change here: swapping with the element to the left

            }
        }
        start++;
    }
    array.setSorted(true);
    return;
}


void SelectionSort(class Array &array){
    size_t size = array.getSize();
     for(size_t i = 0; i < size - 1; i++){
        size_t min = i;

        for(size_t j = i + 1; j < size; j++){
            //highlightValue(window, renderer, array[j]);
            if(array[j] < array[min]){
                min = j;
            }
        }
        //swap the ith value for the min value

        array.swap(i, min);

        /*
        array_member aux = array[i];
        array[i] = array[min];
        array[min] = aux;
        */
    } 
    array.setSorted(true);
}
void InsertionSort(class Array &array){
    size_t size = array.getSize();

    for(size_t i = 1; i < size; i++){

        ArrayItem item = array[i];
        int j = i - 1;

        while(j >= 0 && array[j].getValue() > item.getValue()){
            array.swap(j, j + 1);
            j--;
        } 
    }
    array.setSorted(true);
} 
void QuickSort(class Array &array); 

