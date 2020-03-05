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
    vio_data.data = (unsigned char *)audio::sineData;
    vio_data.length = audio::sineDataSize;
    vio_data.offset = 0;
    // load sample with sndfile
    file = SndfileHandle(vio, &vio_data);
    if (file)
    {
        const sf_count_t lenght = file.frames() * file.channels();
        sample.resize(lenght);
        file.read(&sample.at(0), file.frames());
    }
    else
    {
        printf("load error\n");
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
void MyPlugin::run(const float **inputs, float **outputs, uint32_t frames,
                   const MidiEvent *midiEvents, uint32_t midiEventCount)

{
    for (uint32_t i = 0; i < frames; i++)
    {
        outputs[0][i] = sample.at(phase);
        phase++;
        if (phase >= sample.size())
            phase = 0;
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
