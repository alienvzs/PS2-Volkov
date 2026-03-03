EE_BIN = volkov.elf
EE_OBJS = volkov.o
EE_LIBS = -lgskit -ldmakit -lpad -ldebug

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

