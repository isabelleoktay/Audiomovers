#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

const int sampleRate = 48000;
const int bitDepth = 24;
const int minFreq = 0;
const int maxFreq = 24000;
const int minDuration = 0;
const int maxDuration = 30;
const char *fileName = "audio.wav";

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
        throw invalid_argument("Frequency must be in the natural range (0 Hz - 24 kHz).");
    }
    if (duration < minDuration || duration > maxDuration)
    {
        throw invalid_argument("Duration must be in the range of 0 - 30 seconds.");
    }

    ofstream audioFile;
    try
    {
        audioFile.open(fileName, ios::binary);

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
        auto minAmplitude = -maxAmplitude - 1;
        for (int i = 0; i < sampleRate * duration; i++)
        {
            auto sample = sineOscillator.process();

            int intSample = static_cast<int>((sample - (-amplitude)) / (amplitude - (-amplitude)) * (maxAmplitude - minAmplitude) + minAmplitude);

            writeToFile(audioFile, intSample, bitDepth / 8);
        }
        int postAudioPosition = audioFile.tellp();

        audioFile.seekp(preAudioPosition - 4);
        writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

        audioFile.seekp(4, ios::beg);
        writeToFile(audioFile, postAudioPosition - 8, 4);

        audioFile.close();
    }
    catch (ofstream::failure e)
    {
        cerr << "Exception opening/writing/closing file.\n";
        return -1;
    }

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
    cout << "Frequency (0 Hz - 24 kHz): " << endl;
    cin >> freq;
    cout << "Seconds (0 - 30): " << endl;
    cin >> duration;

    try
    {
        makeSineWave(freq, duration);
        cout << "Thank you. You have generated the file " << fileName << " in your current directory. Goodbye!" << endl;
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