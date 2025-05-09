#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "data.h"
#include "interface.h"
#include "son.h"
#include "langue.h"


int volume_global = 20; // entre 0 et 128
int musique_actuelle = 1; // 1, 2 ou 3


//test//
// Variables globales réelles
int mode_choisi = 0;
int chemin_retour = 0;
int perso_choisi = -1;
bool phraseJouee = false;
int musiqueRes=1;









// === CHARGEMENT ===
Page afficher_chargement(SDL_Renderer *rendu) {
    SDL_Surface *image_fond = IMG_Load("ressource/image/fonds/fond_chargement.png");
    SDL_Texture *fond = SDL_CreateTextureFromSurface(rendu, image_fond);
    SDL_FreeSurface(image_fond);

    TTF_Font *police = TTF_OpenFont("ressource/langue/police/arial.ttf", 28);
    SDL_Color blanc = {255, 255, 255, 255};

    // === Zone de la barre de chargement (étendue) ===
    int marge_horizontale = 30;
    SDL_Rect zone_barre = {
        marge_horizontale,
        500,
        LARGEUR_FENETRE - 2 * marge_horizontale,
        30
    };

    SDL_Rect zone_texte = {
        zone_barre.x,
        zone_barre.y + 40,
        400,
        30
    };

    SDL_Surface* surf_texte = TTF_RenderUTF8_Solid(police, getTexte("chargement en cours"), blanc);
    SDL_Texture *texture_texte = SDL_CreateTextureFromSurface(rendu, surf_texte);
    SDL_FreeSurface(surf_texte);

    int nb_blocs = 50, marge = 2;
    int largeur_bloc = (zone_barre.w - (nb_blocs - 1) * marge) / nb_blocs;

    for (int i = 0; i <= nb_blocs; i++) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        // --- Dégradé rouge foncé → orange → vert ---
        int r, g;
        if (i <= nb_blocs / 2) {
            float t = (float)i / (nb_blocs / 2);
            r = 128 + (int)(127 * t);  // 128 → 255
            g = (int)(128 * t);        // 0 → 128
        } else {
            float t = (float)(i - nb_blocs / 2) / (nb_blocs / 2);
            r = 255 - (int)(255 * t);  // 255 → 0
            g = 128 + (int)(127 * t);  // 128 → 255
        }
        if (r < 0) r = 0;
        if (g > 255) g = 255;
        SDL_SetRenderDrawColor(rendu, r, g, 0, 255);

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

    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_histoire.png");

    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Texture* skip_btn = IMG_LoadTexture(rendu, "ressource/image/utilité/avance.png");
    SDL_Rect skip_rect = {LARGEUR_FENETRE - 120, 20, 80, 80};

    Mix_Chunk* son_phrase = Mix_LoadWAV("ressource/musique/ogg/phrase.ogg");
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

            if (lettres[phrase] == 1 && son_phrase && !phraseJouee) {
                int volume = 30 + rand() % 35;
                Mix_VolumeChunk(son_phrase, volume);
                Mix_PlayChannel(-1, son_phrase, 0);
                phraseJouee = true;
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
            phraseJouee = false;
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


















// === MENU PRINCIPAL ===
Page afficher_menu(SDL_Renderer* rendu) {
    if (musiqueRes == 1) {
        jouerMusique("ressource/musique/ogg/menu/menu_1.ogg", 20);
        musiqueRes = 0;
    }

    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_titre = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_titre.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");
    SDL_Texture* image_trophee = IMG_LoadTexture(rendu, "ressource/image/utilité/trophee.png");
    SDL_Texture* image_manette = IMG_LoadTexture(rendu, "ressource/image/utilité/manette.png");

    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);
    SDL_Color noir = {0, 0, 0, 255};

    int tailleTitre = 250;
    int tailleBouton = 190;
    int largeurBouton = 280;
    int largeurTitre = 500;

    SDL_Rect zone_titre = {LARGEUR_FENETRE / 2 - largeurTitre / 2, 10, largeurTitre, tailleTitre};
    SDL_Rect boutons[3] = {
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 200, largeurBouton, tailleBouton},
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 360, largeurBouton, tailleBouton},
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 520, largeurBouton, tailleBouton}
    };
    const char* ids_textes[] = {"jouer", "options", "quitter"};

    // Position trophée (en bas à droite)
    SDL_Rect rect_trophee = {
        LARGEUR_FENETRE - 120,   // x
        HAUTEUR_FENETRE - 140,   // y
        100, 100                 // w, h
    };

    // Position manette (au-dessus du trophée)
    SDL_Rect rect_manette = {
        LARGEUR_FENETRE - 120,
        HAUTEUR_FENETRE - 260,
        100, 100
    };

    SDL_RenderClear(rendu);
    SDL_RenderCopy(rendu, fond, NULL, NULL);
    SDL_RenderCopy(rendu, cadre_titre, NULL, &zone_titre);

    for (int i = 0; i < 3; i++) {
        SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);
        SDL_Surface* surf = TTF_RenderUTF8_Solid(police, getTexte(ids_textes[i]), noir);
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

    // Affichage des icônes
    SDL_RenderCopy(rendu, image_trophee, NULL, &rect_trophee);
    SDL_RenderCopy(rendu, image_manette, NULL, &rect_manette);

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




















