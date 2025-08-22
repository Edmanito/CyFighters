// ----- Source/interface.c -----

#include "interface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// NB : les variables globales comme mode_choisi, chemin_retour, musique_lancee, musique_global, perso_choisi sont définies dans menu.c

// ================================
// Chargement des textes localisés (multilangue)
// ================================
void charger_langue(const char* chemin) {
    // À implémenter : lecture des fichiers JSON/texte pour charger les chaînes traduites
}

// ================================
// Chargement des polices (Arial ici)
// ================================
TTF_Font* policePrincipale = NULL;
TTF_Font* policePetite = NULL;

void charger_polices() {
    policePrincipale = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    policePetite = TTF_OpenFont("ressource/langue/police/arial.ttf", 22);

    if (!policePrincipale || !policePetite) {
        SDL_Log("Erreur lors de l'ouverture de la police : %s", TTF_GetError());
        exit(1);  // Quitte le jeu si les polices ne sont pas chargées
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


// ================================
// Génération de texte en texture SDL
// ================================


SDL_Texture* generer_texte(SDL_Renderer* rendu, const char* texte, TTF_Font* police) {
    if (!texte || !police || !rendu) {
        SDL_Log("Erreur : paramètre nul dans generer_texte");
        return NULL;
    }

    SDL_Color couleur = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (!surface) {
        SDL_Log("Erreur lors de la création de la surface texte : %s", TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(rendu, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Erreur lors de la création de la texture texte : %s", SDL_GetError());
    }

    return texture;
}

// Libère une texture SDL de texte
void liberer_texte(SDL_Texture* texture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

// ================================
// Vérifie si une équipe est morte (tous les persos à 0 PV)
// ================================
bool equipe_est_morte(int equipe) {
    if(equipe == 1){
        for (int i = 0; i < 3; i++) {
            Fighter* p = get_fighter(i);
            if (p->actu_pv > 0) return false;
        }
        return true;
    }
    else{
        for (int i = 3; i < 6; i++) {
            Fighter* p = get_fighter(i);
            if (p->actu_pv > 0) return false;
        }
        return true;
    }
}