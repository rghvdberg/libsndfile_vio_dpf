#ifndef OSCILLATOR_HPP_INCLUDED
#define OSCILLATOR_HPP_INCLUDED

#include <vector>
#include <cstring>

class Oscillator
{
public:
    Oscillator(); // default ctor
    Oscillator(std::vector<float> &smp);
    // setters
    void setSampleData(std::vector<float> &smp);
    void setChannels(unsigned int channels);
    void setLength(size_t frames);

    float getNextSample();
    void getSamples(float **buffer, size_t frames);
    size_t getPhase() const;
    

private:
    size_t phase; // in frames
    unsigned int channels;
    size_t length; // in frames
    std::vector<float> sampleData;
};

#endif