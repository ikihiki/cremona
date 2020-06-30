COMPILER     = g++
CFLAGS       = -g -MMD -MP -std=c++17  -Wall -Wextra -Winit-self -Wno-missing-field-initializers -DMPACK_HAS_CONFIG -DCRMNA_TEST
LIBS         = 
INCLUDE      = -I../src 
TEST_INCLUDE = $(INCLUDE) -I.. -Itest -I../third_party/googletest/googletest/include -I../third_party/googletest/googlemock/include 
TEST_LDFLAGS = $(LDFLAGS) -L../third_party/googletest/lib -lgtest -lgmock -lgmock_main -lpthread 
TRGETDIR     = $(CURDIR)/../../build/driver/test/bin
TARGET       = $(TRGETDIR)/driver_test
SRCROOT      = ../src
TESTROOT     = .
SRCDIRS     := $(shell find $(SRCROOT) -type d)
SOURCES      = $(filter-out %/main.c, $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c)))
TESTDIRS    := $(shell find $(TESTROOT) -type d)
TESTS        = $(foreach dir, $(TESTDIRS), $(wildcard $(dir)/*.cpp))
OBJROOT      = $(CURDIR)/../../build/driver/test/obj
OBJECTS      = $(addprefix $(OBJROOT)/, $(SOURCES:$(SRCROOT)/%.c=%.o)) 
OBJDIRS      = $(addprefix $(OBJROOT)/, $(SRCDIRS)) 
TESTOBJECTS  = $(addprefix $(OBJROOT)/, $(TESTS:$(TESTROOT)/%.cpp=%.o)) 
TESTOBJDIRS  = $(addprefix $(OBJROOT)/, $(TESTDIRS)) 
DEPENDS      = $(OBJECTS:.o=.d)

$(TARGET): $(TESTOBJECTS) $(OBJECTS) $(LIBS) 
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(COMPILER) -o $@ $^ $(TEST_LDFLAGS)

$(OBJROOT)/%.o: $(SRCROOT)/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c "$(abspath $<)"

$(OBJROOT)/%.o: $(TESTROOT)/%.cpp
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(COMPILER) $(CFLAGS) $(TEST_INCLUDE) -o $@ -c "$(abspath $<)"

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

run: $(TARGET) 
	-$(TARGET)
.PHONY: run

test:
	$(warning SOURCES = $(SOURCES))
	$(warning OBJECTS = $(OBJECTS))
.PHONY: test
