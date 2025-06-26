#include <algorithm>
#include <mutex>
#include <vector>

#include "../include/sortView.hpp"
#include "../include/sound.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_audio.h>

double soundSustain = 2.0;

static size_t pos = 0;

static const size_t INTERNAL_BUFFER_SAMPLES = 2048;
static std::vector<int16_t> gInternalAudioBuffer(INTERNAL_BUFFER_SAMPLES);

extern SDL_AudioStream *gAudioStream;

double Oscillator::envelope(size_t i) const{
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

static std::mutex MtxAccess;

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

    if(gAudioStream){
        SDL_FlushAudioStream(gAudioStream);
    }
}

/*
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

    MtxAccess.lock();

    double pscale = (double)size / accessList.size();

    for (size_t i = 0; i < accessList.size(); i++)
    {
        double relindex = accessList[i] / (double)sv.array.getMaxValue();
        double freq = arrayIndexToFreq(relindex);

        addOscillator(freq, p, p + i * pscale,
                50 / 1000.0 * soundSustain * s_samplerate);
        if(config.debug) std::cout << "[DEBUG] Duration of oscillator: " << sv.array.sortDelay->getDuration() / 1000.0f << std::endl;
    }

    accessList.clear();

    MtxAccess.unlock();

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
*/


void testAudioWithSimpleTone() {
    std::cout << "[DEBUG] Testing audio with simple tone..." << std::endl;
    
    MtxAccess.lock();
    // Force add a test oscillator
    osciList.clear();
    osciList.push_back(Oscillator(440.0, pos, s_samplerate * 2)); // 440Hz for 2 seconds
    MtxAccess.unlock();

    SDL_Log("Current oscillator list size: %zu.", osciList.size());
    
}



/*
int SDLCALL AudioStreamCallBack(void *userdata, SDL_AudioStream *stream, void *out_buffer, int len){

    int16_t *data = (int16_t*)(out_buffer);
    size_t num_samples_requested = len / sizeof(int16_t);


    size_t currentGlobalPos = pos;
    
    if(accessList.empty()){

    }else{
        MtxAccess.lock();
    
        double pscale = (double)num_samples_requested / accessList.size();
        for(size_t i = 0; i < accessList.size(); i++){
            double relIndex = accessList[i] / (double)globalObject->array.getMaxValue();
            double freq = arrayIndexToFreq(relIndex);


            size_t oscillatorStartPos = currentGlobalPos + (size_t)(i * pscale);
            size_t oscillatorDurationSamples = static_cast<size_t>(50.0 / 1000.0 * soundSustain * s_samplerate);
            addOscillator(freq, currentGlobalPos, oscillatorStartPos, oscillatorDurationSamples);
        }
        accessList.clear();
        MtxAccess.unlock();
    }


        std::vector<double> waves(num_samples_requested, 0.0);
        size_t wavecount = 0;

        for(auto it = osciList.begin(); it != osciList.end();){
            if(!it->isDone(currentGlobalPos)){
                it->mix(waves.data(), num_samples_requested, currentGlobalPos);
                wavecount++;
                it++;
            }else{
                it = osciList.erase(it);
            }

        }

        if(wavecount == 0){
            memset(out_buffer, 0, len);
        }else{
            double vol = 0.0;
            if(!waves.empty()){
                vol = *std::max_element(waves.begin(), waves.end());
            }

            static double oldvol = 1.0;

            if(vol > oldvol){

            }else{
                vol = 0.9 * oldvol;
            }

            for(size_t i = 0; i < num_samples_requested; i++){
                int32_t v = 24000.0 * waves[i] / (oldvol) + (vol - oldvol) * (static_cast<double>(i) / num_samples_requested);

                if (v > 32200){
                    v = 32200;
                }

                if (v < -32200){
                    v = -32200;
                }

                data[i] = static_cast<int16_t>(v);
            }

            oldvol = vol;
        }

        pos += num_samples_requested;

    return 0;
}
*/

static void generateAudio(SDL_AudioStream *stream, size_t numSamples){
    if(numSamples > gInternalAudioBuffer.size()){
        numSamples = gInternalAudioBuffer.size();
    }

    size_t currentGlobalPos = pos;
    if(accessList.empty()){

    }else{
        MtxAccess.lock();
    
        double pscale = (double)numSamples / accessList.size();
        for(size_t i = 0; i < accessList.size(); i++){
            double relIndex = accessList[i] / (double)globalObject->array.getMaxValue();
            double freq = arrayIndexToFreq(relIndex);


            size_t oscillatorStartPos = currentGlobalPos + (size_t)(i * pscale);
            size_t oscillatorDurationSamples = static_cast<size_t>(50.0 / 1000.0 * soundSustain * s_samplerate);

            addOscillator(freq, currentGlobalPos, oscillatorStartPos, oscillatorDurationSamples);
        }
        accessList.clear();
    }
    std::fill(gInternalAudioBuffer.begin(), gInternalAudioBuffer.begin() + numSamples, 0);

    std::vector<double> waves(numSamples, 0.0);
    size_t wavecount = 0;

    for(auto it = osciList.begin(); it != osciList.end();){
        if(!it->isDone(currentGlobalPos)){
            it->mix(waves.data(), numSamples, currentGlobalPos);
            wavecount++;
            it++;
        }else{
            it = osciList.erase(it);
        }

    }

    if(wavecount == 0){
        //memset(out_buffer, 0, len);
    }else{
        double vol = 0.0;
        if(!waves.empty()){
            vol = *std::max_element(waves.begin(), waves.end());
        }

        static double oldvol = 1.0;

        if(vol > oldvol){

        }else{
            vol = 0.9 * oldvol;
        }

        for(size_t i = 0; i < numSamples; i++){
            int32_t v = 24000.0 * waves[i] / (oldvol) + (vol - oldvol) * (static_cast<double>(i) / numSamples);

            if (v > 32200){
                v = 32200;
            }

            if (v < -32200){
                v = -32200;
            }

            gInternalAudioBuffer[i] = static_cast<int16_t>(v);
        }

        oldvol = vol;
    }

    pos += numSamples;
    if(!SDL_PutAudioStreamData(stream, gInternalAudioBuffer.data(), numSamples * sizeof(int16_t))){
        SDL_Log("Error on SDL_PutAudioStreamData!");
    }
    MtxAccess.unlock();
}

void SDLCALL AudioStreamNotificationCallback(void *udata, SDL_AudioStream *stream, int additional_amount, int total_amount){
    if(config.debug) SDL_Log("AudioStreamNotificationCallback is called");
    size_t samplesNeeded = additional_amount / sizeof(int16_t);

    size_t samplesToGenerate = std::min(samplesNeeded, INTERNAL_BUFFER_SAMPLES);
    if(samplesToGenerate > 0){
        generateAudio(stream, samplesToGenerate);
    }
}
