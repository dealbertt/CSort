#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL3/SDL_audio.h>
#include <iostream>
#include <cmath>


#include "../include/sortView.hpp"

static const size_t s_samplerate = 44100;


static const size_t max_oscillators = 512;

static std::mutex MtxAccess;
extern ViewObject *globalObject;


class Oscillator{
    protected:
        double m_freq;
        size_t m_start, m_end;

        size_t m_duration;

    public:
        Oscillator(double freq, size_t tstart, size_t duration = 44100 / 8) : m_freq(freq),
        m_start(tstart),
        m_end(tstart + duration),
        m_duration(duration){}

        static double wave_sin(double x){
            return sin(x * 2 * M_PI);
        }

        static double wave_sin3(double x){
            double s = sin(x * 2 * M_PI);

            return s * s* s;
        }

        static double wave_triangle(double x){
            x = fmod(x, 1.0);

            if(x <= 0.25) return 4.0 * x;
            if(x <= 0.75) return 2.0 - 4.0 * x;

            return 4.0 * x - 4.0;
        }

        static double wave(double x){
            return wave_triangle(x);
        }

        double envelope(size_t i) const; 

        void mix(double *data, int size, size_t p) const {
            for(int i = 0; i < size; i++){
                if(p + i < m_start) continue;
                if(p + i >= m_end) break;

                size_t trel = (p + i - m_start);
                
                data[i] += envelope(trel) * wave(static_cast<double>(trel) / s_samplerate * m_freq);
            }
        }

        size_t tstart() const {
            return m_start;
        }

        bool isDone(size_t p) const {
            return (p >= m_end);
        }

};

static std::vector<Oscillator> osciList;

void SoundAccess(size_t i);
void SoundReset();
void testAudioWithSimpleTone();
int SDLCALL AudioStreamCallBack(void *udata, SDL_AudioStream *stream, void *out_buffer, int len);
void SDLCALL AudioStreamNotificationCallback(void *udata, SDL_AudioStream *stream, int additional_amount, int total_amount);
#endif
