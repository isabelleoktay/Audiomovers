#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

const int sampleRate = 48000;
const int bitDepth = 24;

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

void writeToFile(ofstream &file, int value, int size)
{
    file.write(reinterpret_cast<char *>(&value), size);
}

int main()
{
    int duration = 2;
    ofstream audioFile;
    audioFile.open("waveform.wav", ios::binary);
    SineOscillator sineOscillator(48000, 0.5);

    // HEADER CHUNK
    audioFile << "RIFF";
    audioFile << "----";
    audioFile << "WAVE";

    // FORMAT CHUNK
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4);                        // Size
    writeToFile(audioFile, 1, 2);                         // Compression code
    writeToFile(audioFile, 1, 2);                         // Number of channels
    writeToFile(audioFile, sampleRate, 4);                // Sample rate
    writeToFile(audioFile, sampleRate * bitDepth / 8, 4); // Byte rate
    writeToFile(audioFile, bitDepth / 8, 2);              // Block align
    writeToFile(audioFile, bitDepth, 2);                  // Bit depth

    auto maxAmplitude = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; i++)
    {
        auto sample = sineOscillator.process();
        int intSample = static_cast<int>(sample * maxAmplitude);
        writeToFile(audioFile, intSample, 3);
    }

    audioFile.close();

    return 0;
}