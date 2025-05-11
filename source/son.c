// ----- Source/son.c -----

#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "son.h"

// Variable utilisée éventuellement ailleurs (ex : éviter de relancer une musique déjà lancée)
int musique_selection_jouee = 0;

// Pointeur vers la musique en cours
static Mix_Music* musique_actuelle = NULL;

// Mémorise le chemin de la musique actuelle (utile pour ne pas relancer la même)
static char chemin_actuel[256] = "";

// Lance une musique (en boucle)
void jouerMusique(const char* chemin, int volume) {
    // Si une musique est déjà en cours, on l'arrête
    if (musique_actuelle) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
        chemin_actuel[0] = '\0';
    }

    // Charge la nouvelle musique
    musique_actuelle = Mix_LoadMUS(chemin);
    if (!musique_actuelle) {
        fprintf(stderr, "Erreur chargement musique : %s\n", Mix_GetError());
        return;
    }

    // Enregistre le chemin
    strncpy(chemin_actuel, chemin, sizeof(chemin_actuel) - 1);
    chemin_actuel[sizeof(chemin_actuel) - 1] = '\0';

    // Régle le volume et lance en boucle
    Mix_VolumeMusic(volume);
    Mix_PlayMusic(musique_actuelle, -1);
}

// Stoppe la musique uniquement si c’est celle demandée
void arreter_musique(const char* chemin) {
    if (!musique_actuelle) return;

    if (strcmp(chemin, chemin_actuel) == 0) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
        chemin_actuel[0] = '\0';
    }
}

// Joue un petit effet sonore (clic, bruitage...)
void jouer_effet(const char* chemin, int volume) {
    Mix_Chunk* effet = Mix_LoadWAV(chemin);
    if (!effet) {
        fprintf(stderr, "Erreur chargement effet : %s\n", Mix_GetError());
        return;
    }

    Mix_VolumeChunk(effet, volume);
    Mix_PlayChannel(-1, effet, 0);

    SDL_Delay(300); // Laisse le temps au son d’être entendu

    Mix_FreeChunk(effet);
}
