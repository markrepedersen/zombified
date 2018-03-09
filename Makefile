CXXFLAGS = -Iext/gl3w -std=c++11 -Wall -pedantic -DENABLE_SOUND
LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain

BIN=main
OBJ=$(BIN).o common.o legs.o arms.o water.o freeze.o missile.o bomb.o armour.o player1.o player2.o toolboxSlot.o toolboxManager.o world.o button.o worldtexture.o viewHelper.o antidote.o MapGrid.o zombie.o

default: build
	
build: $(BIN)

test: build
	./$(BIN)

%.o: src/%.cpp
	clang++ -c $(CXXFLAGS) -o $@ $<

$(BIN): $(OBJ)
	clang++ -o $@ $(OBJ) $(LIB)

clean:
	- rm -f $(BIN) $(OBJ)
