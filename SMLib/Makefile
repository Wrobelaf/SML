include ../../MakeInclude

SRC = Eval.cpp \
      instructions.cpp \
      Main.cpp \
      operand.cpp \
      Token.cpp \
      SymbolConstant.cpp \
      Symbol.cpp \
      SymbolLine.cpp \
      SymbolTable.cpp \
      SymbolVariable.cpp

OBJ = $(SRC:%.cpp=$(CONFIG)/%.o)

libSMLib.so : $(OBJ)
	$(CC) $(LFLAGS) -shared $(OBJ) -o $(CONFIG)/$@;
	
clean:
	rm -fR $(CONFIG)/* *.*~

ifneq ($(MAKECMDGOALS),clean)
include $(SRC:%.cpp=$(CONFIG)/%.d)
endif
