#include "data.h"
#include "logic.h"
#include "interface.h"


#include <string.h>
#include <time.h>
#include <stdbool.h>

Page afficher_selection_perso(SDL_Renderer* rendu, SDL_Texture* selections_j1[3], SDL_Texture* selections_j2[3]) {

    // --- CHARGEMENT DES TEXTURES ---
    SDL_Texture* fond_texture = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_selection_perso.png");
    if (!fond_texture) {
        SDL_Log("Erreur chargement fond: %s", SDL_GetError());
        return PAGE_QUITTER;
    }

    // --- LOGO VERSUS ---
    SDL_Texture* logo_versus_texture = IMG_LoadTexture(rendu, "Ressource/image/Utilité/versus.png");
    if (!logo_versus_texture) {
        SDL_Log("Erreur chargement logo: %s", SDL_GetError());
        SDL_DestroyTexture(fond_texture);
        return PAGE_QUITTER;
    }

    SDL_Rect dest_logo = {
        (LARGEUR_FENETRE - 80) / 2,
        10,
        80,
        80
    };

    // --- BOUTON RETOUR ---
    SDL_Texture* btn_retour_texture = IMG_LoadTexture(rendu, "Ressource/image/Utilité/retour.png");
    SDL_Rect btn_retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80}; // Nom de variable corrigé

    // --- PORTRAITS DES PERSONNAGES ---
    const char* noms_portraits[8] = {
        "Ressource/image/Personnages_pp/pp_droite/pp_darkshadow.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_hitsugaya.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_incassable.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_katara.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_kirua.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_rengoku.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_temari.png",
        "Ressource/image/Personnages_pp/pp_droite/pp_zoro.png"
    };

    SDL_Texture* portraits[8] = {NULL};
    bool perso_disponible[8] = {true, true, true, true, true, true, true, true};

    for (int i = 0; i < 8; i++) {
        portraits[i] = IMG_LoadTexture(rendu, noms_portraits[i]);
        if (!portraits[i]) {
            SDL_Log("Erreur chargement portrait %d: %s", i, SDL_GetError());
            for (int j = 0; j < i; j++) {
                if (portraits[j]) SDL_DestroyTexture(portraits[j]);
            }
            SDL_DestroyTexture(fond_texture);
            SDL_DestroyTexture(logo_versus_texture);
            if (btn_retour_texture) SDL_DestroyTexture(btn_retour_texture);
            return PAGE_QUITTER;
        }
    }

    // --- GESTION SELECTION ---
   
    int nb_selections_j1 = 0;
    int nb_selections_j2 = 0;
    bool tour_j1 = true;

    // --- POSITIONNEMENT ---
    const int largeur_perso = 150;
    const int hauteur_perso = 150;
    const int espacement_x = (LARGEUR_FENETRE - 4*largeur_perso) / 5;
    const int espacement_y = 50;
    const int start_y_persos = HAUTEUR_FENETRE - hauteur_perso - espacement_y;

    // --- MINI-PORTRAITS ---
    const int mini_size = 80;
    const int marge_haut = 20;
    const int marge_cote = 50;
    const int espacement_selections = 10;

    // --- TEXTE TOUR ---
    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 24);
    if (!police) {
        SDL_Log("Erreur chargement police: %s", TTF_GetError());
        for (int i = 0; i < 8; i++) {
            if (portraits[i]) SDL_DestroyTexture(portraits[i]);
        }
        SDL_DestroyTexture(fond_texture);
        SDL_DestroyTexture(logo_versus_texture);
        if (btn_retour_texture) SDL_DestroyTexture(btn_retour_texture);
        return PAGE_QUITTER;
    }

    SDL_Color couleur_blanc = {255, 255, 255, 255};
    SDL_Surface* surface_tour = NULL;
    SDL_Texture* texture_tour = NULL;

    // --- BOUCLE PRINCIPALE ---
    bool running = true;
    
    while (running && (nb_selections_j1 < 3 || nb_selections_j2 < 3)) {
        // Mise à jour texte tour
        if (surface_tour) SDL_FreeSurface(surface_tour);
        if (texture_tour) SDL_DestroyTexture(texture_tour);
        
        const char* texte_tour = tour_j1 ? "Tour du Joueur 1" : "Tour du Joueur 2";
        surface_tour = TTF_RenderText_Blended(police, texte_tour, couleur_blanc);
        if (!surface_tour) {
            SDL_Log("Erreur création surface texte: %s", TTF_GetError());
            continue;
        }
        
        texture_tour = SDL_CreateTextureFromSurface(rendu, surface_tour);
        if (!texture_tour) {
            SDL_Log("Erreur création texture texte: %s", SDL_GetError());
            SDL_FreeSurface(surface_tour);
            continue;
        }
        
        SDL_Rect rect_tour = {
            (LARGEUR_FENETRE - surface_tour->w) / 2,
            marge_haut + mini_size + 20,
            surface_tour->w,
            surface_tour->h
        };

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return PAGE_QUITTER;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Vérifier si clic sur bouton retour
                if (mouseX >= btn_retour_rect.x && 
                    mouseX <= btn_retour_rect.x + btn_retour_rect.w &&
                    mouseY >= btn_retour_rect.y && 
                    mouseY <= btn_retour_rect.y + btn_retour_rect.h) {
                    running = false;
                    return PAGE_SELEC_MODE;
                }
                else {
                    // Détection clic sur personnage
                    for (int i = 0; i < 8; i++) {
                        if (!perso_disponible[i]) continue;
                        
                        int colonne = i % 4;
                        int ligne = i / 4;
                        
                        SDL_Rect pos_perso = {
                            espacement_x + colonne * (largeur_perso + espacement_x),
                            start_y_persos - ligne * (hauteur_perso + espacement_y),
                            largeur_perso,
                            hauteur_perso
                        };

                        if (mouseX >= pos_perso.x && mouseX <= pos_perso.x + pos_perso.w &&
                            mouseY >= pos_perso.y && mouseY <= pos_perso.y + pos_perso.h) {
                            
                            if (tour_j1 && nb_selections_j1 < 3) {
                                selections_j1[nb_selections_j1] = portraits[i];
                                nb_selections_j1++;
                            } else if (!tour_j1 && nb_selections_j2 < 3) {
                                selections_j2[nb_selections_j2] = portraits[i];
                                nb_selections_j2++;
                            }                                
                            perso_disponible[i] = false;
                            tour_j1 = !tour_j1;
                            break;
                        }
                    }
                }
            }
        }

        // --- RENDU ---
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond_texture, NULL, NULL);
        SDL_RenderCopy(rendu, logo_versus_texture, NULL, &dest_logo);

        // Personnages disponibles
        for (int i = 0; i < 8; i++) {
            if (!perso_disponible[i] || !portraits[i]) continue;
            
            int colonne = i % 4;
            int ligne = i / 4;
            
            SDL_Rect dest = {
                espacement_x + colonne * (largeur_perso + espacement_x),
                start_y_persos - ligne * (hauteur_perso + espacement_y),
                largeur_perso,
                hauteur_perso
            };
            
            SDL_RenderCopy(rendu, portraits[i], NULL, &dest);
        }

        // Sélections J1 (gauche)
        for (int i = 0; i < nb_selections_j1; i++) {
            if (!selections_j1[i]) continue;
            SDL_Rect dest = {
                marge_cote,
                marge_haut + i * (mini_size + espacement_selections),
                mini_size,
                mini_size
            };
            SDL_RenderCopy(rendu, selections_j1[i], NULL, &dest);
        }

        // Sélections J2 (droite)
        for (int i = 0; i < nb_selections_j2; i++) {
            if (!selections_j2[i]) continue;
            SDL_Rect dest = {
                LARGEUR_FENETRE - marge_cote - mini_size,
                marge_haut + i * (mini_size + espacement_selections),
                mini_size,
                mini_size
            };
            SDL_RenderCopy(rendu, selections_j2[i], NULL, &dest);
        }

        // Affichage du texte "Tour de..."
        if (texture_tour) {
            SDL_RenderCopy(rendu, texture_tour, NULL, &rect_tour);
        }

        // Affichage simple du bouton retour
        if (btn_retour_texture) {
            SDL_RenderCopy(rendu, btn_retour_texture, NULL, &btn_retour_rect);
        }

        SDL_RenderPresent(rendu);
    }

    // --- NETTOYAGE ---
    for (int i = 0; i < 8; i++) {
        if (portraits[i]) SDL_DestroyTexture(portraits[i]);
    }
    SDL_DestroyTexture(fond_texture);
    SDL_DestroyTexture(logo_versus_texture);
    if (btn_retour_texture) SDL_DestroyTexture(btn_retour_texture);
    if (surface_tour) SDL_FreeSurface(surface_tour);
    if (texture_tour) SDL_DestroyTexture(texture_tour);
    TTF_CloseFont(police);

       

    return PAGE_CONFIRMATION_PERSO;
}





