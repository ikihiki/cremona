COMPILER     = g++
CFLAGS       = -g -MMD -MP -Wall -Wextra -Winit-self -Wno-missing-field-initializers -DMPACK_HAS_CONFIG -DCRMNA_TEST
LIBS         = 
INCLUDE      = -I../src 
TEST_INCLUDE = $(INCLUDE) -I.. -Itest -I../third_party/googletest/googletest/include 
TEST_LDFLAGS = $(LDFLAGS) -L../third_party/googletest/lib -lgtest -lgtest_main -lpthread 
TRGETDIR     = ./bin
TARGET       = $(TRGETDIR)/driver_test
SRCROOT       = ../src
TESTDIR      = .
SRCDIRS      := $(shell find $(SRCROOT) -type d)
SOURCES      = $(filter-out %/main.c, $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c)))
TESTS        = $(wildcard $(TESTDIR)/*.cpp)
OBJROOT      = ./obj
OBJECTS      = $(addprefix $(OBJROOT)/, $(SOURCES:$(SRCROOT)/%.c=%.o)) 
OBJDIRS      = $(addprefix $(OBJROOT)/, $(SRCDIRS)) 
TESTOBJECTS  = $(addprefix $(OBJROOT)/, $(notdir $(TESTS:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(TESTOBJECTS) $(OBJECTS) $(LIBS) 
	$(COMPILER) -o $@ $^ $(TEST_LDFLAGS)

$(OBJROOT)/%.o: $(SRCROOT)/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c "$(abspath $<)"

$(OBJROOT)/%.o: $(TESTDIR)/%.cpp
	-mkdir -p $(OBJDIR)
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
