include ../../MakeInclude
include ../SMLInclude

SRC = machine.cpp machine_state.cpp memory.cpp SML.cpp
OBJ = $(SRC:%.cpp=$(CONFIG)/%.o)

SML : $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(SML_LIBS) -lUseful -lSMLib -o $(CONFIG)/$@
	$(CC) $(LFLAGS) -shared $(OBJ) -o $(CONFIG)/libSML.so
	
clean:
	rm -fR $(CONFIG)/* *.*~

ifneq ($(MAKECMDGOALS),clean)
include $(SRC:%.cpp=$(CONFIG)/%.d)
endif
