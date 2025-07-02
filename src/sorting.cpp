#include <chrono>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <pthread.h>
#include <mutex>
#include "../include/array.hpp"
#include "../include/sorting.hpp"
std::mutex gMtx;
std::condition_variable gCv;

bool gIsPaused = true;

void threadSignalHandler(int signum){
    (void)signum;
    pthread_exit(nullptr);
}
void toggleSortThreadPause(){
    std::lock_guard<std::mutex> lock(gMtx);
    gIsPaused = !gIsPaused;
    gCv.notify_one();
}
void initAlgorithm(){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK, &set, nullptr);

    signal(SIGUSR1, threadSignalHandler);
}
void checkCondition(){
    std::unique_lock<std::mutex> lock(gMtx);
    gCv.wait(lock, [] {return !gIsPaused;});
}

void BubbleSort(Array &array){
    initAlgorithm();
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
    initAlgorithm();

    size_t start = 0;
    size_t end = array.getSize() - 1;

    while (start < end) {
        // loop from left to right
        for (size_t i = start; i < end; i++) {
            checkCondition();
            if (array[i] > array[i + 1]) {

                array.swap(i, i + 1);

            }
        }
        end--;

        // loop from right to left
        for (size_t i = end; i > start; i--) {
            checkCondition();
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
    initAlgorithm();

    size_t size = array.getSize();
     for(size_t i = 0; i < size - 1; i++){
        size_t min = i;

        for(size_t j = i + 1; j < size; j++){
            //highlightValue(window, renderer, array[j]);
            checkCondition();
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
    initAlgorithm();

    size_t size = array.getSize();

    for(size_t i = 1; i < size; i++){

        checkCondition();
        ArrayItem item = array[i];
        int j = i - 1;

        while(j >= 0 && array[j] > item){
            array.swap(j, j + 1);
            j--;
        } 
    }
    array.setSorted(true);
} 
void QuickSortInit(class Array &array){
    initAlgorithm();

    uint32_t low = 0;
    int32_t high = array.getSize() - 1;

    QuickSort(array, low, high);

    array.setSorted(true);
} 

void QuickSort(class Array &array, uint32_t low, int32_t high){
    if(static_cast<int32_t>(low) < high){
        uint32_t pi = partition(array, low, high);
        QuickSort(array, low, static_cast<int32_t>(pi - 1));
        QuickSort(array, pi + 1, high);
    }
}


uint32_t partition(class Array &array, uint32_t low, int32_t high){
    ArrayItem pivot = array[high];

    int32_t i = static_cast<int32_t>(low - 1);
    
    for (int32_t j = static_cast<int32_t>(low); j <= high - 1; j++) {
        checkCondition();
        if (array[j] < pivot) {
            i++;
            array.swap(i, j);
        }
    }
    
    array.swap(static_cast<uint32_t>(i + 1), high);  

    

    return i + 1;
}

