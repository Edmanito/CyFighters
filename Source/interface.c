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

// Génération d'une texture SDL pour du texte non coloré
SDL_Texture* generer_texte(SDL_Renderer* rendu, const char* texte, TTF_Font* police) {
    SDL_Color couleur = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rendu, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Génération d'une texture SDL pour du texte coloré
SDL_Texture* generer_texte_couleur(SDL_Renderer* rendu,
                                   const char* texte,
                                   TTF_Font* police,
                                   SDL_Color couleur) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rendu, surface);
    SDL_FreeSurface(surface);
    return texture;
}
