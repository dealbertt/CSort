#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string.h>

typedef struct Config{
    int numberElements;
    int windowWidth;
    int windowHeigth;
    int delay; //amount of time that each thread sleep after performing an operation (playing sound or sorting)
    int debug;
    int volume; //Volume of the audio 
}Config;

Config *readConfiguration(const char *);

#endif


