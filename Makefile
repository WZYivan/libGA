include ./script/help.mk

CXX :=

COMPILER ?= g++
## C++ version
VERSION ?= 23
## if build with debug flag: `Debug` or `Release`
BUILD_TYPE ?= Debug
OPTIMIZE ?= 2
## how to link dependence: `Dynamic` or `Static`
DEPS_LINK_TYPE ?= Dynamic
CXX += $(COMPILER) -std=gnu++$(VERSION) -fPIC -Wno-interference-size -Wall

ifeq ($(BUILD_TYPE), Debug)
CXX += -g
LGA_MACROS += -DLGA_DEBUG
else ifeq ($(BUILD_TYPE), Release)
CXX += -O$(OPTIMIZE)
endif

## where to find dependence, this is my custom setting
THIRD_PARTY_ROOT ?= /home/azusa/file/libs/install
LGA_LIB_NAME := ga
LGA_LINK := 

ifeq ($(DEPS_LINK_TYPE), Dynamic)
LGA_LINK += -Bdynamic
endif

LGA_HEADER_DIR := ./src
LGA_SRC_DIR := ./src/lga/impl
LGA_TEST_SRC_DIR := ./test
LGA_MACROS := -DCATCH_CONFIG_MAIN
LGA_INCLUDE := -I$(THIRD_PARTY_ROOT)/include -I$(LGA_HEADER_DIR)
LGA_LINK += -lDataFrame -ltbb -lboost_json -lCatch2Main -lCatch2 -L$(THIRD_PARTY_ROOT)/lib
LGA_RUNTIME := -Wl,-rpath=$(THIRD_PARTY_ROOT)/lib
LGA_SO_NAME := lib$(LGA_LIB_NAME).so
LGA_A_NAME := lib$(LGA_LIB_NAME)_static.a
LGA_LIB_DIR := ./lib
LGA_OBJ_DIR := $(LGA_LIB_DIR)/obj
LGA_TEST_BIN_DIR := $(LGA_TEST_SRC_DIR)/bin
LGA_TEST_OBJ_DIR := $(LGA_TEST_SRC_DIR)/obj
LGA_SRC_FILES := $(wildcard $(LGA_SRC_DIR)/*.cpp)
LGA_OBJ_FILES := $(addprefix  $(LGA_OBJ_DIR)/, $(notdir $(LGA_SRC_FILES:.cpp=.o))) 
LGA_TEST_SRC_FILES := $(wildcard $(LGA_TEST_SRC_DIR)/*.cpp)
LGA_TEST_OBJ_FILES := $(addprefix $(LGA_TEST_OBJ_DIR)/, $(notdir $(LGA_TEST_SRC_FILES:.cpp=.o)))
LGA_TEST_EXE_FILES := $(addprefix $(LGA_TEST_BIN_DIR)/, $(notdir $(LGA_TEST_SRC_FILES:.cpp=)))

## Eigen3: `https://libeigen.gitlab.io/`
EIGEN3_ROOT   ?= $(THIRD_PARTY_ROOT)
## DataFrame: `https://github.com/hosseinmoein/DataFrame`
DATAFRAME_ROOT ?= $(THIRD_PARTY_ROOT)
## onetbb: `https://github.com/uxlfoundation/oneTBB`
ONETBB_ROOT   ?= $(THIRD_PARTY_ROOT)
## boost: `https://github.com/boostorg/boost`
BOOST_ROOT    ?= $(THIRD_PARTY_ROOT)
DEPS := EIGEN3 DATAFRAME ONETBB BOOST CATCH2
$(foreach lib, $(DEPS),\
    $(if $(filter-out $(THIRD_PARTY_ROOT),$($(lib)_ROOT)),\
        $(eval LGA_INCLUDE += -I$($(lib)_ROOT)/include)\
        $(eval LGA_LINK   += -L$($(lib)_ROOT)/lib)\
        $(eval LGA_RUNTIME += -Wl,-rpath=$($(lib)_ROOT)/lib)\
    ,)\
)




DIRS := $(LGA_LIB_DIR) $(LGA_OBJ_DIR) $(LGA_TEST_BIN_DIR) $(LGA_TEST_OBJ_DIR)
$(shell mkdir -p $(DIRS))

$(LGA_OBJ_DIR)/%.o: $(LGA_SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(LGA_MACROS) $(LGA_INCLUDE)

$(LGA_SO_NAME): $(LGA_OBJ_FILES) 
	$(CXX) -shared $^ -o $(LGA_LIB_DIR)/$(LGA_SO_NAME) $(LGA_LINK) $(LGA_RUNTIME)

## build shared lib (libga.so)
#:req THIRD_PARTY_ROOT
#:opt BUILD_TYPE DEPS_LINK_TYPE <LIB_NAME>_ROOT
shared-lib: $(LGA_SO_NAME)

$(LGA_TEST_OBJ_DIR)/%.o: $(LGA_TEST_SRC_DIR)/%.cpp 
	$(CXX) -c $< -o $@ $(LGA_MACROS) $(LGA_INCLUDE)

$(LGA_TEST_BIN_DIR)/%: $(LGA_TEST_OBJ_DIR)/%.o
	$(CXX) $< -o $@ -lga -L$(LGA_LIB_DIR) -Wl,-rpath=$(LGA_LIB_DIR) $(LGA_LINK) $(LGA_RUNTIME)

## build all test
test-exe: $(LGA_TEST_EXE_FILES) $(LGA_TEST_OBJ_FILES)

YELLOW := $(shell tput setaf 3)
BLUE   := $(shell tput setaf 4)
CYAN   := $(shell tput setaf 6)
RED    := $(shell tput setaf 1)
GREEN  := $(shell tput setaf 2)
NC     := $(shell tput sgr0)

## run all test
run:
	@find $(LGA_TEST_BIN_DIR) -type f -executable \
		-exec echo -e "$(RED)> Running {}$(NC)" \; \
		-exec echo "" \; \
		-exec {} \; \
		-exec echo "" \; \
		-exec echo -e "$(GREEN)-------------------------------------------------------------------------------$(NC)" \; \
		-exec echo "" \;

clean-dev:
	rm -f frame.log.csv
	rm -f info_frame.log.csv 
	rm -f log.csv
	rm -f tmp 
