
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

#ifndef LIBSNDFILE_VIO_HPP
#define LIBSNDFILE_VIO_HPP

#include "DistrhoPlugin.hpp"
#include <sndfile.hh>
#include <vector>
#include "Samples.hpp"
#include "Oscillator.hpp"

START_NAMESPACE_DISTRHO

class MyPlugin : public Plugin
{
public:
    MyPlugin();

protected:
    const char *getLabel() const override
    {
        return "libsndfile_vio";
    }

    const char *getDescription() const override
    {
        return "Demo using libsndfile with virtual io";
    }

    const char *getMaker() const override
    {
        return "Clearly Broken Software";
    }

    const char *getHomePage() const override
    {
        return "https://github.com/rghvdberg/libsndfile_vio_dpf";
    }

    const char *getLicense() const override
    {
        return "ISC";
    }

    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('s', 'F', 'v', 'I');
    }

    void initParameter(uint32_t, Parameter &) override;
    float getParameterValue(uint32_t) const override;
    void setParameterValue(uint32_t, float) override;

    void run(const float **inputs, float **outputs, uint32_t, const MidiEvent *midiEvents, uint32_t midiEventCount) override;

    // -------------------------------------------------------------------------------------------------------

private:
    // virtual file system
    SF_VIRTUAL_IO vio;
    // local functions for virtual file system
    static sf_count_t vfget_filelen(void *user_data);
    static sf_count_t vfseek(sf_count_t offset, int whence, void *user_data);
    static sf_count_t vfread(void *ptr, sf_count_t count, void *user_data);
    static sf_count_t vfwrite(const void *ptr, sf_count_t count, void *user_data);
    static sf_count_t vftell(void *user_data);

    // struct holding data for virtual file system
    struct VIO_DATA
    {
        sf_count_t offset, length;
        unsigned char *data;
    };
    VIO_DATA vio_data;
    SndfileHandle file;
    //
    Oscillator osc;

    
   
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPlugin)
};

Plugin *createPlugin()
{
    return new MyPlugin();
}

END_NAMESPACE_DISTRHO
#endif