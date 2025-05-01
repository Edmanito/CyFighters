#include <string.h>
#include "data.h"
#include "logic.h"
#include "interface.h"

//test//
// Variables globales réelles
int mode_choisi = 0;
int chemin_retour = 0;
int musique_lancee = 0;
Mix_Music* musique_global = NULL;
int perso_choisi = -1;


// === CHARGEMENT ===
Page afficher_chargement(SDL_Renderer *rendu) {
    SDL_Surface *image_fond = IMG_Load("Ressource/image/Fonds/fond_chargement.png");
    SDL_Texture *fond = SDL_CreateTextureFromSurface(rendu, image_fond);
    SDL_FreeSurface(image_fond);

    TTF_Font *police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 28);
    SDL_Color blanc = {255, 255, 255, 255};

    // === CADRE DE TITRE SEUL ===

    SDL_Rect zone_barre = {60, 500, 900, 30};
    SDL_Rect zone_texte = {zone_barre.x, zone_barre.y + 40, 400, 30};

    SDL_Surface *surf_texte = TTF_RenderUTF8_Solid(police, "Chargement en cours...", blanc);
    SDL_Texture *texture_texte = SDL_CreateTextureFromSurface(rendu, surf_texte);
    SDL_FreeSurface(surf_texte);

    int nb_blocs = 50, marge = 2;
    int largeur_bloc = (zone_barre.w - (nb_blocs - 1) * marge) / nb_blocs;

    for (int i = 0; i <= nb_blocs; i++) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        SDL_SetRenderDrawColor(rendu, 255, 220, 0, 255);
        for (int j = 0; j < i; j++) {
            SDL_Rect bloc = {
                zone_barre.x + j * (largeur_bloc + marge),
                zone_barre.y,
                largeur_bloc,
                zone_barre.h
            };
            SDL_RenderFillRect(rendu, &bloc);
        }
        SDL_RenderCopy(rendu, texture_texte, NULL, &zone_texte);
        SDL_RenderPresent(rendu);

        if (i > nb_blocs * 0.95)
            SDL_Delay(300);
        else if (i > nb_blocs * 0.85)
            SDL_Delay(150);
        else if (i > nb_blocs * 0.6)
            SDL_Delay(50);
        else
            SDL_Delay(20);
    }

    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(texture_texte);
    TTF_CloseFont(police);
    return PAGE_HISTOIRE;
}




// === HISTOIRE ===
Page afficher_histoire(SDL_Renderer* rendu) {
    #define NB_PHRASES 5

    const char* phrases[NB_PHRASES] = {
        "Dans un monde divisé par les royaumes...",
        "Un phénomène étrange a bouleversé l’équilibre.",
        "Les artefacts Shōnen refont surface...",
        "Toi, jeune combattant, relèveras-tu le défi ?",
        "Bienvenue dans Project Shōnen Smash."
    };

    char affichage[NB_PHRASES][256] = {{0}};
    int lettres[NB_PHRASES] = {0};

    SDL_Texture* fond = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_histoire.png");

    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 32);
    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Texture* skip_btn = IMG_LoadTexture(rendu, "Ressource/image/Utilité/avance.png");
    SDL_Rect skip_rect = {LARGEUR_FENETRE - 120, 20, 80, 80};

    Mix_Chunk* son_phrase = Mix_LoadWAV("Ressource/musique/Wav/phrase.wav");
    if (!son_phrase) SDL_Log("ERREUR chargement son phrase : %s", Mix_GetError());

    Uint32 last_char = SDL_GetTicks();
    SDL_Event event;
    int phrase = 0;

    while (phrase < NB_PHRASES) {
        Uint32 now = SDL_GetTicks();

        if (lettres[phrase] < strlen(phrases[phrase]) && now - last_char > 40) {
            lettres[phrase]++;
            last_char = now;
            memset(affichage[phrase], 0, sizeof(affichage[phrase]));
            strncpy(affichage[phrase], phrases[phrase], lettres[phrase]);

            if (lettres[phrase] == 1 && son_phrase) {
                int volume = 30 + rand() % 35;
                Mix_VolumeChunk(son_phrase, volume);
                Mix_PlayChannel(-1, son_phrase, 0);
            }
        }

        SDL_RenderClear(rendu);
        if (fond) SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i <= phrase; i++) {
            if (lettres[i] > 0) {
                SDL_Surface* surf = TTF_RenderUTF8_Solid(police, affichage[i], blanc);
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);
                    SDL_Rect rect = {
                        (LARGEUR_FENETRE - surf->w) / 2,
                        60 + i * 80,
                        surf->w,
                        surf->h
                    };
                    SDL_RenderCopy(rendu, tex, NULL, &rect);
                    SDL_FreeSurface(surf);
                    SDL_DestroyTexture(tex);
                }
            }
        }

        if (skip_btn) SDL_RenderCopy(rendu, skip_btn, NULL, &skip_rect);
        SDL_RenderPresent(rendu);

        if (lettres[phrase] == strlen(phrases[phrase])) {
            SDL_Delay(1000);
            phrase++;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return PAGE_QUITTER;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;
                if (x >= skip_rect.x && x <= skip_rect.x + skip_rect.w &&
                    y >= skip_rect.y && y <= skip_rect.y + skip_rect.h) {
                    Mix_HaltChannel(-1);
                    return PAGE_MENU;
                }
            }
        }
    }

    if (fond) SDL_DestroyTexture(fond);
    if (skip_btn) SDL_DestroyTexture(skip_btn);
    if (son_phrase) Mix_FreeChunk(son_phrase);
    TTF_CloseFont(police);

    return PAGE_MENU;
}

