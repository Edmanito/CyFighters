// ----- Source/son.c -----
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include "son.h"


int musique_selection_jouee = 0;


static Mix_Music* musique_actuelle = NULL;

static char chemin_actuel[256] = ""; // stocke le chemin actuel

void jouerMusique(const char* chemin, int volume) {
    if (musique_actuelle) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
        chemin_actuel[0] = '\0';
    }

    musique_actuelle = Mix_LoadMUS(chemin);
    if (!musique_actuelle) {
        fprintf(stderr, "Erreur chargement musique : %s\n", Mix_GetError());
        return;
    }

    // ⬅️ Met à jour le chemin actif
    strncpy(chemin_actuel, chemin, sizeof(chemin_actuel) - 1);
    chemin_actuel[sizeof(chemin_actuel) - 1] = '\0';

    Mix_VolumeMusic(volume);
    Mix_PlayMusic(musique_actuelle, -1);  // -1 = boucle infinie
}

void arreter_musique(const char* chemin) {
    if (!musique_actuelle) return;

    if (strcmp(chemin, chemin_actuel) == 0) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
        chemin_actuel[0] = '\0';
    }
}

void jouer_effet(const char* chemin, int volume) {
    Mix_Chunk* effet = Mix_LoadWAV(chemin);
    if (!effet) {
        fprintf(stderr, "Erreur chargement effet : %s\n", Mix_GetError());
        return;
    }

    Mix_VolumeChunk(effet, volume);
    Mix_PlayChannel(-1, effet, 0);
    SDL_Delay(300); // attend un peu (juste pour tester)
    Mix_FreeChunk(effet);
    
}