Page afficher_credit(SDL_Renderer* rendu, Page page_prec);


// === OPTIONS ===
Page afficher_options(SDL_Renderer* rendu, Page page_prec) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    SDL_Texture* drapeauAllemand = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauAllemand.png");
    SDL_Texture* drapeauAnglais  = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauAnglais.png");
    SDL_Texture* drapeauEspagnol = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauEspagnol.png");
    SDL_Texture* drapeauFrancais = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauFrancais.png");

    SDL_Texture* images_musiques[6];
    for (int i = 0; i < 6; i++) {
        char chemin[128];
        sprintf(chemin, "ressource/image/utilité/musique/menu_%d.png", i + 1);
        images_musiques[i] = IMG_LoadTexture(rendu, chemin);
    }

    const char* ids_textes[] = {"credit", "langue", "volume", "musique"};

    SDL_Color blanc = {255, 255, 255, 255};
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);

    int tailleBouton = 250;
    int largeurBouton = 200;

    SDL_Rect boutons[4] = {
        {0, -50, largeurBouton, tailleBouton},
        {0, 100, largeurBouton, tailleBouton},
        {0, 250, largeurBouton, tailleBouton},
        {0, 400, largeurBouton, tailleBouton}
    };

    SDL_Rect barre = {
        (LARGEUR_FENETRE - 300) / 2,
        boutons[2].y + 100,
        300, 30
    };

    SDL_Rect retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Rect musiques[6];
    for (int i = 0; i < 6; i++) {
        int ligne = i / 3;
        int colonne = i % 3;

        int largeur = 200;
        int hauteur = 150;
        int espacement_base = 280;
        int decalage_col_1 = 90;
        int decalage_col_2 = 150;

        musiques[i].w = largeur;
        musiques[i].h = hauteur;

        int base_x = LARGEUR_FENETRE / 2 - (espacement_base * 3) / 2;

        musiques[i].x = base_x + colonne * espacement_base;
        if (colonne == 1) musiques[i].x += decalage_col_1;
        if (colonne == 2) musiques[i].x += decalage_col_2;

        musiques[i].y = barre.y + 100 + ligne * (hauteur + 30);

        if (ligne == 0) musiques[i].y -= 50;
        if (ligne == 1) musiques[i].y -= 80;
    }

    int drapeau_taille = 110;
    int espacement = 60;
    SDL_Rect drapeaux[4];
    int base_x = 350;
    int base_y = boutons[1].y + (boutons[1].h - drapeau_taille) / 2;
    for (int i = 0; i < 4; i++) {
        drapeaux[i].x = base_x + i * (drapeau_taille + espacement);
        drapeaux[i].y = base_y;
        drapeaux[i].w = drapeau_taille;
        drapeaux[i].h = drapeau_taille;
    }

    bool curseur_actif = false;
    SDL_Event event;

    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < 4; i++) {
            SDL_RenderCopy(rendu, cadre_bouton, NULL, &boutons[i]);
            SDL_Surface* surf = TTF_RenderUTF8_Solid(police, getTexte(ids_textes[i]), blanc);
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

        SDL_RenderCopy(rendu, drapeauFrancais, NULL, &drapeaux[0]);
        SDL_RenderCopy(rendu, drapeauAnglais,  NULL, &drapeaux[1]);
        SDL_RenderCopy(rendu, drapeauEspagnol, NULL, &drapeaux[2]);
        SDL_RenderCopy(rendu, drapeauAllemand, NULL, &drapeaux[3]);

        SDL_Rect fond_barre = {barre.x - 4, barre.y - 4, barre.w + 8, barre.h + 8};
        SDL_SetRenderDrawColor(rendu, 20, 20, 20, 255); SDL_RenderFillRect(rendu, &fond_barre);
        SDL_SetRenderDrawColor(rendu, 40, 40, 40, 255); SDL_RenderFillRect(rendu, &barre);
        float ratio = volume_global / 128.0f;
        int r = (int)(255 * (1 - ratio));
        int g = (int)(255 * ratio);
        SDL_SetRenderDrawColor(rendu, r, g, 60, 255);
        SDL_Rect niveau = {barre.x, barre.y, volume_global * barre.w / 128, barre.h};
        SDL_RenderFillRect(rendu, &niveau);
        int curseur_x = barre.x + niveau.w;
        int curseur_y = barre.y + barre.h / 2;
        for (int i = 10; i >= 1; i--) {
            SDL_SetRenderDrawColor(rendu, 255, 255, 255, 20 * i);
            SDL_Rect halo = {curseur_x - i, curseur_y - i, 2 * i, 2 * i};
            SDL_RenderFillRect(rendu, &halo);
        }
        SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);
        SDL_Rect curseur = {curseur_x - 4, curseur_y - 4, 8, 8};
        SDL_RenderFillRect(rendu, &curseur);

        char texte_volume[16];
        sprintf(texte_volume, "%d %%", volume_global * 100 / 128);
        SDL_Surface* surf_volume = TTF_RenderUTF8_Solid(police, texte_volume, blanc);
        SDL_Texture* tex_volume = SDL_CreateTextureFromSurface(rendu, surf_volume);
        SDL_Rect txt = {
            barre.x + barre.w + 20,
            barre.y + (barre.h - surf_volume->h) / 2,
            surf_volume->w,
            surf_volume->h
        };
        SDL_Rect cadre_txt = {txt.x - 10, txt.y - 5, txt.w + 20, txt.h + 10};
        SDL_SetRenderDrawColor(rendu, 0, 0, 0, 200);
        SDL_RenderFillRect(rendu, &cadre_txt);
        SDL_RenderCopy(rendu, tex_volume, NULL, &txt);
        SDL_FreeSurface(surf_volume);
        SDL_DestroyTexture(tex_volume);

        for (int i = 0; i < 6; i++) {
            SDL_RenderCopy(rendu, images_musiques[i], NULL, &musiques[i]);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour_rect);
        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                return PAGE_QUITTER;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                // Bouton crédit (en haut)
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    Page retour = afficher_credit(rendu, PAGE_OPTIONS);
                    if (retour == PAGE_QUITTER) {
                        for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                        return PAGE_QUITTER;
                    }
                }

                if (x >= retour_rect.x && x <= retour_rect.x + retour_rect.w &&
                    y >= retour_rect.y && y <= retour_rect.y + retour_rect.h) {
                    for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                    return page_prec;
                }

                if (x >= barre.x && x <= barre.x + barre.w &&
                    y >= barre.y && y <= barre.y + barre.h) {
                    volume_global = (x - barre.x) * 128 / barre.w;
                    if (volume_global < 0) volume_global = 0;
                    if (volume_global > 128) volume_global = 128;
                    Mix_VolumeMusic(volume_global);
                    curseur_actif = true;
                }

                for (int i = 0; i < 4; i++) {
                    if (x >= drapeaux[i].x && x <= drapeaux[i].x + drapeaux[i].w &&
                        y >= drapeaux[i].y && y <= drapeaux[i].y + drapeaux[i].h) {
                        changerLangue((Langue)i);
                    }
                }

                for (int i = 0; i < 6; i++) {
                    if (x >= musiques[i].x && x <= musiques[i].x + musiques[i].w &&
                        y >= musiques[i].y && y <= musiques[i].y + musiques[i].h) {
                        musique_actuelle = i + 1;
                        Mix_HaltMusic();
                        char chemin[128];
                        sprintf(chemin, "ressource/musique/ogg/menu/menu_%d.ogg", musique_actuelle);
                        jouerMusique(chemin, volume_global);
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                curseur_actif = false;
            }

            if (event.type == SDL_MOUSEMOTION && curseur_actif) {
                int x = event.motion.x;
                volume_global = (x - barre.x) * 128 / barre.w;
                if (volume_global < 0) volume_global = 0;
                if (volume_global > 128) volume_global = 128;
                Mix_VolumeMusic(volume_global);
            }
        }
    }
}






