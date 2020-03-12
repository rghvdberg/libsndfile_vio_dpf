#include "Oscillator.hpp"
#include <stdio.h>

Oscillator::Oscillator()
{
    phase = 0;
    // default ctor
}

Oscillator::Oscillator(std::vector<float> &v)
{
    phase = 0;
    sampleData = v;
}

void Oscillator::setChannels(unsigned int c)
{
    channels = c;
}

void Oscillator::setSampleData(std::vector<float> &v)
{
    sampleData = v;
}

void Oscillator::setLength(size_t frames)
{
    length = frames;
}

float Oscillator::getNextSample()
{
    if (phase > sampleData.size())
    {
        return 0.0f;
    }
    else
    {
        return sampleData.at(phase++);
    }
}

size_t Oscillator::getPhase() const
{
    return phase;
}

void Oscillator::getSamples(float **buffer, size_t offset, size_t frames)
{
    size_t frames_left = frames;
    while (frames_left)
    {
        size_t start = phase;
        size_t end = phase + frames_left;
        if (end >= length)
        {
            end = length;
            phase = 0;
        }
        size_t block = end - start;
        for (size_t i = 0; i < block; i++)
        {
            const size_t left = (start + i) * channels;
            const size_t right = (start + i) * channels + 1;
            buffer[0][i + offset] = sampleData.at(left);
            buffer[1][i + offset] = sampleData.at(right);
        }
        phase += block;

        if (phase >= length)
            phase = phase - length;
        frames_left -= block;
    }
}
