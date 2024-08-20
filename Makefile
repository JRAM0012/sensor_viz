CC = gcc
INCLUDES = -IC:\raylib\raylib\src -Iinclude
LIBRARIES = -LC:\raylib\raylib\src -lraylib -lgdi32 -lopengl32 -lwinmm 
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

# run: bin\sensor_viz
# @clear
# @bin\sensor_viz

bin\sensor_viz.exe: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBRARIES)

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(INCLUDES)

create_dirs:
	@-mkdir obj
	@-mkdir bin

-include $(DEPS)

run: bin\sensor_viz.exe
	@bin\sensor_viz.exe

clean:
	rm -rf obj/*.o bin/sensor_viz.exe