Page afficher_confirmation_perso(SDL_Renderer* rendu, SDL_Texture* equipe1[3], SDL_Texture* equipe2[3]) {
    // Chargement des textures de fond et d'interface uniquement
    SDL_Texture* fond_texture = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_selection_perso.png");
    SDL_Texture* btn_avancer_texture = IMG_LoadTexture(rendu, "Ressource/image/Utilité/avance.png");
    SDL_Texture* btn_retour_texture = IMG_LoadTexture(rendu, "Ressource/image/Utilité/retour.png");
    SDL_Texture* vs_texture = IMG_LoadTexture(rendu, "Ressource/image/Utilité/versus_recap.png");

    // Vérification des textures chargées
    if (!fond_texture || !btn_avancer_texture || !btn_retour_texture || !vs_texture) {
        if (fond_texture) SDL_DestroyTexture(fond_texture);
        if (btn_avancer_texture) SDL_DestroyTexture(btn_avancer_texture);
        if (btn_retour_texture) SDL_DestroyTexture(btn_retour_texture);
        if (vs_texture) SDL_DestroyTexture(vs_texture);
        return PAGE_QUITTER;
    }

    // Positions des éléments fixes
    const SDL_Rect btn_retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80};
    const SDL_Rect btn_avancer_rect = {LARGEUR_FENETRE - 100, HAUTEUR_FENETRE - 100, 80, 80};
    const SDL_Rect vs_rect = {LARGEUR_FENETRE/2 - 40, HAUTEUR_FENETRE/2 - 40, 80, 80};

    // Paramètres d'affichage des personnages
    const int largeur_perso = 150;
    const int hauteur_perso = 150;
    const int marge_haut = 100;
    const int espacement = 30;
    const int start_x_j1 = 100; // Position fixe gauche
    const int start_x_j2 = LARGEUR_FENETRE - 100 - largeur_perso; // Position fixe droite

    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return PAGE_QUITTER;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Point mouse = {event.button.x, event.button.y};
                
                if (SDL_PointInRect(&mouse, &btn_avancer_rect)) {
                    running = false;
                    return PAGE_COMBAT;
                }
                else if (SDL_PointInRect(&mouse, &btn_retour_rect)) {
                    running = false;
                    return PAGE_SELECTION_PERSO;
                }
            }
        }

        // Rendu
        SDL_RenderClear(rendu);
        
        // Fond
        SDL_RenderCopy(rendu, fond_texture, NULL, NULL);
        
        // Équipe 1 (gauche) - seulement les persos sélectionnés
        for (int i = 0; i < 3; i++) {
            if (equipe1[i]) {
                SDL_Rect dest = {
                    start_x_j1,
                    marge_haut + i * (hauteur_perso + espacement),
                    largeur_perso, 
                    hauteur_perso
                };
                SDL_RenderCopy(rendu, equipe1[i], NULL, &dest);
            }
        }
        
        // Équipe 2 (droite) - seulement les persos sélectionnés
        for (int i = 0; i < 3; i++) {
            if (equipe2[i]) {
                SDL_Rect dest = {
                    start_x_j2,
                    marge_haut + i * (hauteur_perso + espacement),
                    largeur_perso,
                    hauteur_perso
                };
                SDL_RenderCopy(rendu, equipe2[i], NULL, &dest);
            }
        }
        
        // Éléments d'interface
        SDL_RenderCopy(rendu, vs_texture, NULL, &vs_rect);
        SDL_RenderCopy(rendu, btn_avancer_texture, NULL, &btn_avancer_rect);
        SDL_RenderCopy(rendu, btn_retour_texture, NULL, &btn_retour_rect);
        
        SDL_RenderPresent(rendu);
    }
    
    // Nettoyage (ne pas détruire equipe1/equipe2)
    SDL_DestroyTexture(fond_texture);
    SDL_DestroyTexture(btn_avancer_texture);
    SDL_DestroyTexture(btn_retour_texture);
    SDL_DestroyTexture(vs_texture);
    
    return PAGE_CONFIRMATION_PERSO;
}
