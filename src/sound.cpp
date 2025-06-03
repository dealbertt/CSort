#include <algorithm>
#include <limits>
#include <vector>

#include "../include/sortView.hpp"
#include "../include/sound.hpp"

static const size_t s_samplerate = 44100;

double soundSustain = 2.0;

static const size_t max_oscillators = 512;

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

        double envelope(size_t i) const {
            double x = static_cast<double>(i) / m_duration;
            
            if(x > 1.0) x = 1.0;

            static const double attack = 0.025;
            static const double decay = 0.1;
            static const double sustain = 0.9;
            static const double release = 0.3;

            if(x < attack){
                return 1.0 / attack * x;
            }

            if(x < attack + decay){
                return 1.0 - (x - attack) / decay * (1.0 - sustain);
            }

            if(x < 1.0 - release){
                return sustain;
            }

            return sustain / release * (1.0 - x);

        }

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

static size_t pos = 0;

static void addOscillator(double freq, size_t p, size_t pstart, size_t pduration){
    if(config.debug) std::cout << "[DEBUG] addOscillator freq: " << freq << " at pos: " << pstart << "\n";


    for(size_t i = 0; i < osciList.size(); i++){
        if(osciList[i].isDone(p)){
            osciList[i] = Oscillator(freq, pstart, pduration);
            if(config.debug) std::cout << "[DEBUG] Replaced finished oscillator at index " << i << "\n";
            return;
        }
    }

    if(osciList.size() < max_oscillators){
        osciList.push_back(Oscillator(freq, pstart, pduration));
        if(config.debug) std::cout << "[DEBUG] Added new oscillator, total count: " << osciList.size() << "\n";
        return;
    }
    
    size_t oldest_idx = 0;
    size_t oldest_start = osciList[0].tstart();

    for(size_t i = 1; i < osciList.size(); i++){
        if(osciList[i].tstart() < oldest_start){
            oldest_idx = i;
            oldest_start = osciList[i].tstart();
        }
    }

    osciList[oldest_idx] = Oscillator(freq, pstart, pduration);
    if(config.debug) std::cout << "[DEBUG] Replaced oldest oscillator at index " << oldest_idx << "\n";
}

static std::vector<unsigned int> accessList;

static double arrayIndexToFreq(double index){
    return 120 + 1200 * (index * index);
}

void SoundAccess(size_t i){
    if(config.debug){
        std::cout << "[DEBUG] SoundAccess(" << i << ")\n" << std::endl;
        std::cout << "[DEBUG] accessList size: " << accessList.size() << "\n";
    }
    accessList.push_back(i);
}


void SoundReset(){
    pos = 0;
    osciList.clear();
}

void SoundCallBack(void *udata, Uint8 *stream, int len){
    size_t &p = pos;
    ViewObject &sv = *globalObject;

    int16_t *data = (int16_t *)(stream);

    size_t size = len / sizeof(int16_t);

    if(accessList.empty()){
        memset(stream, 0, len);
        p += size;
        return;
    }
    double pscale = (double)size / accessList.size();

    for (size_t i = 0; i < accessList.size(); i++)
    {
        double relindex = accessList[i] / (double)sv.array.getMaxValue();
        double freq = arrayIndexToFreq(relindex);

        addOscillator(freq, p, p + i * pscale,
                sv.array.sortDelay->getDuration() / 1000.0 * soundSustain * s_samplerate);
    }

    accessList.clear();

    std::vector<double> wave(size, 0.0);
    size_t wavecount = 0;

    for (std::vector<Oscillator>::const_iterator it = osciList.begin(); it != osciList.end(); it++)
    {
        if (!it->isDone(p))
        {
            it->mix(wave.data(), size, p);
            wavecount ++;
        }
    }

    if(wavecount == 0){
        memset(stream, 0, len);
    }else{
        double vol = *std::max_element(wave.begin(), wave.end());

        static double oldvol = 1.0;
        if(vol > oldvol){

        }else{
            vol = 0.9 * oldvol;
        }

        for (size_t i = 0; i < size; i++)
        {
            int32_t v = 24000.0 * wave[i] / (oldvol + (vol - oldvol) * (i / (double)size));

            if (v > 32200) {
                //std::cout << "clip " << p << "\n";
                v = 32200;
            }
            if (v < -32200) {
                //std::cout << "clip " << p << "\n";
                v = -32200;
            }

            data[i] = v;
        }

        oldvol = vol;
    }

    // advance sample timestamp
    p += size;
}

void testAudioWithSimpleTone() {
    std::cout << "[DEBUG] Testing audio with simple tone..." << std::endl;
    
    // Force add a test oscillator
    osciList.clear();
    osciList.push_back(Oscillator(440.0, pos, s_samplerate * 2)); // 440Hz for 2 seconds
    
    std::cout << "[DEBUG] Added test oscillator: 440Hz" << std::endl;
    std::cout << "[DEBUG] Current position: " << pos << std::endl;
    std::cout << "[DEBUG] Oscillator list size: " << osciList.size() << std::endl;
}


