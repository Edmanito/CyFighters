CC       = gcc
CFLAGS   = -Wall -Iinclude `sdl2-config --cflags`
LIBS     = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC = \
    source/attaque.c \
    source/data.c \
    source/interface.c \
    source/jeu.c \
    source/langue.c \
    source/logique.c \
    source/main.c \
    source/maps.c \
    source/menu.c \
    source/select_perso.c\
    source/son.c



EXEC     = exec/jeu

.PHONY: all compile jeu clean

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p exec
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilation réussie : $(EXEC)"

compile: clean all

jeu: compile
	@echo "🎮 Lancement du jeu..."
	@./$(EXEC)

clean:
	@echo "🧹 Nettoyage..."
	@rm -f $(EXEC)