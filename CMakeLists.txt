cmake_minimum_required(VERSION 3.8)

project(PortAudioExample VERSION 1.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_executable(PortAudioExample ${CMAKE_CURRENT_SOURCE_DIR}/src/PortAudioExample
                                ${CMAKE_CURRENT_SOURCE_DIR}/src/wav_header)

target_compile_options(PortAudioExample
                       PRIVATE -fPIC
                               -O3
                               $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a>)     

find_package(portaudio)
target_link_libraries(PortAudioExample
                      PRIVATE portaudio_static
                              stdc++fs)