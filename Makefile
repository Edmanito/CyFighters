CC = gcc
CFLAGS = -Wall -IInclude `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC = Source/main.c \
      Source/menu.c  # Assure-toi que menu.c est ici

EXEC = exec/jeu

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p exec
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilation réussie : $(EXEC)"

compile: clean all

jeu: compile
	@echo "🎮 Lancement du jeu..."
	./$(EXEC)

clean:
	@echo "🧹 Nettoyage..."
	rm -f $(EXEC)
