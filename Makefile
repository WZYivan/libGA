# !/bin/zsh

.DEFAULT_GOAL := all

# sys
SHELL := zsh
RM := rm -f

RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
NC := \033[0m 


# path
INCLUDE_DIR := ./include
LIB_DIR := ./lib
SRC_DIR := ./src/lga
BIN_DIR := ./bin
OBJ_DIR := ./obj
TEST_DIR := ./test
BUILD_DIR := $(TEST_DIR)/build

# toolchian
CXX := g++
CXX_FLAGS := -std=gnu++23 \
	-I$(INCLUDE_DIR) \
	-I./src \
	-L$(LIB_DIR) \
	-L$(BIN_DIR)

BUILD_TYPE ?= Release

ifeq ($(BUILD_TYPE), Debug)  
	CXX_FLAGS += -DLGA_DEBUG
endif

COMPILE := $(CXX) $(CXX_FLAGS)
MK_STATIC := ar rcs
MK_DYNAMIC := $(CXX) -fPIC -shared

# targets
CPP_FILES := $(wildcard $(SRC_DIR)/impl/*.cpp )
OBJ_FILES :=  $(addprefix $(OBJ_DIR)/, $(notdir $(CPP_FILES:.cpp=.o)))


# make lib
.PHONY: lib
ifdef STATIC_LIB
LIB_NAME := libga.a
else
LIB_NAME :=
endif
DLIB_NAME := libga.so
lib: $(BIN_DIR) $(OBJ_DIR) $(OBJ_FILES) $(LIB_NAME) $(DLIB_NAME)
	@echo "> compile $(LIB_NAME) $(DLIB_NAME) over"

$(OBJ_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/impl/%.cpp
	$(COMPILE) -fPIC -c $< -o $@
    
$(LIB_NAME): $(OBJ_FILES)
	$(MK_STATIC) $(BIN_DIR)/$@ $^

$(DLIB_NAME): $(OBJ_FILES)
	$(MK_DYNAMIC) -o $(BIN_DIR)/$@ $^

.PHONY: clean-lib
clean-lib: 
	-$(RM) $(BIN_DIR)/$(LIB_NAME)
	-$(RM) $(BIN_DIR)/$(DLIB_NAME)
	-$(RM) $(OBJ_DIR)

# build test

LINK_CATCH2 := -lCatch2Main -lCatch2 
DEF_CATCH2 := -DCATCH_CONFIG_MAIN

LINK_LGA := -Bdynamic -l:libga.so -Wl,-rpath=$(BIN_DIR)
DEF_LGA ?= #-DLGA_HEADER_ONLY

TEST_BUILD_DIR := $(TEST_DIR)/build
TEST_OBJ_DIR := $(TEST_BUILD_DIR)/obj
TEST_CPP := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ := $(addprefix $(TEST_OBJ_DIR)/, $(notdir $(TEST_CPP:.cpp=.o)))
TEST_EXE := $(addprefix $(TEST_BUILD_DIR)/, $(notdir $(TEST_CPP:.cpp=)))

.PHONY: test 
test: $(TEST_OBJ_DIR) $(TEST_OBJ) $(TEST_EXE)

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(COMPILE) -c $< -o $@ $(DEF_CATCH2) $(DEF_LGA)

$(TEST_BUILD_DIR)/%: $(TEST_OBJ_DIR)/%.o
	@mkdir -p $(dir $@)
	$(COMPILE) $< -o $@ $(LINK_CATCH2) $(LINK_LGA)

$(TEST_OBJ_DIR): $(TEST_BUILD_DIR)
	@mkdir -p $@

$(TEST_BUILD_DIR):
	@mkdir -p $@

.PHONY: clean-test
clean-test:
	rm -rf $(TEST_OBJ_DIR) $(TEST_BUILD_DIR)

run:
	@find $(BUILD_DIR) -type f -executable \
		-exec echo -e "$(YELLOW)> Running {}$(NC)" \; \
		-exec echo "" \; \
		-exec {} \; \
		-exec echo -e "$(YELLOW)-------------------------------------------------------------------------------$(NC)" \; \
		-exec echo "" \;

run-test: $(BUILD_DIR) test run
	@echo "Running all executables in $(BUILD_DIR)"

clean: clean-lib clean-test

all: test lib 


