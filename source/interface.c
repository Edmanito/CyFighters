// ----- Source/interface.c -----
#include "interface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// NB : les globals mode_choisi, chemin_retour, musique_lancee,
//     musique_global et perso_choisi sont définis dans menu.c

// Lecture et gestion de la musique


// Chargement des fichiers de langue/textes
void charger_langue(const char* chemin) {
    // TODO: Lire un fichier texte ou JSON pour charger les chaînes localisées
}

TTF_Font* policePrincipale = NULL;
TTF_Font* policePetite = NULL;


// Génération d'une texture SDL pour du texte coloré
SDL_Texture* generer_texte(SDL_Renderer* rendu, const char* texte, TTF_Font* police) {
    if (!texte || !police || !rendu) {
        SDL_Log("generer_texte : paramètre nul !");
        return NULL;
    }

    SDL_Color couleur = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (!surface) {
        SDL_Log("Erreur TTF_RenderUTF8_Blended : %s", TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(rendu, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
    }

    return texture;
}

// Libérer une texture
void liberer_texte(SDL_Texture* texture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}


void charger_polices() {
    policePrincipale = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    policePetite = TTF_OpenFont("ressource/langue/police/arial.ttf", 22);

    if (!policePrincipale || !policePetite) {
        SDL_Log("Erreur ouverture police : %s", TTF_GetError());
        exit(1);
    }
}


void liberer_polices() {
    if (policePrincipale) {
        TTF_CloseFont(policePrincipale);
        policePrincipale = NULL;
    }
    if (policePetite) {
        TTF_CloseFont(policePetite);
        policePetite = NULL;
    }
}



bool equipe_est_morte(int equipe) {
    for (int i = 0; i < 3; i++) {
        Fighter* p = get_fighter(i);
        if (p->actu_pv > 0) return false;
    }
    return true;
}
