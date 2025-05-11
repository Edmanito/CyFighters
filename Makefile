CC       = gcc
CFLAGS   = -Wall -Iinclude `sdl2-config --cflags`
LIBS     = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SRC = \
    source/attaque.c \
    source/data.c \
    source/interface.c \
    source/affichage.c \
    source/jeu.c \
    source/langue.c \
    source/main.c \
    source/menu.c \
    source/select_perso.c \
    source/son.c

EXEC     = exec/jeu

.PHONY: all compile jeu clean valgrind

# Cible par défaut, elle compile l'exécutable
all: $(EXEC)

# Compilation de l'exécutable
$(EXEC): $(SRC)
	@mkdir -p exec
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Compilation réussie : $(EXEC)"

# Cible pour nettoyer les fichiers compilés et recompiler
compile: clean all

# Cible pour lancer le jeu
jeu: compile
	@echo "🎮 Lancement du jeu..."
	@./$(EXEC)

# Cible pour nettoyer les fichiers compilés
clean:
	@echo "🧹 Nettoyage..."
	@rm -f $(EXEC)

# Cible pour vérifier les fuites de mémoire avec valgrind
valgrind: $(EXEC)
	@echo "🎮 Lancement du jeu avec Valgrind..."
	@valgrind --leak-check=full --track-origins=yes ./$(EXEC)