Page afficher_credit(SDL_Renderer* rendu, Page page_prec){
    SDL_Event event;

    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/credit.png");

    if (!fond) {
        printf("Erreur chargement fond credit : %s\n", IMG_GetError());
    }

    SDL_Rect retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80};

    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 28);
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyTexture(bouton_retour);
        SDL_DestroyTexture(fond);
        return page_prec;
    }

    const char* lignes[] = {
        "Project Shonen Smash – Credits",
        "",
        "Développement principal :",
        "Mansour Wajih",
        "  Intégration SDL2 (affichages, sons, transitions)",
        "  Programmation du gameplay, des menus, du système de combat",
        "  Organisation générale du projet",
        "",
        "Yanis",
        "  Développement du gameplay (attaques, tours, effets)",
        "  Élaboration des interfaces et logique de jeu",
        "",
        "Malak",
        "  Sélection et structuration des personnages jouables",
        "  Création des attaques spéciales et des effets de statut",
        "  Équilibrage du système de combat",
        "",
        "Collaborations :",
        "Loukas",
        "  Contributions techniques sur certains éléments de l’interface SDL2",
        "  Suggestions sur l’organisation des fichiers et du code",
        "",
        "Nemu",
        "  Création des sprites pixelisés des personnages",
        "  Contribution au style visuel du jeu",
        "",
        "Technologies utilisées :",
        "  Langage C (standard ANSI)",
        "  SDL2 – SDL_image – SDL_mixer – SDL_ttf",
        "  Architecture modulaire : menus, combats, options, transitions...",
        "",
        "Contexte :",
        "  Projet universitaire réalisé à CY Tech",
        "  Création d’un jeu complet en SDL2 avec gameplay 3v3",
        "  Année académique 2025",
        "",
        "Remerciements :",
        "  Nos enseignants pour leur accompagnement",
        "  Les joueurs ayant testé le jeu et partagé leurs retours",
        "  ChatGPT, notre débugger de l’ombre",
        "    Qui corrige les erreurs SDL avant même qu’on crie à l’aide",
        "    Et qui supporte nos boucles infinies à 3h du matin",
        "  Bon... certaines musiques ne sont pas libres de droits,",
        "  mais c’était pour la bonne cause : créer une vraie ambiance !",
        "",
        "Merci d’avoir joué à Project Shonen Smash !",
        "Édition 2025 – Forgé entre lignes de code et éclats de pixels",
        "Le rideau tombe...",
        "Mais l'aventure, elle, ne fait que commencer !"
    };
    

    int nb_lignes = sizeof(lignes) / sizeof(lignes[0]);
    int y_offset = HAUTEUR_FENETRE;

    Mix_HaltMusic();
    jouerMusique("ressource/musique/ogg/credit.ogg", volume_global);

    while (1) {
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < nb_lignes; i++) {
            if (!lignes[i] || strlen(lignes[i]) == 0) continue;

            SDL_Color couleur;

            if (strcmp(lignes[i], "Project Shonen Smash – Credits") == 0) {
                couleur = (SDL_Color){100, 180, 255, 255}; // Bleu
            } else if (
                strcmp(lignes[i], "Mansour Wajih") == 0 ||
                strcmp(lignes[i], "Yanis") == 0 ||
                strcmp(lignes[i], "Malak") == 0 ||
                strcmp(lignes[i], "Loukas") == 0 ||
                strcmp(lignes[i], "Nemu") == 0
            ) {
                couleur = (SDL_Color){255, 70, 70, 255}; // Rouge
            } else if (
                strcmp(lignes[i], "Developpement principal :") == 0 ||
                strcmp(lignes[i], "Collaborations :") == 0 ||
                strcmp(lignes[i], "Technologies utilisees :") == 0 ||
                strcmp(lignes[i], "Contexte :") == 0 ||
                strcmp(lignes[i], "Remerciements :") == 0
            ) {
                couleur = (SDL_Color){255, 215, 0, 255}; // Jaune
            } else {
                couleur = (SDL_Color){255, 255, 255, 255}; // Blanc
            }

            SDL_Surface* surf = TTF_RenderUTF8_Solid(font, lignes[i], couleur);
            if (!surf) continue;

            SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, surf);
            if (!tex) {
                SDL_FreeSurface(surf);
                continue;
            }

            SDL_Rect rect = {
                (LARGEUR_FENETRE - surf->w) / 2,
                y_offset + i * 40,
                surf->w,
                surf->h
            };

            SDL_RenderCopy(rendu, tex, NULL, &rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour_rect);
        SDL_RenderPresent(rendu);

        SDL_Delay(30);
        y_offset -= 5; // Défilement rapide

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyTexture(bouton_retour);
                SDL_DestroyTexture(fond);
                TTF_CloseFont(font);
                return PAGE_QUITTER;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;
                if (x >= retour_rect.x && x <= retour_rect.x + retour_rect.w &&
                    y >= retour_rect.y && y <= retour_rect.y + retour_rect.h) {

                    SDL_DestroyTexture(bouton_retour);
                    SDL_DestroyTexture(fond);
                    TTF_CloseFont(font);

                    char chemin[128];
                    sprintf(chemin, "ressource/musique/ogg/menu/menu_%d.ogg", musique_actuelle);
                    jouerMusique(chemin, volume_global);
                    return page_prec;
                }
            }
        }

        if (y_offset + nb_lignes * 40 < 0) {
            SDL_DestroyTexture(bouton_retour);
            SDL_DestroyTexture(fond);
            TTF_CloseFont(font);

            char chemin[128];
            sprintf(chemin, "ressource/musique/ogg/menu/menu_%d.ogg", musique_actuelle);
            jouerMusique(chemin, volume_global);
            return page_prec;
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
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);
    if (police) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderUTF8_Solid(police, getTexte("jeu_en_cours"), blanc);
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
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    TTF_Font* police_titre = TTF_OpenFont("ressource/langue/police/arial.ttf", 50);
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 65); // un peu plus petit qu’avant
    TTF_SetFontStyle(police, TTF_STYLE_BOLD);
    SDL_Color noir = {0, 0, 0, 255};

    int marge = 30;
    int largeur_bouton = (LARGEUR_FENETRE - 3 * marge) / 2;
    int hauteur_bouton = HAUTEUR_FENETRE - 2 * marge - 100;

    const char* textes[2][3] = {
        {"J1", "VS", "J2"},
        {"J1", "VS", "IA"}
    };

    SDL_Rect boutons[2] = {
        {marge, marge + 100, largeur_bouton, hauteur_bouton},
        {2 * marge + largeur_bouton, marge + 100, largeur_bouton, hauteur_bouton}
    };

    SDL_Rect retour = {20, HAUTEUR_FENETRE - 100, 80, 80};

    // Titre "Sélection du mode"
    SDL_Surface* surf_titre = TTF_RenderUTF8_Solid(police_titre, getTexte("selection mode"), noir);
    SDL_Texture* tex_titre = SDL_CreateTextureFromSurface(rendu, surf_titre);
    SDL_Rect rect_titre = {
        (LARGEUR_FENETRE - surf_titre->w) / 2,
        30,
        surf_titre->w,
        surf_titre->h
    };
    SDL_FreeSurface(surf_titre);

    SDL_Event event;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);
        SDL_RenderCopy(rendu, tex_titre, NULL, &rect_titre);

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

                if (x >= retour.x && x <= retour.x + retour.w &&
                    y >= retour.y && y <= retour.y + retour.h)
                    return PAGE_MENU;

                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    chemin_retour = 0;
                    return PAGE_SELECTION_PERSO;
                }

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
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);
    SDL_Color noir = {0, 0, 0, 255};

    int marge = 30;
    int largeur_bouton = LARGEUR_FENETRE - 2 * marge;
    int hauteur_bouton = (HAUTEUR_FENETRE - 4 * marge) / 3;

    const char* ids_difficulte[] = {"facile", "moyen", "difficile"};

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
            SDL_Surface* surf = TTF_RenderUTF8_Solid(police, getTexte(ids_difficulte[i]), noir);
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
                  
                        partieActuelle.iaDifficulte=1;
                        return PAGE_SELECTION_PERSO;
                }
        
                // Clic sur "Moyen"
                if (x >= boutons[1].x && x <= boutons[1].x + boutons[1].w &&
                    y >= boutons[1].y && y <= boutons[1].y + boutons[1].h) {
                  
                        partieActuelle.iaDifficulte=2;
                        return PAGE_SELECTION_PERSO;
                }
        
                // Clic sur "Difficile"
                if (x >= boutons[2].x && x <= boutons[2].x + boutons[2].w &&
                    y >= boutons[2].y && y <= boutons[2].y + boutons[2].h) {
                    
                        partieActuelle.iaDifficulte=3;
                        return PAGE_SELECTION_PERSO;
                }
        
            }
        }
    }
}