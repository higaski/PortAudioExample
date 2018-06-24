#include "wav_header.hpp"

/// Encode wav header information
///
/// \param  p   Pointer to wav header
/// \return WavHeader
WavHeader encode_wav_header(uint8_t* p) {
  WavHeader wav_header{};
  uint8_t* p_cpy{p};

  // Contains "RIFF"
  wav_header.riff_header = {static_cast<char>(p[0]),
                            static_cast<char>(p[1]),
                            static_cast<char>(p[2]),
                            static_cast<char>(p[3])};
  p += sizeof(std::array<char, 4>);

  // Size of the wav portion of the file, which follows the first 8 bytes. File
  // size - 8
  wav_header.wav_size = {
      static_cast<uint32_t>(p[3]) << 24 | static_cast<uint32_t>(p[2]) << 16 |
      static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[0])};
  p += sizeof(uint32_t);

  // Contains "WAVE"
  wav_header.wave_header = {static_cast<char>(p[0]),
                            static_cast<char>(p[1]),
                            static_cast<char>(p[2]),
                            static_cast<char>(p[3])};
  p += sizeof(std::array<char, 4>);

  // Contains "fmt " (includes trailing space)
  wav_header.fmt_header = {static_cast<char>(p[0]),
                           static_cast<char>(p[1]),
                           static_cast<char>(p[2]),
                           static_cast<char>(p[3])};
  p += sizeof(std::array<char, 4>);

  // Should be 16 for PCM
  wav_header.fmt_chunk_size = {
      static_cast<uint32_t>(p[3]) << 24 | static_cast<uint32_t>(p[2]) << 16 |
      static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[0])};
  p += sizeof(uint32_t);

  // Should be 1 for PCM. 3 for IEEE Float
  wav_header.audio_format = {static_cast<uint16_t>(p[1]) << 8 |
                             static_cast<uint16_t>(p[0])};
  p += sizeof(uint16_t);

  // Number of channels
  wav_header.channels = {static_cast<uint16_t>(p[1]) << 8 |
                         static_cast<uint16_t>(p[0])};
  p += sizeof(uint16_t);

  wav_header.sample_rate = {
      static_cast<uint32_t>(p[3]) << 24 | static_cast<uint32_t>(p[2]) << 16 |
      static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[0])};
  p += sizeof(uint32_t);

  // Number of bytes per second
  wav_header.byte_rate = {
      static_cast<uint32_t>(p[3]) << 24 | static_cast<uint32_t>(p[2]) << 16 |
      static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[0])};
  p += sizeof(uint32_t);

  // Number of bytes for one sample (including all channels)
  wav_header.sample_alignment = {static_cast<uint16_t>(p[1]) << 8 |
                                 static_cast<uint16_t>(p[0])};
  p += sizeof(uint16_t);

  // Number of bits per sample
  wav_header.bit_depth = {static_cast<uint16_t>(p[1]) << 8 |
                          static_cast<uint16_t>(p[0])};
  p += sizeof(uint16_t);

  constexpr std::array<char, 4> cmp{'d', 'a', 't', 'a'};
  for (;;) {
    // Contains "data"
    wav_header.data_header = {static_cast<char>(p[0]),
                              static_cast<char>(p[1]),
                              static_cast<char>(p[2]),
                              static_cast<char>(p[3])};

    if (wav_header.data_header == cmp) {
      p += sizeof(std::array<char, 4>);
      break;
    } else
      ++p;
  }

  // Number of bytes in data
  wav_header.data_size = {
      static_cast<uint32_t>(p[3]) << 24 | static_cast<uint32_t>(p[2]) << 16 |
      static_cast<uint32_t>(p[1]) << 8 | static_cast<uint32_t>(p[0])};
  p += sizeof(uint32_t);

  // Data offset
  wav_header.data_offset = p - p_cpy;

  return wav_header;
}
