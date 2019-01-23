# ThingsSoundTest

Simple audio test for Android Things that plays sound in a loop.  
The app will try to initialize the TAS5825M dsp with the config from `amp_cfg_TAS5825M_bypass_LR.sh`, which is converted into a kotlin array by the below mentioned converter. The data is stored in `DspData.kt`.

In the `onCreate` method of the `MainActivity` there are three options to play audio:
- `playAudio1`: Uses `AudioTrack` to manually configure audio options and stream data from a WAV file into the output.
- `playAudio2`: Uses `AudioManager` to automize playback.
- `playAudio3`: Uses `AudioTrack` and feeds the output stream with generated data. This outputs disturbing noise.
- `volumeTest(from, to)`: Works like `playAudio1`, but only plays a small portion of the audio while increasing the volume.

---

Included is an application for converting i2c cfg files into different selectable outputs and the other way round (found in `./i2c-cfg-converter/`).
Simply build with `g++ converter.cpp -o converter`. There are no special dependencies needed. Execute without parameters for help:

```
Usage: bla <1-7>[u] <input file>
Result will be written to output.txt

Params:
 1: kotlin: single commands with only writeRegByte
 2: kotlin: single commands with writeRegBuffer
 3: kotlin: array
 4: kotlin: array with arrays
 5: bash: single pio commands
 6: bash: single pio commands with buffer
 7: bash: single i2cset commands

 [u] modfier calls the corresponding undo function to revert the output into the i2c cfg format
```
