
CXXFLAGS+= -ggdb3
override CXXFLAGS+= -Wall -fsigned-char -fno-exceptions

OBJECTS = \
	game.o \
	gui.o \
	main.o \
	render.o \
	rendertext.o \
	texture.o \
	tools.o \
	world.o

HEADERS = \
	engine.h \
	tools.h

LIBS = -lX11 `sdl-config --libs` -lSDL_image -lGL -lz
INCLUDES = `sdl-config --cflags`

$(OBJECTS): CXXFLAGS+= $(INCLUDES)

default: all

all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(INCLUDES) $(LIBS) -o roguey

install: all
	mv roguey ../

clean:
	$(RM) $(OBJECTS)

#dependencies

$(OBJECTS) : $(HEADERS)