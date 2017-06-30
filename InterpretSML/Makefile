include ../../MakeInclude
include ../SMLInclude

SRC = InterpretEval.cpp \
      InterpretSML.cpp
       
OBJ = $(SRC:%.cpp=$(CONFIG)/%.o)

InterpretSML : $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(SML_LIBS) -lUseful -lSMLib -lSML -o $(CONFIG)/$@

clean:
	rm -fR $(CONFIG)/*

ifneq ($(MAKECMDGOALS),clean)
include $(SRC:%.cpp=$(CONFIG)/%.d)
endif

