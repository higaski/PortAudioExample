SHELL = /bin/sh
MKDIR_P = mkdir -p
RM = -rm -rf
CP = cp

CXX = g++
LD = g++

BUILD_DIR = ./build
TARGET_NAME = $(MAKECMDGOALS)
TARGET_DIR = $(BUILD_DIR)/$(TARGET_NAME)
EXE = $(TARGET_DIR)/$(TARGET_NAME)

all:
	$(MAKE) PortAudioExample
	
.PHONY: all clean

clean:
	$(RM) $(BUILD_DIR)/*

PortAudioExample: $(EXE)

INC_DIRS = ./src
SRC_DIRS = ./src 

# Create include flags
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# Preprocessor flags
CPPFLAGS = $(INC_FLAGS) -MMD -MP

# Common flags
FLAGS = -fmessage-length=0 -fsigned-char -fPIC -O2

# C++ flags
CXXFLAGS = $(FLAGS)
CXXFLAGS += -std=c++2a

# Linker flags
LDFLAGS = $(FLAGS)
LDLIBS = -lportaudio -lstdc++fs

# Recursively add all sources
SRCS := $(shell find $(SRC_DIRS) -name *asm -or -name *.s -or -name *.S -or -name *.c -or -name *.cc -or -name *.cpp)
OBJS := $(SRCS:%=$(TARGET_DIR)/%.o)

# Include all dependency files (.d)
DEPS := $(OBJS:.o=.d)

# Create include directories from dependency files
INC_DIRS += $(shell find $(SRC_DIRS) -type d)

# Create executable
$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)  

# C++ 
$(TARGET_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)