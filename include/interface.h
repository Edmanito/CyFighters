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
// Taille par défaut de la fenêtre (modifiable si tu veux un redimensionnement dynamique)

// === Enumération des différentes pages de ton jeu ===
// Permet de gérer le changement d'écran dans ta boucle principale
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

// === Variables partagées par l'interface ===
// Ces variables sont souvent utilisées dans les fonctions d'affichage
extern int mode_choisi;
extern int chemin_retour;      // Navigation entre pages
extern int musique_lancee;     // Pour éviter de relancer la musique plusieurs fois
extern int perso_choisi;       // Index ou booléen de sélection active ?
extern Mix_Music* musique_global; // Musique en cours
extern int ecartementPont;     // Espace entre les persos ou éléments graphiques
extern TTF_Font* policePrincipale;
extern TTF_Font* policePetite;

// === Fonctions d’affichage de chaque page ===
// Chaque fonction retourne la prochaine page à afficher
Page afficher_chargement(SDL_Renderer* rendu);
Page afficher_histoire(SDL_Renderer* rendu);
Page afficher_menu(SDL_Renderer* rendu);
Page afficher_selec_mode(SDL_Renderer* rendu);
Page afficher_selec_difficulte(SDL_Renderer* rendu);
Page afficher_selection_perso(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_confirmation_perso(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_jeu(SDL_Renderer* rendu, SDL_Texture* sel1[NB_PERSOS_EQUIPE], SDL_Texture* sel2[NB_PERSOS_EQUIPE]);
Page afficher_options(SDL_Renderer* rendu, Page préc); // "préc" = page précédente (permet retour contextuel)

// === Fonctions de rendu pendant le combat ===
void renduJeu(SDL_Renderer* rendu); // Rafraîchit la scène de combat
void animationNouveauTour(SDL_Renderer* renderer, int numeroTour); // Affiche une animation visuelle entre les tours
SDL_Rect get_rect_fighter(Fighter* f); // Donne la position à l’écran du personnage f
void jouerAnimationAttaque(SDL_Renderer* renderer, int type, SDL_Rect lanceur, SDL_Rect cible, ElementType element);

// === Utilitaires pour l'audio et le texte ===
void jouer_musique(const char* chemin, int volume); // Joue une musique avec volume ajusté
void charger_langue(const char* chemin); // (Prévu pour charger les traductions multilingues)

void charger_polices();  // Initialise policePrincipale / policePetite
void liberer_polices();  // Libère proprement les polices TTF

void liberer_texte(SDL_Texture* texture); // Libère une texture de texte

// Génère une texture SDL à partir d’un texte
SDL_Texture* generer_texte(SDL_Renderer* rendu, const char* texte, TTF_Font* police);
SDL_Texture* generer_texte_couleur(SDL_Renderer* rendu, const char* texte, TTF_Font* police, SDL_Color couleur);

// Fonction redondante ici (elle est déjà dans data.h normalement)
bool equipe_est_morte(int equipe);

#endif // INTERFACE_H