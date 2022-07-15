CC = g++
LD = g++
SUFFIX = .cpp
CUR_DIR = .
BIN_DIR = ./bin
DEC_DIR = $(CUR_DIR)/decoder
ENC_DIR = $(CUR_DIR)/encoder
LCC_DIR = $(CUR_DIR)/LDPCc

INC_DIR	= -I$(LCC_DIR) \
			-I$(DEC_DIR) \
			-I$(ENC_DIR)

SRC = 	$(wildcard $(LCC_DIR)/*$(SUFFIX)) \
		$(wildcard $(DEC_DIR)/*$(SUFFIX)) \
		$(wildcard $(ENC_DIR)/*$(SUFFIX)) \
		main.cpp

OBJ = $(patsubst %$(SUFFIX),%.o, $(SRC))

DEBUG := -O3 -DNDEBUG
# choose if enable debug
# when enable, close optimization
debug ?= n
ifeq ($(debug),y)
DEBUG := -g 
endif

CLFAGS = $(DEBUG) $(INC_DIR)
TARGET = $(BIN_DIR)/ldpc

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ --verbose -Wall
	@echo "Compile done."

$(OBJ):%.o:%$(SUFFIX)
	@echo "compiling $< --> $@"
	$(CC) $(CLFAGS) -c $< -o $@

all:
	@echo $(INC_DIR)
	@echo $(SRC)
	@echo $(OBJ)

clean:
	rm -f $(OBJ)
	@echo clean all obj done

cleanall: clean
	rm -f $(TARGET)
	@echo clean target

# assign simulation output file
enc :=
out ?=
outfile :=
ifneq ($(out),)
outfile := >> $(out).log
endif

# run 4x8 base matrix size code
run:
	@$(TARGET) \
		--len 2048 \
		-r 1024 \
		-b 1027 \
		-d0 \
		-i40 \
		-a625 \
		-C 8 \
		-V 4 \
		-G 256 \
		-x$(CUR_DIR)/matrix/G_mat.csv \
		-c$(CUR_DIR)/matrix/CNsCon.csv \
		-v$(CUR_DIR)/matrix/VNsCon.csv \
		-s 1.9 \
		-S 0.2 \
		-E 2.5 \
		$(outfile)

help:
	@$(TARGET) --help


# include other code
CODE_DIR     = $(CUR_DIR)/code-conf
CODEMKFILE   = $(CODE_DIR)/makefile.

# 6x8 code
include $(CODEMKFILE)257_6x8
include $(CODEMKFILE)257_6x12
include $(CODEMKFILE)257_6x16
include $(CODEMKFILE)257_6x20
include $(CODEMKFILE)257_6x24

# 4x20
# include $(MKFILE)257_4x20


