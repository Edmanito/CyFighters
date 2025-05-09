#include "attaque.h"
#include "data.h"
#include "interface.h"
#include "son.h"

#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

SDL_Window* fenetre;

int main(int argc, char* argv[]) {
    SDL_Texture* selections_j1[NB_PERSOS_EQUIPE] = { NULL };
    SDL_Texture* selections_j2[NB_PERSOS_EQUIPE] = { NULL };
    Page page = PAGE_MENU;
    int quit = 0;
    int musique_menu_jouee = 0;

    // SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return 1;
    }

    // TTF
    if (TTF_Init() != 0) {
        printf("Erreur TTF : %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // MIXER
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        printf("Erreur Audio : %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Fenêtre
    fenetre = SDL_CreateWindow("Project Shōnen Smash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               LARGEUR_FENETRE, HAUTEUR_FENETRE, SDL_WINDOW_SHOWN);
    if (!fenetre) {
        printf("Erreur fenêtre : %s\n", SDL_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Renderer
    SDL_Renderer* rendu = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rendu) {
        printf("Erreur renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Chargement
    page = afficher_chargement(rendu);

    // Boucle principale
    while (!quit) {
        switch (page) {
            case PAGE_MENU:
                if (!musique_menu_jouee) {
                    jouerMusique("ressource/musique/ogg/menu/menu_1.ogg", 20);
                    musique_menu_jouee = 1;
                }
                page = afficher_menu(rendu);
                break;

            case PAGE_SELEC_MODE:
                page = afficher_selec_mode(rendu);
                break;

            case PAGE_SELEC_DIFFICULTE:
                page = afficher_selec_difficulte(rendu);
                break;

            case PAGE_SELECTION_PERSO: {
                SDL_Texture* old_j1[NB_PERSOS_EQUIPE] = { selections_j1[0], selections_j1[1], selections_j1[2] };
                SDL_Texture* old_j2[NB_PERSOS_EQUIPE] = { selections_j2[0], selections_j2[1], selections_j2[2] };

                page = afficher_selection_perso(rendu, selections_j1, selections_j2);
                if (page == PAGE_CONFIRMATION_PERSO) {
                    page = afficher_confirmation_perso(rendu, selections_j1, selections_j2);
                    if (page == PAGE_SELECTION_PERSO) {
                        for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
                            selections_j1[i] = old_j1[i];
                            selections_j2[i] = old_j2[i];
                        }
                    }
                }
                break;
            }

            case PAGE_COMBAT:
                jouerMusique("ressource/musique/ogg/jeu/combat_1.ogg", 20);
                musique_menu_jouee = 0;
                page = afficher_jeu(rendu, selections_j1, selections_j2);
                break;

            case PAGE_OPTIONS:
                page = afficher_options(rendu, PAGE_MENU);
                break;

            case PAGE_HISTOIRE:
                arreter_musique();
                
                musique_menu_jouee = 0;
                page = afficher_histoire(rendu);
                break;

            case PAGE_QUITTER:
                quit = 1;
                break;

            default:
                printf("Page inconnue : %d\n", page);
                quit = 1;
                break;
        }

        SDL_Delay(5); // Petit délai pour éviter de trop surcharger le CPU/audio
    }

    // Libération des textures
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
        if (selections_j1[i]) SDL_DestroyTexture(selections_j1[i]);
        if (selections_j2[i]) SDL_DestroyTexture(selections_j2[i]);
    }

    arreter_musique();

    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(fenetre);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
