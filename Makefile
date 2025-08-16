PROJECTNAME = cgame
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinc
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC = $(wildcard src/*.cpp)

all: $(OUTPUT_DIR)/$(PROJECTNAME)
	./$(OUTPUT_DIR)/$(PROJECTNAME).exe

$(OUTPUT_DIR)/$(PROJECTNAME):
	g++ $(SRC) -o $(OUTPUT_DIR)/$(PROJECTNAME) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)

clean:
	rm -f $(OUTPUT_DIR)/*