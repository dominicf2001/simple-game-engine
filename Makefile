CXX = g++
TARGET = main
SOURCES = utility.cpp gui.cpp entity.cpp $(TARGET).cpp
LIB_DIR = $(PWD)/lib
LDLIBS = -lGL -lglut -lGLU -L/$(LIB_DIR) -lIrrKlang -pthread -lfreeimage

$(TARGET): $(SOURCES)
	$(CXX) -o $@ $^ $(LDLIBS) -Wl,-rpath,$(LIB_DIR)

all: $(TARGET)

clean:
	rm -f $(TARGET)
