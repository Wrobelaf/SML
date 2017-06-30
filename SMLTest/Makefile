include ../../MakeInclude
include ../SMLInclude

SRC = SMLTest_unix.cpp
      
OBJ = $(SRC:%.cpp=$(CONFIG)/%.o)

SMLTest : $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(SML_LIBS) -lUseful -o $(CONFIG)/$@
	
clean:
	rm -fR $(CONFIG)/* *.*~

ifneq ($(MAKECMDGOALS),clean)
include $(SRC:%.cpp=$(CONFIG)/%.d)
endif

