#
SOURCE = main.cpp
TARGET = verify

CROSS_COMPILE = arm-linux-gnueabihf-g++
CFLAGS = -g -Wall

build:
	$(CROSS_COMPILE) $(CFLAGS) $(SOURCE) $(INCLUDE) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~
