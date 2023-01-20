#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

const int sampleRate = 48000;
const int bitDepth = 24;
const int minFreq = 20;
const int maxFreq = 20000;
const int maxDuration = 30;

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
    }
};

void writeToFile(ofstream &file, int value, int size)
{
    file.write(reinterpret_cast<char *>(&value), size);
}

int makeSineWave(float freq, int_least32_t duration)
{

    if (freq < minFreq || freq > maxFreq)
    {
        throw invalid_argument("Frequency must be in the range of human hearing (20 Hz - 20 kHz).");
    }
    if (duration <= 0 || duration > maxDuration)
    {
        throw invalid_argument("Duration must be in the range of 1 - 30 seconds.");
    }

    ofstream audioFile;
    audioFile.open("audio.wav", ios::binary);
    float amplitude = 0.5;
    SineOscillator sineOscillator(freq, amplitude);

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

    // DATA CHUNK
    audioFile << "data";
    audioFile << "----";

    int preAudioPosition = audioFile.tellp();
    auto maxAmplitude = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; i++)
    {
        auto sample = sineOscillator.process();
        int intSample = static_cast<int>(sample * maxAmplitude);
        writeToFile(audioFile, intSample, 3);
    }
    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();

    return 0;
}

int main()
{
    char userInput;

    cout << "Hello! Welcome to the sine wave generator." << endl;

    string directions = "Please enter the frequency and number of seconds of your sine wave: \n";
    float freq;
    int duration;

    cout << directions << endl;
    cout << "Frequency (20 Hz - 20 kHz): " << endl;
    cin >> freq;
    cout << "Seconds (1 - 30): " << endl;
    cin >> duration;

    try
    {
        makeSineWave(freq, duration);
        cout << "Thank you for participlating. Goodbye!" << endl;
    }
    catch (invalid_argument &e)
    {
        cerr << e.what() << endl;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    return 0;
}