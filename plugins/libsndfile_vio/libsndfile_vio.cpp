/*Copyright (c) 2020, Rob van den Berg <rghvdberg@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "libsndfile_vio.hpp"
#define SIGNED_SIZEOF(x) ((int64_t)(sizeof(x)))

START_NAMESPACE_DISTRHO

MyPlugin::MyPlugin() : Plugin(0, 0, 0)
{
    // setup function pointers
    vio.get_filelen = vfget_filelen;
    vio.seek = vfseek;
    vio.read = vfread;
    vio.write = vfwrite;
    vio.tell = vftell;
    // fill virtual file system
    size_t length = 0;
    vio_data.data = getSample(length);
    vio_data.length = length;
    vio_data.offset = 0;
    // load sample with sndfile
    file = SndfileHandle(vio, &vio_data);
    if (!file.error())
    {
        const sf_count_t length = file.frames() * file.channels();
        std::vector<float> sample;
        sample.resize(length);
        file.read(&sample.at(0), file.frames() * file.channels());
        osc.setSampleData(sample);
        osc.setChannels(file.channels());
        osc.setLength(file.frames());
        osc.playing = false;
        // printf("length : %ul\n", length);
    }
    else
    {
        printf("libsndfile error %i\n", file.error());
    }
}

void MyPlugin::initParameter(uint32_t, Parameter &)
{
    // no parameters
}
float MyPlugin::getParameterValue(uint32_t) const
{
    return 0.0f;
}
void MyPlugin::setParameterValue(uint32_t, float)
{
    // no parameters
}
void MyPlugin::run(const float **, float **outputs, uint32_t frames,
                   const MidiEvent *midiEvents, uint32_t midiEventCount)
{
    uint32_t framesDone = 0;
    uint32_t curEventIndex = 0;
    size_t frame_in = 0;
    size_t frame_out = frames;
    // reset output buffer
    std::memset(outputs[0], 0, sizeof(float) * frames);
    std::memset(outputs[1], 0, sizeof(float) * frames);

    while (framesDone < frames)
    {
        while (curEventIndex < midiEventCount && (framesDone == midiEvents[curEventIndex].frame))
        {
            int message = midiEvents[curEventIndex].data[0] & 0xF0;
            int velocity = midiEvents[curEventIndex].data[2];
            switch (message)
            {
            case 0x80: // note off
                frame_out = midiEvents[curEventIndex].frame;
                if (frame_out > frame_in && osc.playing)
                {
                    const size_t offset = frame_in;
                    const size_t block = frame_out - frame_in;
                    osc.getSamples(outputs, offset, block);
                    osc.playing = false;
                }
                break;

            case 0x90:             // note on
                if (velocity == 0) // treat as note off
                {
                    frame_out = midiEvents[curEventIndex].frame;
                    if (frame_out > frame_in && osc.playing)
                    {
                        const size_t offset = frame_in;
                        const size_t block = frame_out - frame_in;
                        osc.getSamples(outputs, offset, block);
                        osc.playing = false;
                    }
                    break;
                }
                frame_in = midiEvents[curEventIndex].frame;
                osc.phase = 0;
                osc.playing = true;
                break;
            }
            curEventIndex++;
        }
        ++framesDone;
    }

    if ((frame_out > frame_in) && osc.playing)
    {
        const size_t offset = frame_in;
        const size_t block = frame_out - frame_in;
        osc.getSamples(outputs, offset, block);
    }
}
//-----------------------------------------------------------------------------
sf_count_t MyPlugin::vfget_filelen(void *user_data)
{
    VIO_DATA *vf = (VIO_DATA *)user_data;

    return vf->length;
}

sf_count_t MyPlugin::vfseek(sf_count_t offset, int whence, void *user_data)
{
    VIO_DATA *vf = (VIO_DATA *)user_data;

    switch (whence)
    {
    case SEEK_SET:
        vf->offset = offset;
        break;

    case SEEK_CUR:
        vf->offset = vf->offset + offset;
        break;

    case SEEK_END:
        vf->offset = vf->length + offset;
        break;
    default:
        break;
    };

    return vf->offset;
}

sf_count_t MyPlugin::vfread(void *ptr, sf_count_t count, void *user_data)
{

    VIO_DATA *vf = (VIO_DATA *)user_data;
    if (vf->offset + count > vf->length)
        count = vf->length - vf->offset;
    memcpy(ptr, vf->data + vf->offset, count);
    vf->offset += count;

    return count;
}

sf_count_t MyPlugin::vfwrite(const void *ptr, sf_count_t count, void *user_data)
{
    VIO_DATA *vf = (VIO_DATA *)user_data;

    if (vf->offset >= SIGNED_SIZEOF(vf->data))
        return 0;

    if (vf->offset + count > SIGNED_SIZEOF(vf->data))
        count = sizeof(vf->data) - vf->offset;

    memcpy(vf->data + vf->offset, ptr, (size_t)count);
    vf->offset += count;

    if (vf->offset > vf->length)
        vf->length = vf->offset;

    return count;
}

sf_count_t MyPlugin::vftell(void *user_data)
{
    VIO_DATA *vf = (VIO_DATA *)user_data;

    return vf->offset;
}

END_NAMESPACE_DISTRHO
