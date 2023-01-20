#include <iostream>
#include <cmath>

const int sampleRate = 48000;

class SineOscillator
{
    float frequency, amplitude, angle = 0.0f, offset = 0.0f;

public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp)
    {
        offset = 2 * M_PI * frequency / sampleRate;
    }
    float process()
    {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
        // Asin(2pif/sr)
    }
};

int main()
{
    int duration = 2;
    SineOscillator sineOscillator(48000, 0.5);

    for (int i = 0; i < sampleRate * duration; i++)
    {
        sine
    }

    return 0;
}