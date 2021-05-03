CXX = g++
AR  = ar
RL  = ranlib
CP  = cp -r

DIRSRC = src
DIRTEST = test
DIRLIB = lib
DIRBIN = bin
DIROBJ = obj

TARGET = $(DIRLIB)/libblowfish.a

LSRCS += $(DIRSRC)/blowfish.cpp
TSRCS += $(DIRTEST)/main.cpp

LOBJS = $(LSRCS:$(DIRSRC)/%.cpp=$(DIROBJ)/%.o)
TOBJS = $(TSRCS:$(DIRTEST)/%.cpp=$(DIROBJ)/%.o)

CFLAGS += -I$(DIRSRC)
LFLAGS += -L$(DIRLIB) -lblowfish
LAOPT  =

# automatic architecture sensing.
KRNL := $(shell uname -s)
KVER := $(shell uname -r | cut -d . -f1) 
ARCH := $(shell uname -m)

ifeq ($(KRNL),Darwin)
	# MacOSX using llvm-g++
	CXX = llvm-g++
	ifeq ($(shell test $(KVER) -gt 19; echo $$?),0)
		LAOPT += -arch x86_64 -arch arm64
	endif
else
	STRIPKRNL = $(shell echo $(KRNL) | cut -d . -f1)
	ifeq ($(STRIPKRNL),MINGW64_NT-10)
		LAOPT += -s -static
	else
		CFLAGS += -std=c++11
		LAOPT += -s
	endif
endif

.PHONY:	prepare clean

all: prepare $(TARGET)
test: $(DIRBIN)/test

prepare:
	@mkdir -p $(DIROBJ)
	@mkdir -p $(DIRLIB)
	@mkdir -p $(DIRBIN)

clean:
	@rm -rf $(LOBJS)
	@rm -rf $(TOBJS)
	@rm -rf $(TARGET)
	@rm -rf $(DIRLIB)/blowfish.h
	@rm -rf $(DIRBIN)/test

$(LOBJS): $(DIROBJ)/%.o: $(DIRSRC)/%.cpp
	@$(CXX) $(CFLAGS) $(LAOPT) -c $< -o $@

$(TOBJS): $(DIROBJ)/%.o: $(DIRTEST)/%.cpp
	@$(CXX) -I$(DIRLIB) $(LAOPT) -c $< -o $@

$(TARGET): $(LOBJS)
	@$(AR) -cr $@ $^
	@$(RL) $@
	@$(CP) $(DIRSRC)/blowfish.h $(DIRLIB)

$(DIRBIN)/test: $(TOBJS) $(TARGET)
	@echo "Building test ..."
	@$(CXX) -I$(DIRLIB) $< $(LFLAGS) $(LAOPT) -o $@
