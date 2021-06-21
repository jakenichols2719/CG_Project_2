LIB=jngl.a

# Targets
lib: $(LIB)

# Flags
# Msys/MinGW
ifeq "$(OS)" "Windows_NT"
FLG=-O3 -Wall -DUSEGLEW
LIBS=-lglfw3 -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.o *.a
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
CLEAN=rm -f *.o *.a
endif

# Object files
# Library
material.o: material.cpp jngl.h
loadobj.o: loadobj.cpp jngl.h
texture.o: texture.cpp jngl.h
shader.o: shader.cpp jngl.h
window.o: window.cpp jngl.h
camera.o: camera.cpp jngl.h
debug.o: debug.cpp jngl.h
vec.o: vec.cpp jngl.h
mat.o: mat.cpp jngl.h
vbo.o: vbo.cpp jngl.h

# Create library
$(LIB): material.o loadobj.o texture.o shader.o window.o camera.o debug.o vec.o mat.o vbo.o shader.o
	ar -rcs $@ $^
	cp $(LIB) jngl.h ..

# C++ compile rules
.cpp.o:
	g++ -c $(FLG) $<

# Clean
clean:
	$(CLEAN)