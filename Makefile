.DEFAULT_GOAL := help

RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
NC := \033[0m 

CXX ?= g++
VERSION := 23
CXX += -std=gnu++$(VERSION) -fPIC
CXX += -Wno-interference-size # depress warning from libDataFrame
BUILD_TYPE ?= Debug
AR := ar rcs
OPTIMIZE ?= 1
RM := rm -rf
RUNTIME_LINK := 

ifeq ($(BUILD_TYPE), Debug)
CXX += -g
endif

ifeq ($(BUILD_TYPE), Release)
CXX += -O$(OPTIMIZE)
endif

MY_LOCAL_INSTALL := /home/azusa/file/libs/install
THIRD_PARTY_ROOT ?= $(MY_LOCAL_INSTALL)
CXX += -I$(THIRD_PARTY_ROOT)/include -L$(THIRD_PARTY_ROOT)/lib
RUNTIME_LINK += -Wl,-rpath=$(THIRD_PARTY_ROOT)/lib

BOOST_ROOT ?= $(THIRD_PARTY_ROOT)
BOOST_INCLUDE_DIR := $(BOOST_ROOT)/include
BOOST_LIB_DIR := $(BOOST_ROOT)/lib
ifneq ($(BOOST_ROOT), $(THIRD_PARTY_ROOT))
CXX += -I$(BOOST_INCLUDE_DIR) -L$(BOOST_LIB_DIR)
RUNTIME_LINK += -Wl,-rpath=$(BOOST_LIB_DIR)
endif

EIGEN3_ROOT ?= $(THIRD_PARTY_ROOT)
EIGEN3_INCLUDE_DIR := $(EIGEN3_ROOT)/include
EIGEN3_LIB_DIR := $(EIGEN3_ROOT)/lib
ifneq ($(EIGEN3_ROOT), $(THIRD_PARTY_ROOT))
CXX += -I$(EIGEN3_INCLUDE_DIR) -L$(EIGEN3_LIB_DIR)
RUNTIME_LINK += -Wl,-rpath=$(EIGEN3_LIB_DIR)
endif

DATAFRAME_ROOT ?= $(THIRD_PARTY_ROOT)
DATAFRAME_INCLUDE_DIR := $(DATAFRAME_ROOT)/include
DATAFRAME_LIB_DIR := $(DATAFRAME_ROOT)/lib
ifneq ($(DATAFRAME_ROOT), $(THIRD_PARTY_ROOT))
CXX += -I$(DATAFRAME_INCLUDE_DIR) -L$(DATAFRAME_LIB_DIR)
RUNTIME_LINK += -Wl,-rpath=$(DATAFRAME_LIB_DIR)
endif

ONETBB_ROOT ?= $(THIRD_PARTY_ROOT)
ONETBB_INCLUDE_DIR := $(ONETBB_ROOT)/include
ONETBB_LIB_DIR := $(ONETBB_ROOT)/lib
ifneq ($(ONETBB_ROOT), $(THIRD_PARTY_ROOT))
CXX += -I$(ONETBB_INCLUDE_DIR) -L$(ONETBB_LIB_DIR)
RUNTIME_LINK += -Wl,-rpath=$(ONETBB_LIB_DIR)
endif

LGA_INCLUDE_DIR := ./src
LGA_LIB_DIR := ./lib
CXX += -I$(LGA_INCLUDE_DIR) -L$(LGA_LIB_DIR)
LGA_LIB_OBJ_DIR := $(LGA_LIB_DIR)/obj
LGA_LIB_CPP := $(wildcard $(LGA_INCLUDE_DIR)/lga/impl/*.cpp)
LGA_LIB_OBJ :=  $(addprefix $(LGA_LIB_OBJ_DIR)/, $(notdir $(LGA_LIB_CPP:.cpp=.o)))
LGA_LIB_NAME := libga.so
LGA_STATIC_LIB_NAME := libga_static.a
RUNTIME_LINK += -Wl,-rpath=$(LGA_LIB_DIR)

LGA_LIB_LINK := -lDataFrame -ltbb -lboost_json $(RUNTIME_LINK)
LGA_LIB_DEF := 
ifeq ($(BUILD_TYPE), Debug)
LGA_LIB_DEF += -DLGA_DEBUG
endif

LGA_TEST_DIR := ./test
LGA_TEST_OBJ_DIR := $(LGA_TEST_DIR)/obj
LGA_TEST_EXE_DIR :=  $(LGA_TEST_DIR)/bin
LGA_TEST_CPP_DIR := $(LGA_TEST_DIR)
LGA_TEST_CPP := $(wildcard $(LGA_TEST_CPP_DIR)/*.cpp)
LGA_TEST_OBJ :=  $(addprefix $(LGA_TEST_OBJ_DIR)/, $(notdir $(LGA_TEST_CPP:.cpp=.o)))
LGA_TEST_EXE :=  $(addprefix $(LGA_TEST_EXE_DIR)/, $(notdir $(LGA_TEST_CPP:.cpp=)))
LGA_TEST_LINK := -lga $(LGA_LIB_LINK) -lCatch2Main -lCatch2 $(RUNTIME_LINK)
LGA_TEST_DEF := -DLGA_DEBUG -DCATCH_CONFIG_MAIN
.PRECIOUS := $(LGA_TEST_OBJ)


shared-lib: $(LGA_LIB_DIR)/$(LGA_LIB_NAME)

static-lib: $(LGA_LIB_DIR)/$(LGA_STATIC_LIB_NAME)
	
$(LGA_LIB_DIR): $(LGA_LIB_OBJ_DIR) 
	@mkdir -p $@

$(LGA_LIB_OBJ_DIR):
	@mkdir -p $@

$(LGA_LIB_DIR)/$(LGA_LIB_NAME): $(LGA_LIB_OBJ)  | $(LGA_LIB_DIR) 
	$(CXX) -shared $^ -o $(LGA_LIB_DIR)/$(LGA_LIB_NAME) $(LGA_LIB_LINK) 

$(LGA_LIB_DIR)/$(LGA_STATIC_LIB_NAME): $(LGA_LIB_OBJ)  | $(LGA_LIB_DIR) 
	$(AR) $@ $^

$(LGA_LIB_OBJ_DIR)/%.o : $(LGA_INCLUDE_DIR)/lga/impl/%.cpp
	$(CXX) -c $< -o $@ $(LGA_LIB_DEF)

test-exe:  $(LGA_TEST_OBJ) $(LGA_TEST_EXE)


$(LGA_TEST_EXE_DIR)/%: $(LGA_TEST_OBJ_DIR)/%.o | $(LGA_TEST_EXE_DIR)
	$(CXX) $< -o $@ $(LGA_TEST_LINK)

$(LGA_TEST_OBJ_DIR)/%.o: $(LGA_TEST_CPP_DIR)/%.cpp | $(LGA_TEST_OBJ_DIR)
	$(CXX) -c $< -o $@ $(LGA_TEST_DEF)

$(LGA_TEST_OBJ_DIR):
	@mkdir -p $@

$(LGA_TEST_EXE_DIR):
	@mkdir -p $@

clean: 
	$(RM) $(LGA_TEST_OBJ_DIR)
	$(RM) $(LGA_TEST_EXE_DIR)
	$(RM) $(LGA_LIB_OBJ_DIR)

clean-test: 
	$(RM) $(LGA_TEST_OBJ_DIR)
	$(RM) $(LGA_TEST_EXE_DIR)

clean-lib-obj:
	$(RM) $(LGA_LIB_OBJ_DIR)

clean-test-obj:
	$(RM) $(LGA_TEST_OBJ_DIR)

clean-test-exe:
	$(RM) $(LGA_TEST_EXE_DIR)

INSTALL_PREFIX ?= ./install-dir
HEADER_DEST_DIR := $(INSTALL_PREFIX)/include
LIB_DEST_DIR := $(INSTALL_PREFIX)/lib

install: shared-lib static-lib | $(LIB_DEST_DIR) $(HEADER_DEST_DIR)
	cp -r $(LGA_INCLUDE_DIR)/lga $(HEADER_DEST_DIR)/lga
	cp $(LGA_LIB_DIR)/$(LGA_LIB_NAME) $(LIB_DEST_DIR)/$(LGA_LIB_NAME)
	cp $(LGA_LIB_DIR)/$(LGA_STATIC_LIB_NAME) $(LIB_DEST_DIR)/$(LGA_STATIC_LIB_NAME)

$(LIB_DEST_DIR):
	@mkdir -p $@

$(HEADER_DEST_DIR):
	@mkdir -p $@

run:
	@find $(LGA_TEST_EXE_DIR) -type f -executable \
		-exec echo -e "$(YELLOW)> Running {}$(NC)" \; \
		-exec echo "" \; \
		-exec {} \; \
		-exec echo "" \; \
		-exec echo -e "$(YELLOW)-------------------------------------------------------------------------------$(NC)" \; \
		-exec echo "" \;

help:
	@echo "This is a Makefile 100% hand-writing without AI or stuff. It may get bug or something else."
	@echo "-------------------------------------------------------------------------------------------"
	@echo "$(YELLOW)> Targets$(NC)"
	@echo "$(GREEN)"
	@echo "help$(NC)"
	@echo "====> print this help doc"
	@echo "$(GREEN)"
	@echo "shared-lib$(NC)"
	@echo "==========> build libga.so"
	@echo "$(GREEN)"
	@echo "static-lib$(NC)"
	@echo "==========> build libga_static.a"
	@echo "$(GREEN)"
	@echo "install$(NC)"
	@echo "=======> install all lib and header to $(BLUE)INSTALL_PREFIX/$(NC)"
	@echo "$(YELLOW)> Options$(NC)"
	@echo "$(BLUE)"
	@echo "INSTALL_PREFIX$(NC) = install-dir"
	@echo "==============> install to INSTALL_PREFIX/include and INSTALL_PREFIX/lib"
	@echo "$(BLUE)"
	@echo "BUILD_TYPE$(NC) = Debug|Release"
	@echo "$(RED)"
	@echo "THIRD_PARTY_ROOT$(NC) = $(BLUE)MY_LOCAL_INSTALL$(NC)"
	@echo "================> where to get 3rd party libraries,$(RED)this must be set coz MY_LOCAL_INSTALL is my local folder$(NC)"
	@echo "$(BLUE)"
	@echo "<LIB_NAME>_ROOT$(NC)"
	@echo "==============> where to get specified library [BOOST|DATAFRAME|ONETBB|EIGEN3|CATCH2]"
	@echo "$(BLUE)"
	@echo "CXX$(NC) = g++|..."
	@echo "===> never test in other compiler, and this depends on $(RED)C++23$(NC)"
	@echo "$(BLUE)"
	@echo "OPTIMIZE$(NC) = 1|2|3|..."

doxygen-doc:
	@doxygen

clean-dev:
	$(RM) draft.cpp
	@find -type f -name "*log*" -exec echo "remove {}" \; -exec $(RM) {} \;