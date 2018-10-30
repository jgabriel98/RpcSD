SHELL = /bin/sh
CC=gcc				#C compiler
CPPC=g++			#C++ compiler
CFLAGS=-std=c++14 -pthread -lrpc	#-I/home/aluno/Música/bin-rpclib/include -L/home/aluno/Música/bin-rpclib/lib	#standard g++ flags

#CFLAGS=-std=c++11 `pkg-config --cflags protobuf grpc` 	#standard g++ flags
#LDFLAGS += `pkg-config --libs protobuf grpc++ grpc`

ECHOMSG := "Normal compiling mode\n"


# src dir
SRCDIR =RPC_SD
# Binary dir
BDIR =bin

# object files ( .o)
ODIR = $(BDIR)
_OBJ = Node.o Main.o HostNode.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# headers
_DEPS = Node.h Headers.h HostNode.h color.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

#target .c files
$(BDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${RED}$(subst $(SRCDIR)/,,$<) ${NC}"
#target .cpp files
$(BDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${YELLOW}$(subst $(SRCDIR)/,,$<) ${NC}"
#target .cc files
$(BDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${YELLOW}$(subst $(SRCDIR)/,,$<) ${NC}"


# normal compiling mode
#default: ECHOMSG := "Normal compiling mode\n"
default: $(OBJ)
	@echo "$(BL_YELLOW)Linking files$(NC)"
	@$(CPPC) $^ -o Prog.exe $(CFLAGS)$(EXTFLG)
	@printf $(ECHOMSG)
	@echo "${BL_GREEN}Done${NC}"

#fast, optimized compiling mode
fast: opt2
opt3: EXTFLG = -O3 -march=native -Wno-unused-result
opt3: ECHOMSG = "fast, optimized compiling mode level 3\n"
opt3: default

opt2: EXTFLG = -O2 -march=native -Wno-unused-result
opt2: ECHOMSG = "fast, optimized compiling mode level 2\n"
opt2: default

opt1: EXTFLG = -O1 -march=native -Wno-unused-result
opt1: ECHOMSG = "fast, optimized compiling mode level 1\n"
opt1: default

debug: EXTFLG = -ggdb3 -O0 -fno-inline-functions -DGLIBCXX_DEBUG -rdynamic
debug: ECHOMSG = "Debugging compiling mode \n"
debug: default

all:
	@touch $(SRCDIR)/*
	@echo 'touching all src files'

clean: 
	@find -name '*.o' -delete
	@echo 'deleted .o files'




#  .---------- COLORS FOR ECHO---------. #
##########################################

#  Regular
BLACK = \033[0;30m#Black
RED = \033[0;31m#Red
GREEN = \033[0;32m#Green
YELLOW = \033[0;33m#Yellow
BLUE = \033[0;34m#Blue
PURPLE = \033[0;35m#Purple
CYAN = \033[0;36m#Cyan
WHITE = \033[0;37m#White


NC = \033[0m # o Color -> reset the color


#  Bold

B_BLACK = \033[1;30m
B_RED = \033[1;31m
B_GREEN = \033[1;32m
B_YELLOW = \033[1;33m
B_BLUE = \033[1;34m
B_PURPLE = \033[1;35m
B_CYAN = \033[1;36m
B_WHITE = \033[1;37m


#Light / high intesity

L_BLACK = \033[0;90m
L_RED = \033[0;91m
L_GREEN = \033[0;92m
L_YELLOW = \033[0;93m
L_BLUE = \033[0;94m
L_PURPLE = \033[0;95m
L_CYAN = \033[0;96m
L_WHITE = \033[0;97m

# Bold light/hight intesity

BL_BLACK = \033[1;90m
BL_RED = \033[1;91m
BL_GREEN = \033[1;92m
BL_YELLOW = \033[1;93m
BL_BLUE = \033[1;94m
BL_PURPLE = \033[1;95m
BL_CYAN = \033[1;96m
BL_WHITE = \033[1;97m
