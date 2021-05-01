CXX = llvm-g++
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
	@$(CXX) $(CFLAGS) -c $< -o $@

$(TOBJS): $(DIROBJ)/%.o: $(DIRTEST)/%.cpp
	@$(CXX) -I$(DIRLIB) -c $< -o $@

$(TARGET): $(LOBJS)
	@$(AR) -cr $@ $^
	@$(RL) $@
	@$(CP) $(DIRSRC)/blowfish.h $(DIRLIB)

$(DIRBIN)/test: $(TOBJS) $(TARGET)
	@echo "Building test ..."
	@$(CXX) -I$(DIRLIB) $(LFLAGS) $< -o $@
