CUR_DIR := .

INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
OBJ_STATIC := $(OBJ_DIR)/static
OBJ_SHARED := $(OBJ_DIR)/shared
BIN_DIR := $(CUR_DIR)/bin

PROGRAM_STATIC := main_static
PROGRAM_SHARED := main_shared
LIB_NAME := libstrutils


STATIC_LIB := ./lib/static
SHARED_LIB := ./lib/shared

static: $(SRC_DIR)/bstrutils.c $(SRC_DIR)/main.c
	gcc -c $(SRC_DIR)/bstrutils.c -o $(OBJ_STATIC)/bstrutils.o -I$(INC_DIR)
	ar rcs $(STATIC_LIB)/lib$(LIB_NAME).a $(OBJ_STATIC)/bstrutils.o
	gcc $(SRC_DIR)/main.c -L$(STATIC_LIB) -l$(LIB_NAME) -o $(BIN_DIR)/$(PROGRAM_STATIC) -I$(INC_DIR)

shared: $(SRC_DIR)/bstrutils.c $(SRC_DIR)/main.c
	gcc -c -fPIC $(SRC_DIR)/bstrutils.c -o $(OBJ_SHARED)/bstrutils.o -I$(INC_DIR)
	gcc -shared $(OBJ_SHARED)/bstrutils.o -o $(SHARED_LIB)/lib$(LIB_NAME).so
	gcc $(SRC_DIR)/main.c -L$(SHARED_LIB) -l$(LIB_NAME) -o $(BIN_DIR)/main_shared -I$(INC_DIR)

all: static shared

clean:
	rm -rf $(OBJ_SHARED)/*.o
	rm -rf $(OBJ_STATIC)/*.o
	rm -rf $(STATIC_LIB)/*.a 
	rm -rf $(SHARED_LIB)/*.so
	rm -rf $(BIN_DIR)/*