// === LECTEUR DE MUSIQUE GLOBAL ===
void jouer_musique(const char* chemin, int volume) {
    // Si la musique est déjà lancée et est en train de jouer, on ne fait rien
    if (musique_lancee && Mix_PlayingMusic()) {
        return;
    }

    // Sinon, on arrête l'ancienne musique si elle existe
    if (musique_global) {
        Mix_HaltMusic();
        Mix_FreeMusic(musique_global);
        musique_global = NULL;
    }

    // Chargement de la nouvelle musique
    musique_global = Mix_LoadMUS(chemin);
    if (musique_global) {
        Mix_VolumeMusic(volume);
        Mix_PlayMusic(musique_global, -1);
        musique_lancee = 1;
    } else {
        SDL_Log("Erreur musique : %s", Mix_GetError());
    }
}



// === MENU PRINCIPAL ===
Page afficher_menu(SDL_Renderer* rendu) {
    //jouer_musique("Ressource/musique/Wav/menu.wav", 20);

    SDL_Texture* fond = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_menu.png");
    SDL_Texture* cadre_titre = IMG_LoadTexture(rendu, "Ressource/image/Cadres/cadre_titre.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "Ressource/image/Cadres/cadre_texte.png");

    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 40);
    SDL_Color noir = {0, 0, 0, 255};

    SDL_Rect zone_titre = {262, 0, 500, 250};
    SDL_Rect boutons[3] = {
        {370, 160, 280, 190},
        {370, 280, 280, 190},
        {370, 400, 280, 190}
    };
    const char* textes[] = {"Jouer", "Options", "Quitter"};

    SDL_RenderClear(rendu);
    SDL_RenderCopy(rendu, fond, NULL, NULL);
    SDL_RenderCopy(rendu, cadre_titre, NULL, &zone_titre);

    for (int i = 0; i < 3; i++) {
        SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);
        SDL_Surface* surf = TTF_RenderUTF8_Solid(police, textes[i], noir);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);
        SDL_Rect txt = {
            boutons[i].x + (boutons[i].w - surf->w) / 2,
            boutons[i].y + (boutons[i].h - surf->h) / 2,
            surf->w,
            surf->h
        };
        SDL_RenderCopy(rendu, tex, NULL, &txt);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }

    SDL_RenderPresent(rendu);

    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h)
                    return PAGE_SELEC_MODE;

                if (x >= boutons[1].x && x <= boutons[1].x + boutons[1].w &&
                    y >= boutons[1].y && y <= boutons[1].y + boutons[1].h)
                    return PAGE_OPTIONS;

                if (x >= boutons[2].x && x <= boutons[2].x + boutons[2].w &&
                    y >= boutons[2].y && y <= boutons[2].y + boutons[2].h)
                    return PAGE_QUITTER;
            }
        }
    }

    return PAGE_MENU;
}

