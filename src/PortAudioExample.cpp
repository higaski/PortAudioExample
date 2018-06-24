#include <array>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <vector>
#include <portaudio.h>
#include "wav_header.hpp"

/// Length of data
unsigned int data_length{};

/// Raw wav data
std::vector<uint8_t> raw{};

/// Pointer to raw wav data
uint8_t* in;

/// Read wave data into global variables
///
/// \param  path    Filepath
WavHeader read_wav_data(char const* path) {
  using namespace std;
  namespace fs = filesystem;

  WavHeader wav_header{};

  if (auto p{fs::current_path() /= path}; fs::exists(p)) {
    if (ifstream wav{p, ios::binary}; wav.is_open()) {
      array<uint8_t, 1024> buf{};
      wav.read(reinterpret_cast<char*>(begin(buf)), 1024);
      wav_header = encode_wav_header(begin(buf));
      data_length = wav_header.data_size;
      raw.reserve(wav_header.data_size);
      wav.seekg(wav_header.data_offset);
      wav.read(reinterpret_cast<char*>(&raw[0]), wav_header.data_size);
      in = &raw[0];

    } else {
      cout << "can't open file\n";
      exit(0);
    }
  } else {
    cout << "file not found\n";
    exit(0);
  }

  return wav_header;
}

/// PortAudio callback
///
/// \param  input       Unused
/// \param  output      Destination
/// \param  frameCount  Samples per buffer
/// \param  timeInfo    Unused
/// \param  statusFlags Unused
/// \param  userData    Wav header
/// \return paContinue  Continue
/// \return paComplete  Stop stream, drain output
/// \return paAbort     Abort immediately
static int paCallback(void const* input,
                      void* output,
                      unsigned long frameCount,
                      PaStreamCallbackTimeInfo const* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData) {
  WavHeader* wav_header{reinterpret_cast<WavHeader*>(userData)};

  unsigned int n{data_length >= frameCount ? frameCount : data_length};

  // 8 bit
  if (wav_header->bit_depth == 8) {
    uint8_t* out{reinterpret_cast<uint8_t*>(output)};

    // Assuming 8 bit is always mono
    for (auto i{0ul}; i < n; ++i) {
      *out++ = *in++;
      data_length--;
    }
    // 16 bit
  } else if (wav_header->bit_depth == 16) {
    int16_t* out{reinterpret_cast<int16_t*>(output)};
    int16_t* in_cpy{reinterpret_cast<int16_t*>(in)};

    // Assuming 16 bit is stereo
    for (auto i{0ul}; i < n; ++i) {
      *out++ = *in_cpy++;
      *out++ = *in_cpy++;
      data_length -= 4;
      in += 4;
    }
  }

  return n == frameCount ? paContinue : paComplete;
}

/// Main
///
/// \param  argv    Filepath
/// \return Exit
int main(int argc, char* argv[]) {
  using namespace std;

  if (argc < 2) {
    cout << "useage: PortAudioExample <path>\n";
    exit(0);
  }

  // Read .wav header
  auto wav_header{read_wav_data(argv[1])};

  // Create PortAudio stuff
  PaStreamParameters outputParameters;
  PaStream* stream;
  PaError err;

  err = Pa_Initialize();
  if (err != paNoError)
    goto error;

  // Default output device
  outputParameters.device = Pa_GetDefaultOutputDevice();
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr, "Error: No default output device.\n");
    goto error;
  }

  // Set stream parameters
  outputParameters.channelCount = wav_header.channels;
  outputParameters.sampleFormat = wav_header.bit_depth == 8 ? paUInt8 : paInt16;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  // Open stream
  err = Pa_OpenStream(&stream,
                      NULL,  // No input
                      &outputParameters,
                      wav_header.sample_rate,
                      256,
                      paNoFlag,
                      paCallback,
                      &wav_header);
  if (err != paNoError)
    goto error;

  // Start stream
  err = Pa_StartStream(stream);
  if (err != paNoError)
    goto error;

  while (data_length)
    asm volatile("nop");

  return err;

error:
  Pa_Terminate();
  fprintf(stderr, "An error occured while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
  return err;
}
