CC = gcc
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -Wno-unused-parameter -UENABLE_DEBUG
LDFLAGS =
LIBS = -lstdc++ -lopcodes -lbfd -liberty
TARGET = swdisasm

.SUFFIXES: .cc

SRC = disassembler.cc image.cc le.cc le_image.cc swdisasm.cc util.cc

OBJ = $(SRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

.cc.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET) core
