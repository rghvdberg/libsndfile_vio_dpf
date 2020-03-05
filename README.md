# libsndfile_vio_dpf

## Audio plugin demonstrating how to load audio files from memory buffers with [libsndfile](https://github.com/erikd/libsndfile).

This plugin uses [dpf](https://github.com/DISTRHO/DPF) as audio plugin framework. It builds a lv2 and vst2 plugin and a jack app if the jack headers are installed
There's no GUI.

---

## Build Instructions

### Dependencies
`libsndfile1-dev`  
`lv2-dev`  
(optional) `libjack-jackd2-dev` **or** `libjack-dev`

```
git clone --recursive https://github.com/rghvdberg/libsndfile_vio_dpf.git
cd libsndfile_vio_dpf
make
```

there's no ~~`make install`~~  
binaries are in the `bin` directory