// === OPTIONS ===
Page afficher_options(SDL_Renderer* rendu, Page page_prec) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "Ressource/image/Cadres/cadre_texte.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "Ressource/image/Utilité/retour.png");

    const char* textes[] = {"Sauvegarde", "Langue", "Son"};
    SDL_Color noir = {0, 0, 0, 255};
    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 40);

    SDL_Rect boutons[3] = {
        {370, 160, 280, 190},
        {370, 280, 280, 190},
        {370, 400, 280, 190}
    };
    SDL_Rect retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Event event;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < 3; i++) {
            SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);
            SDL_Surface* surf = TTF_RenderUTF8_Solid(police, textes[i], noir);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);
            SDL_Rect txt = {
                boutons[i].x + (boutons[i].w - surf->w) / 2,
                boutons[i].y + (boutons[i].h - surf->h) / 2,
                surf->w,
                surf->h
            };
            SDL_RenderCopy(rendu, tex, NULL, &txt);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour_rect);
        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;
                if (x >= retour_rect.x && x <= retour_rect.x + retour_rect.w &&
                    y >= retour_rect.y && y <= retour_rect.y + retour_rect.h)
                    return page_prec;
            }
        }
    }
}

// === PAGE DE JEU ===
Page afficher_jeu(SDL_Renderer* rendu, SDL_Texture* selections_j1[3], SDL_Texture* selections_j2[3]) {
    // Fond noir
    SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
    SDL_RenderClear(rendu);

    // Afficher les personnages sélectionnés
    const int taille_perso = 100;
    const int marge = 50;
    
    // Afficher l'équipe du joueur 1 (gauche)
    for (int i = 0; i < 3; i++) {
        if (selections_j1[i]) {
            SDL_Rect dest = {
                marge,
                marge + i * (taille_perso + 20),
                taille_perso,
                taille_perso
            };
            SDL_RenderCopy(rendu, selections_j1[i], NULL, &dest);
        }
    }

    // Afficher l'équipe du joueur 2 (droite)
    for (int i = 0; i < 3; i++) {
        if (selections_j2[i]) {
            SDL_Rect dest = {
                LARGEUR_FENETRE - marge - taille_perso,
                marge + i * (taille_perso + 20),
                taille_perso,
                taille_perso
            };
            SDL_RenderCopy(rendu, selections_j2[i], NULL, &dest);
        }
    }

    // Texte "JEU EN COURS..."
    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 40);
    if (police) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderUTF8_Solid(police, "JEU EN COURS...", blanc);
        if (surf) {
            SDL_Texture* texte = SDL_CreateTextureFromSurface(rendu, surf);
            if (texte) {
                SDL_Rect rect = {
                    (LARGEUR_FENETRE - surf->w) / 2,
                    HAUTEUR_FENETRE - 100,
                    surf->w,
                    surf->h
                };
                SDL_RenderCopy(rendu, texte, NULL, &rect);
                SDL_DestroyTexture(texte);
            }
            SDL_FreeSurface(surf);
        }
        TTF_CloseFont(police);
    }

    SDL_RenderPresent(rendu);

    // Gestion des événements
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                return PAGE_MENU;
            }
        }
        SDL_Delay(16); // Limite à ~60 FPS
    }
}

