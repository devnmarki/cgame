PROJECTNAME = project
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinc
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL3 -lSDL3_image

SRC = $(wildcard src/*.cpp)

all: $(OUTPUT_DIR)/$(PROJECTNAME)
	./$(OUTPUT_DIR)/$(PROJECTNAME).exe

$(OUTPUT_DIR)/$(PROJECTNAME):
	g++ $(SRC) -o $(OUTPUT_DIR)/$(PROJECTNAME) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)

clean:
	rm -f $(OUTPUT_DIR)/*