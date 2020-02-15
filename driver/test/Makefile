COMPILER     = g++
CFLAGS       = -g -MMD -MP -Wall -Wextra -Winit-self -Wno-missing-field-initializers -DMPACK_HAS_CONFIG -DCRMNA_TEST
LIBS         = 
INCLUDE      = -I../src 
TEST_INCLUDE = $(INCLUDE) -I.. -Itest -I../third_party/googletest/googletest/include 
TEST_LDFLAGS = $(LDFLAGS) -L../third_party/googletest/lib -lgtest -lgtest_main -lpthread 
TRGETDIR     = ./bin
TARGET       = $(TRGETDIR)/driver_test
SRCDIR       = ../src
TESTDIR      = .
SOURCES      = $(filter-out %/main.c, $(wildcard $(SRCDIR)/*.c))
TESTS        = $(wildcard $(TESTDIR)/*.cpp)
OBJDIR       = ./obj
OBJECTS      = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.c=.o)))
TESTOBJECTS  = $(addprefix $(OBJDIR)/, $(notdir $(TESTS:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(TESTOBJECTS) $(OBJECTS) $(LIBS) 
	-mkdir -p $(TRGETDIR)
	$(COMPILER) -o $@ $^ $(TEST_LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(TEST_INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

run: $(TARGET) 
	-$(TARGET)
.PHONY: run


tttt:
	@echo $(SOURCES)