// === PAGE DE SÉLECTION MODE ===
Page afficher_selec_mode(SDL_Renderer* rendu) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "Ressource/image/Cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "Ressource/image/Utilité/retour.png");

    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 80);
    TTF_SetFontStyle(police, TTF_STYLE_BOLD);

    SDL_Color noir = {0, 0, 0, 255};

    int marge = 30;
    int largeur_bouton = (LARGEUR_FENETRE - 3 * marge) / 2;
    int hauteur_bouton = HAUTEUR_FENETRE - 2 * marge;

    const char* textes[2][3] = {
        {"J1", "VS", "J2"},
        {"J1", "VS", "IA"}
    };

    SDL_Rect boutons[2] = {
        {marge, marge, largeur_bouton, hauteur_bouton},
        {2 * marge + largeur_bouton, marge, largeur_bouton, hauteur_bouton}
    };

    SDL_Rect retour = {20, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Event event;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < 2; i++) {
            SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);

            for (int j = 0; j < 3; j++) {
                SDL_Surface* surf = TTF_RenderUTF8_Solid(police, textes[i][j], noir);
                SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);

                SDL_Rect txt = {
                    boutons[i].x + (boutons[i].w - surf->w) / 2 - 30 + j * 30,
                    boutons[i].y + (boutons[i].h - 3 * surf->h) / 2 + j * surf->h,
                    surf->w,
                    surf->h
                };
                SDL_RenderCopy(rendu, tex, NULL, &txt);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour);
        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                // 👉 Clique sur le bouton Retour en premier
                if (x >= retour.x && x <= retour.x + retour.w &&
                    y >= retour.y && y <= retour.y + retour.h) {
                    return PAGE_MENU; // 🔥 Retourner au menu principal
                }

                // 👉 Clique sur J1 vs J2
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    chemin_retour = 0;
                    return PAGE_SELECTION_PERSO;
                }

                // 👉 Clique sur J1 vs IA
                if (x >= boutons[1].x && x <= boutons[1].x + boutons[1].w &&
                    y >= boutons[1].y && y <= boutons[1].y + boutons[1].h) {
                    chemin_retour = 1;
                    return PAGE_SELEC_DIFFICULTE;
                }
            }
        }
    }
}



// === PAGE DE SÉLECTION DIFFICULTÉ ===
Page afficher_selec_difficulte(SDL_Renderer* rendu) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "Ressource/image/Fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "Ressource/image/Cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "Ressource/image/Utilité/retour.png");

    TTF_Font* police = TTF_OpenFont("Ressource/langue/Police/arial.ttf", 40);
    SDL_Color noir = {0, 0, 0, 255};

    int marge = 30;
    int largeur_bouton = LARGEUR_FENETRE - 2 * marge;
    int hauteur_bouton = (HAUTEUR_FENETRE - 4 * marge) / 3;

    const char* textes[] = {"Facile", "Moyen", "Difficile"};

    SDL_Rect boutons[3] = {
        {marge, marge, largeur_bouton, hauteur_bouton},
        {marge, 2 * marge + hauteur_bouton, largeur_bouton, hauteur_bouton},
        {marge, 3 * marge + 2 * hauteur_bouton, largeur_bouton, hauteur_bouton}
    };

    SDL_Rect retour = {20, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Event event;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < 3; i++) {
            SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);
            SDL_Surface* surf = TTF_RenderUTF8_Solid(police, textes[i], noir);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);
            SDL_Rect txt = {
                boutons[i].x + (boutons[i].w - surf->w) / 2,
                boutons[i].y + (boutons[i].h - surf->h) / 2,
                surf->w,
                surf->h
            };
            SDL_RenderCopy(rendu, tex, NULL, &txt);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour);
        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return PAGE_QUITTER;
            }
        
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;

                // Clic sur retour
                if (x >= retour.x && x <= retour.x + retour.w &&
                    y >= retour.y && y <= retour.y + retour.h) {
                    return PAGE_SELEC_MODE;
                }
                
                // Clic sur "Facile"
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    return PAGE_SELECTION_PERSO;
                }
        
                // Clic sur "Moyen"
                if (x >= boutons[1].x && x <= boutons[1].x + boutons[1].w &&
                    y >= boutons[1].y && y <= boutons[1].y + boutons[1].h) {
                    return PAGE_SELECTION_PERSO;
                }
        
                // Clic sur "Difficile"
                if (x >= boutons[2].x && x <= boutons[2].x + boutons[2].w &&
                    y >= boutons[2].y && y <= boutons[2].y + boutons[2].h) {
                    return PAGE_SELECTION_PERSO;
                }
        
            }
        }
    }
}