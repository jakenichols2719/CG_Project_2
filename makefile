EXE=run

# Targets
default: $(EXE)

# Flags
# Msys/MinGW
ifeq "$(OS)" "Windows_NT"
FLG=-O3 -Wall -DUSEGLEW
LIBS=-lglfw3 -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.a *.o
else
# OSX
ifeq "$(shell uname)" "Darwin"
FLG=-O3 -Wall -Wno-deprecated-declarations -DUSEGLEW
LIBS=-lglfw3 -lglew -framework Cocoa -framework OpenGL -framework IOKit
# Linux/Unix/Solaris
else
FLG=-O3 -Wall
LIBS=-lglfw -lGLU -lGL -lm
endif
# OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.a *.o
endif

# Object files
run.o: run.cpp jngl.h

# Create program
jngl.a:
	make clean -C jngl
	make -C jngl
$(EXE): run.o jngl.a
	g++ -o $@ $^ $(LIBS)

# C++ compile rule
.cpp.o:
	g++ -c $(FLG) $<

clean:
	$(CLEAN)