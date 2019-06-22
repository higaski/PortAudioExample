execute_process(COMMAND git submodule update --init  --recursive  --
                                                                  external/portaudio
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/external/portaudio
                 ${PROJECT_BINARY_DIR}/external/portaudio)
