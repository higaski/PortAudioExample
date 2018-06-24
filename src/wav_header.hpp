#pragma once

#include <array>
#include <cstdint>

/// WavHeader
struct WavHeader {
  // Contains "RIFF"
  std::array<char, 4> riff_header;
  // Size of the wav portion of the file, which follows the first 8 bytes. File
  // size - 8
  uint32_t wav_size;
  // Contains "WAVE"
  std::array<char, 4> wave_header;

  // Contains "fmt " (includes trailing space)
  std::array<char, 4> fmt_header;
  // Should be 16 for PCM
  uint32_t fmt_chunk_size;
  // Should be 1 for PCM. 3 for IEEE Float
  uint16_t audio_format;
  uint16_t channels;
  uint32_t sample_rate;
  // Number of bytes per second
  uint32_t byte_rate;
  // Number of bytes for one sample (including all channels)
  uint16_t sample_alignment;
  // Number of bits per sample
  uint16_t bit_depth;

  // Contains "data"
  std::array<char, 4> data_header;
  // Number of bytes in data
  uint32_t data_size;

  // Data offset
  uint32_t data_offset;
};

WavHeader encode_wav_header(uint8_t* p);
