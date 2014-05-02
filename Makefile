TARGET = PSP-Fighter
OBJS = main.o 

PSPSDK = $(shell psp-config --pspsdk-path)
PSPDEV = $(shell psp-config -d)
PSPBIN = $(PSPDEV)/bin
SDL_CONFIG = $(PSPBIN)/sdl-config

CFLAGS = -O2 -G0 -Wall -D PSP
CFLAGS += $(shell $(SDL_CONFIG) --cflags)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lstdc++ -lSDL_ttf -lSDL_gfx -lfreetype -lSDL_image -ljpeg -lpng -lz -lm -lSDL_mixer -lvorbisidec -lmikmod -lmad $(shell $(SDL_CONFIG) --libs)
LIBS += -lpspaudiolib -lpspaudio -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP Fighter 1.0

include $(PSPSDK)/lib/build.mak