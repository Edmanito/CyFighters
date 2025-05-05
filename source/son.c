// ----- Source/son.c -----
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include "son.h"

static Mix_Music* musique_actuelle = NULL;

void jouerMusique(const char* chemin, int volume) {
    if (musique_actuelle) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
    }

    musique_actuelle = Mix_LoadMUS(chemin);
    if (!musique_actuelle) {
        fprintf(stderr, "Erreur chargement musique : %s\n", Mix_GetError());
        return;
    }

    Mix_VolumeMusic(volume);
    Mix_PlayMusic(musique_actuelle, -1);  // -1 = boucle infinie
}

void arreter_musique() {
    if (musique_actuelle) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_actuelle);
        musique_actuelle = NULL;
    }
}

void jouer_effet(const char* chemin, int volume) {
    Mix_Chunk* effet = Mix_LoadWAV(chemin);
    if (!effet) {
        fprintf(stderr, "Erreur chargement effet : %s\n", Mix_GetError());
        return;
    }

    Mix_VolumeChunk(effet, volume);
    Mix_PlayChannel(-1, effet, 0); // 0 = jouer une fois

    // Libération immédiate après lecture
    Mix_FreeChunk(effet);
}
