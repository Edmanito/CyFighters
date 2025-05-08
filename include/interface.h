#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "data.h"

// === Constantes interface ===
#define LARGEUR_FENETRE 1280
#define HAUTEUR_FENETRE 720
#define ECARTEMENT_PONT 40

typedef enum {
    PAGE_MENU,
    PAGE_SELEC_MODE,
    PAGE_SELEC_DIFFICULTE,
    PAGE_SELECTION_PERSO,
    PAGE_CONFIRMATION_PERSO,
    PAGE_COMBAT,
    PAGE_OPTIONS,
    PAGE_HISTOIRE,
    PAGE_QUITTER
} Page;

// === Variables interface ===
extern int mode_choisi;
extern int chemin_retour;
extern int musique_lancee;
extern int perso_choisi;
extern Mix_Music* musique_global;

// === Fonctions d'affichage des pages ===
Page afficher_chargement(SDL_Renderer* rendu);
Page afficher_histoire(SDL_Renderer* rendu);
Page afficher_menu(SDL_Renderer* rendu);
Page afficher_selec_mode(SDL_Renderer* rendu);
Page afficher_selec_difficulte(SDL_Renderer* rendu);
Page afficher_selection_perso(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_confirmation_perso(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_jeu(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_options(SDL_Renderer* rendu, Page préc);

// === Fonctions de rendu/animation ===
void renduJeu(SDL_Renderer* rendu);
void animationNouveauTour(SDL_Renderer* renderer, int numeroTour);
SDL_Rect get_rect_fighter(Fighter* f);
void jouerAnimationAttaque(SDL_Renderer* renderer, int type, SDL_Rect lanceur, SDL_Rect cible, ElementType element);

// === Utilitaires interface ===
void jouer_musique(const char* chemin, int volume);
void charger_langue(const char* chemin);
SDL_Texture* generer_texte(SDL_Renderer* rendu, const char* texte, TTF_Font* police);
SDL_Texture* generer_texte_couleur(SDL_Renderer* rendu, const char* texte, TTF_Font* police, SDL_Color couleur);

#endif // INTERFACE_H
