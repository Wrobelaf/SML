include ../../MakeInclude
include ../SMLInclude

SRC = CompileEval.cpp \
      CompilerInstructions.cpp \
      CompilerInstructionTable.cpp \
      CompileSML.cpp
      
OBJ = $(SRC:%.cpp=$(CONFIG)/%.o)

CompileSML : $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(SML_LIBS) -lUseful -lSMLib -lSML -o $(CONFIG)/$@
	
clean:
	rm -fR $(CONFIG)/* *.*~

ifneq ($(MAKECMDGOALS),clean)
include $(SRC:%.cpp=$(CONFIG)/%.d)
endif

