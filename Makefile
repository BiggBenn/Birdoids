CC = g++
CFLAGS = -O2 -std=c++17
EXE = Birdoids

OBJ_DIR = ./obj
INCLUDES = -Iother

OBJS:= Birdoids.o Game.o GameObject.o Player.o Projectile.o Bird.o
OBJ_DEPS = $(patsubst %,$(OBJ_DIR)/%,$(OBJS))

build: dir_obj $(EXE)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -c -o $@

$(EXE): $(OBJ_DEPS)
	$(CC) $(CFLAGS) $^ -o ./$@ -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

dir_obj:
	mkdir -p $(OBJ_DIR)

.PHONY: clean
.PHONY: dir_obj

clean:
	rm -rf ./obj