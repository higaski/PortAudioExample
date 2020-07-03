cmake_minimum_required(VERSION 3.11)
include(FetchContent)

project(
  PortAudioExample
  VERSION 1.1
  LANGUAGES C CXX)

add_executable(PortAudioExample ${CMAKE_CURRENT_SOURCE_DIR}/src/PortAudioExample
                                ${CMAKE_CURRENT_SOURCE_DIR}/src/wav_header)

target_compile_options(PortAudioExample
                       PRIVATE -fPIC -O3 $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a>)

message("Cloning portaudio")
FetchContent_Declare(portaudio
                     GIT_REPOSITORY https://git.assembla.com/portaudio.git)
FetchContent_MakeAvailable(portaudio)
target_link_libraries(PortAudioExample PRIVATE portaudio_static stdc++fs)
