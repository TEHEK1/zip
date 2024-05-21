#Setting output dirs
OUTPUT_DIR ?= ./output
BINARY_DIR ?= $(OUTPUT_DIR)/bin
LIBRARY_DIR ?= $(OUTPUT_DIR)/lib
TESTS_DIR ?= $(OUTPUT_DIR)/tests
OBJ_DIR ?= $(OUTPUT_DIR)/obj

# Setting compiler and flags
CC ?= gcc
CXX ?= g++
C_FLAGS += -Wall -Wextra -g
CXX_FLAGS += -std=c++17

ifdef sanitize
	CFLAGS += -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
endif

ARGP_LIBS ?= -largp
Z_LIBS ?= -lz
LIBS := $(ARGP_LIBS) $(Z_LIBS)

ARGP_LIB_DIRS ?= -L/opt/homebrew/Cellar/argp-standalone/1.3/lib
Z_LIB_DIRS ?= -L/opt/homebrew/Cellar/argp-standalone/1.3/lib
LIB_DIRS := $(ARGP_LIB_DIRS) $(Z_LIB_DIRS)

ARGP_INCLUDE_DIRS ?= -I/opt/homebrew/Cellar/argp-standalone/1.3/include
INCLUDE_DIRS := -Iinclude $(ARGP_INCLUDE_DIRS)

EXECUTABLE1 := $(BINARY_DIR)/zip
EXECUTABLE2 := $(BINARY_DIR)/unzip
EXECUTABLES := $(EXECUTABLE1) $(EXECUTABLE2)

# List of sources for zip and unzip
ZIP_SOURCES = zip/main.c zip/arg_parsing.c file_manipulations.c zip/compress_zlib.c zip/compress.c
UNZIP_SOURCES = unzip/main.c unzip/arg_parsing.c file_manipulations.c unzip/decompress_zlib.c unzip/decompress.c

# Rules
build: $(EXECUTABLES)

$(EXECUTABLE1): $(addprefix $(OBJ_DIR)/, $(ZIP_SOURCES:.c=.o))
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) $(INCLUDE_DIRS) $(addprefix $(OBJ_DIR)/, $(ZIP_SOURCES:.c=.o)) $(LIB_DIRS) $(LIBS) -o $@

$(EXECUTABLE2): $(addprefix $(OBJ_DIR)/, $(UNZIP_SOURCES:.c=.o))
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) $(INCLUDE_DIRS) $(addprefix $(OBJ_DIR)/, $(UNZIP_SOURCES:.c=.o)) $(LIB_DIRS) $(LIBS) -o $@

# Obj files
$(OBJ_DIR)/%.o: %.c
	@echo $(OBJ_DIR)/$(TESTS_SOURCES_DIR)/
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) $(INCLUDE_DIRS) -c $< -o $@

#tests
TEST_SOURCE := tests/test.cpp
TEST_SOURCE_DIR := tests
TEST_BINARY :=
TEST_DEPEND_SOURCES := zip/arg_parsing.c zip/compress_zlib.c zip/compress.c file_manipulations.c unzip/decompress_zlib.c unzip/decompress.c

$(OUTPUT_DIR)/$(TEST_SOURCE:.cpp=.out):$(TEST_SOURCE) $(addprefix $(OBJ_DIR)/, $(TEST_DEPEND_SOURCES:.c=.o)) $(GTEST_SOURCES)
	@mkdir -p $(@D)
	$(CXX)  $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) $(INCLUDE_DIRS) $(TEST_SOURCE) $(LIB_DIRS) $(LIBS)\
 		$(GTEST_SOURCES) $(addprefix $(OBJ_DIR)/, $(TEST_DEPEND_SOURCES:.c=.o)) -o $@



%.run: %.out
	./$<

tests: $(OUTPUT_DIR)/$(TEST_SOURCE:.cpp=.run)

$(OBJ_DIR)/%.gcov:%.c
	@echo $(OBJ_DIR)/$(TESTS_SOURCES_DIR)/
	@mkdir -p $(@D)
	$(CC) -ftest-coverage --coverage $(C_FLAGS) $(INCLUDE_DIRS) -c $< -o $*.o

$(OBJ_DIR)/%.gcov:%.h
	@echo $(OBJ_DIR)/$(TESTS_SOURCES_DIR)/
	@mkdir -p $(@D)
	$(CC) -ftest-coverage --coverage $(C_FLAGS) $(INCLUDE_DIRS) -c $< -o $*.o

$(OBJ_DIR)/%.gcov:%.cpp
	@mkdir -p $(@D)
	$(CXX) -ftest-coverage --coverage $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) $(INCLUDE_DIRS) -c $< -o $*.o

$(OBJ_DIR)/%.gcov:%.cc
	@mkdir -p $(@D)
	$(CXX) -ftest-coverage --coverage $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) $(INCLUDE_DIRS) -c $< -o $*.o

coverage: $(addprefix $(OBJ_DIR)/, $(TEST_SOURCE:.cpp=.gcov)) $(addprefix $(OBJ_DIR)/, $(TEST_DEPEND_SOURCES:.c=.gcov)) $(addprefix $(OBJ_DIR)/, $(GTEST_SOURCES:.cc:.c=.gcov))
	@mkdir -p $(@D)

	$(CXX)  $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) $(INCLUDE_DIRS) $(TEST_SOURCE) $(LIB_DIRS) $(LIBS)\
     		$(GTEST_SOURCES) $(TEST_DEPEND_SOURCES:.c=.o) -o $@.out
	./$@.out
	gcov tests/test.gcda

seccoverage:$(TEST_SOURCE) $(TEST_DEPEND_SOURCES) $(GTEST_SOURCES)
	@mkdir -p $(@D)
	$(CXX) -ftest-coverage --coverage $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) $(INCLUDE_DIRS) $(TEST_SOURCE) $(LIB_DIRS) $(LIBS)\
		$(GTEST_SOURCES) $(TEST_DEPEND_SOURCES) -o $@.out


#Google test
GTEST_DIR ?= ./googletest/googletest
GTEST_LIB ?= $(LIBRARY_DIR)/libgtest.a
GTEST_SOURCES := $(GTEST_DIR)/src/gtest-all.cc
GTEST_OBJS := $(addprefix $(OBJ_DIR)/, $(GTEST_SOURCES:.cc=.o))
GTEST_FLAGS := -pthread
GTEST_INCLUDE_DIRS := -isystem $(GTEST_DIR)/include -I$(GTEST_DIR)
$(OBJ_DIR)/$(GTEST_DIR)/%.o: $(GTEST_DIR)/%.cc
	@mkdir -p $(@D)
	$(CXX)  $(CXX_FLAGS) $(GTEST_FLAGS) $(GTEST_INCLUDE_DIRS) -c $< -o $@

$(GTEST_LIB): $(GTEST_OBJS)
	@mkdir -p $(@D)
	ar -rv $(GTEST_LIB) $(GTEST_OBJS)

# Cleaning
clean:
	rm -f $(EXECUTABLES) $(addprefix $(OBJ_DIR)/, $(ZIP_SOURCES:.c=.o))\
 		$(addprefix $(OBJ_DIR)/, $(UNZIP_SOURCES:.c=.o)) $(TEST_DEPEND_SOURCES:.c=.gcov)\
 		$(TEST_DEPEND_SOURCES:.c=.gcno) *.gcov *.gcno

.PHONY: clean zip unzip