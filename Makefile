CC = gcc
CFLAGS = -Wall -Iinclude `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

SRC = source/main.c source/menu.c source/texte.c source/langue.c
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
