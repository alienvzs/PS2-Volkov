EE_BIN = volkov.elf
EE_OBJS = volkov.o
EE_LIBS = -lfileXio -lpad -lgskit -ldmakit -lgskit_font

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
