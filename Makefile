include ../MakeInclude

BUILD_DIRS = SMLib SML CompileSML InterpretSML SMLTest TestSuite

.PHONY: default $(BUILD_DIRS)

default: $(BUILD_DIRS)

$(BUILD_DIRS):
	cd $@ && $(MAKE)

clean :
	@for i in $(BUILD_DIRS); do \
             echo " "; \
             echo "###################################"; \
             echo "Cleaning library `basename $$i` ..."; \
             echo "###################################"; \
             $(MAKE) clean -C $$i; \
        done
