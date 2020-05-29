CC = gcc
CFLAGS = -O3 -Wall -s -lm
TARGET = numbacruncha
BUILD_PATH = ./
INSTALL_PATH = /usr/local/bin/
SOURCE = numba_cruncha.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(BUILD_PATH)$(SOURCE) $(CFLAGS) -o $(TARGET)

install:
	mv $(TARGET) $(INSTALL_PATH)

clean:
	$(RM) $(TARGET)
	$(RM) $(INSTALL_PATH)$(TARGET)	
