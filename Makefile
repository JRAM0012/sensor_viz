CC = gcc
INLCUDES = -IC:\raylib\raylib\src
LIBRARIES = -LC:\raylib\raylib\src -lraylib -lgdi32 -lopengl32 -lwinmm 
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

# run: bin\sensor_viz
# @clear
# @bin\sensor_viz

bin\sensor_viz: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBRARIES)

obj/%.o: src/%.c
	$(CC) -c -o $@ $^ $(INLCUDES)

create_dirs:
	@-mkdir obj
	@-mkdir bin
