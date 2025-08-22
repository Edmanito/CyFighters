#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "data.h"
#include "interface.h"
#include "son.h"
#include "langue.h"


int volume_global = 20; // entre 0 et 128
int musique_actuelle = 1; // 1, 2 ou 3
//
// Variables globales réelles
int mode_choisi = 0;
int chemin_retour = 0;
int perso_choisi = -1;
bool phraseJouee = false;
int musiqueRes=1;



// Affiche une page de chargement avec une animation de barre de progression
Page afficher_chargement(SDL_Renderer *rendu) {
    // Chargement du fond
    SDL_Surface *image_fond = IMG_Load("ressource/image/fonds/fond_chargement.png");
    SDL_Texture *fond = SDL_CreateTextureFromSurface(rendu, image_fond);
    SDL_FreeSurface(image_fond);

    // Chargement de la police pour afficher le texte
    TTF_Font *police = TTF_OpenFont("ressource/langue/police/arial.ttf", 28);
    SDL_Color blanc = {255, 255, 255, 255};

    // Définition de la zone de la barre de chargement
    int marge_horizontale = 30;
    SDL_Rect zone_barre = {
        marge_horizontale,
        500,
        LARGEUR_FENETRE - 2 * marge_horizontale,
        30
    };

    // Zone du texte affiché sous la barre
    SDL_Rect zone_texte = {
        zone_barre.x,
        zone_barre.y + 40,
        400,
        30
    };

    // Création du texte "Chargement en cours..."
    SDL_Surface* surf_texte = TTF_RenderUTF8_Solid(police, getTexte("chargement en cours"), blanc);
    SDL_Texture *texture_texte = SDL_CreateTextureFromSurface(rendu, surf_texte);
    SDL_FreeSurface(surf_texte);

    // Paramètres de la barre
    int nb_blocs = 50;  // nombre de blocs dans la barre
    int marge = 2;      // espace entre les blocs
    int largeur_bloc = (zone_barre.w - (nb_blocs - 1) * marge) / nb_blocs;

    // Affichage progressif de la barre de chargement
    for (int i = 0; i <= nb_blocs; i++) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        // Couleur dégradée rouge → vert selon la progression
        int r, g;
        if (i <= nb_blocs / 2) {
            float interpolation = (float)i / (nb_blocs / 2);
            r = 128 + (int)(127 * interpolation);  // passe de 128 à 255
            g = (int)(128 * interpolation);        // passe de 0 à 128
        } else {
            float interpolation = (float)(i - nb_blocs / 2) / (nb_blocs / 2);
            r = 255 - (int)(255 * interpolation);  // passe de 255 à 0
            g = 128 + (int)(127 * interpolation);  // passe de 128 à 255
        }

        // Limites de sécurité sur les couleurs
        if (r < 0) r = 0;
        if (g > 255) g = 255;

        SDL_SetRenderDrawColor(rendu, r, g, 0, 255);

        // Affichage des blocs
        for (int j = 0; j < i; j++) {
            SDL_Rect bloc = {
                zone_barre.x + j * (largeur_bloc + marge),
                zone_barre.y,
                largeur_bloc,
                zone_barre.h
            };
            SDL_RenderFillRect(rendu, &bloc);
        }

        // Affiche le texte sous la barre
        SDL_RenderCopy(rendu, texture_texte, NULL, &zone_texte);
        SDL_RenderPresent(rendu);

        // Vitesse d’animation : ralentit vers la fin
        if (i > nb_blocs * 0.95)
            SDL_Delay(300);
        else if (i > nb_blocs * 0.85)
            SDL_Delay(150);
        else if (i > nb_blocs * 0.6)
            SDL_Delay(50);
        else
            SDL_Delay(20);
    }

    // Nettoyage
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(texture_texte);
    TTF_CloseFont(police);

    // Passe à la page histoire après le chargement
    return PAGE_HISTOIRE;
}



// Affiche une cinématique d'intro avec 5 phrases qui apparaissent progressivement à l'écran
Page afficher_histoire(SDL_Renderer* rendu) {
    #define NB_PHRASES 5

    // Phrases à afficher
    const char* phrases[NB_PHRASES] = {
        "Dans un monde divisé par les royaumes...",
        "Un phénomène étrange a bouleversé l’équilibre.",
        "Les artefacts Shōnen refont surface...",
        "Toi, jeune combattant, relèveras-tu le défi ?",
        "Bienvenue dans Project Shōnen Smash."
    };

    char affichage[NB_PHRASES][256] = {{0}};  // Phrases partiellement affichées
    int lettres[NB_PHRASES] = {0};           // Nb de lettres déjà affichées

    // Fond d'écran
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_histoire.png");

    // Police + couleur
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    SDL_Color blanc = {255, 255, 255, 255};

    // Bouton "passer"
    SDL_Texture* skip_btn = IMG_LoadTexture(rendu, "ressource/image/utilité/avance.png");
    SDL_Rect skip_rect = {LARGEUR_FENETRE - 120, 20, 80, 80};

    // Son joué quand une phrase commence à apparaître
    Mix_Chunk* son_phrase = Mix_LoadWAV("ressource/musique/ogg/phrase.ogg");
    if (!son_phrase) SDL_Log("ERREUR chargement son phrase : %s", Mix_GetError());

    Uint32 last_char = SDL_GetTicks();  // Temps d'affichage de la dernière lettre
    SDL_Event event;
    int phrase = 0;

    // Affichage des 5 phrases, une à une
    while (phrase < NB_PHRASES) {
        Uint32 now = SDL_GetTicks();

        // Ajout d’une lettre toutes les 40 ms
        if (lettres[phrase] < strlen(phrases[phrase]) && now - last_char > 40) {
            lettres[phrase]++;
            last_char = now;
            memset(affichage[phrase], 0, sizeof(affichage[phrase]));
            strncpy(affichage[phrase], phrases[phrase], lettres[phrase]);

            // Lance un son au début de chaque nouvelle phrase
            if (lettres[phrase] == 1 && son_phrase && !phraseJouee) {
                int volume = 30 + rand() % 35;
                Mix_VolumeChunk(son_phrase, volume);
                Mix_PlayChannel(-1, son_phrase, 0);
                phraseJouee = true;
            }
        }

        // Affichage graphique
        SDL_RenderClear(rendu);
        if (fond) SDL_RenderCopy(rendu, fond, NULL, NULL);

        // Affiche toutes les phrases déjà commencées
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

        // Bouton de skip
        if (skip_btn) SDL_RenderCopy(rendu, skip_btn, NULL, &skip_rect);
        SDL_RenderPresent(rendu);

        // Pause entre deux phrases
        if (lettres[phrase] == strlen(phrases[phrase])) {
            SDL_Delay(1000);
            phrase++;
            phraseJouee = false;
        }

        // Gère les clics
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

    // Nettoyage
    if (fond) SDL_DestroyTexture(fond);
    if (skip_btn) SDL_DestroyTexture(skip_btn);
    if (son_phrase) Mix_FreeChunk(son_phrase);
    TTF_CloseFont(police);

    return PAGE_MENU;
}



// === MENU PRINCIPAL ===
Page afficher_menu(SDL_Renderer* rendu) {
    // Joue la musique du menu une seule fois
    if (musiqueRes == 1) {
        jouerMusique("ressource/musique/ogg/menu/menu_1.ogg", 20);
        musiqueRes = 0;
    }

    // Chargement des images (fond, cadres, icônes)
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_titre = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_titre.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");
    SDL_Texture* image_trophee = IMG_LoadTexture(rendu, "ressource/image/utilité/trophee.png");
    SDL_Texture* image_manette = IMG_LoadTexture(rendu, "ressource/image/utilité/manette.png");

    // Police et couleur des textes
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);
    SDL_Color noir = {0, 0, 0, 255};

    // Dimensions pour les titres et boutons
    int tailleTitre = 250;
    int tailleBouton = 190;
    int largeurBouton = 280;
    int largeurTitre = 500;

    // Positionnement du cadre de titre
    SDL_Rect zone_titre = {
        LARGEUR_FENETRE / 2 - largeurTitre / 2,
        10,
        largeurTitre,
        tailleTitre
    };

    // Boutons "Jouer", "Options", "Quitter"
    SDL_Rect boutons[3] = {
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 200, largeurBouton, tailleBouton},
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 360, largeurBouton, tailleBouton},
        {LARGEUR_FENETRE / 2 - largeurBouton / 2, 520, largeurBouton, tailleBouton}
    };
    const char* ids_textes[] = {"jouer", "options", "quitter"};

    // Position des icônes trophée et manette
    SDL_Rect rect_trophee = {LARGEUR_FENETRE - 120, HAUTEUR_FENETRE - 140, 100, 100};
    SDL_Rect rect_manette = {LARGEUR_FENETRE - 120, HAUTEUR_FENETRE - 260, 100, 100};

    // Affichage de base
    SDL_RenderClear(rendu);
    SDL_RenderCopy(rendu, fond, NULL, NULL);
    SDL_RenderCopy(rendu, cadre_titre, NULL, &zone_titre);

    // Affiche les 3 boutons texte
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

    // Affiche les images décoratives
    SDL_RenderCopy(rendu, image_trophee, NULL, &rect_trophee);
    SDL_RenderCopy(rendu, image_manette, NULL, &rect_manette);
    SDL_RenderPresent(rendu);

    // Boucle d’attente des événements
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Libère les ressources si fermeture
                TTF_CloseFont(police);
                SDL_DestroyTexture(fond);
                SDL_DestroyTexture(cadre_titre);
                SDL_DestroyTexture(cadre_bouton);
                SDL_DestroyTexture(image_trophee);
                SDL_DestroyTexture(image_manette);
                return PAGE_QUITTER;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                // Clic sur "Jouer"
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    TTF_CloseFont(police);
                    SDL_DestroyTexture(fond);
                    SDL_DestroyTexture(cadre_titre);
                    SDL_DestroyTexture(cadre_bouton);
                    SDL_DestroyTexture(image_trophee);
                    SDL_DestroyTexture(image_manette);
                    return PAGE_SELEC_MODE;
                }

                // Clic sur "Options"
                if (x >= boutons[1].x && x <= boutons[1].x + boutons[1].w &&
                    y >= boutons[1].y && y <= boutons[1].y + boutons[1].h) {
                    TTF_CloseFont(police);
                    SDL_DestroyTexture(fond);
                    SDL_DestroyTexture(cadre_titre);
                    SDL_DestroyTexture(cadre_bouton);
                    SDL_DestroyTexture(image_trophee);
                    SDL_DestroyTexture(image_manette);
                    return PAGE_OPTIONS;
                }

                // Clic sur "Quitter"
                if (x >= boutons[2].x && x <= boutons[2].x + boutons[2].w &&
                    y >= boutons[2].y && y <= boutons[2].y + boutons[2].h) {
                    TTF_CloseFont(police);
                    SDL_DestroyTexture(fond);
                    SDL_DestroyTexture(cadre_titre);
                    SDL_DestroyTexture(cadre_bouton);
                    SDL_DestroyTexture(image_trophee);
                    SDL_DestroyTexture(image_manette);
                    return PAGE_QUITTER;
                }
            }
        }
    }

    // Code jamais atteint
    TTF_CloseFont(police);
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(cadre_titre);
    SDL_DestroyTexture(cadre_bouton);
    SDL_DestroyTexture(image_trophee);
    SDL_DestroyTexture(image_manette);
    return PAGE_MENU;
}




Page afficher_credit(SDL_Renderer* rendu, Page page_prec);







// === OPTIONS ===
Page afficher_options(SDL_Renderer* rendu, Page page_prec) {
    // Chargement des textures du fond et des éléments visuels
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    // Drapeaux pour le choix de la langue
    SDL_Texture* drapeauAllemand = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauAllemand.png");
    SDL_Texture* drapeauAnglais  = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauAnglais.png");
    SDL_Texture* drapeauEspagnol = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauEspagnol.png");
    SDL_Texture* drapeauFrancais = IMG_LoadTexture(rendu, "ressource/image/utilité/drapeau/drapeauFrancais.png");

    // Chargement des miniatures associées aux musiques de menu
    SDL_Texture* images_musiques[6];
    for (int i = 0; i < 6; i++) {
        char chemin[128];
        sprintf(chemin, "ressource/image/utilité/musique/menu_%d.png", i + 1);
        images_musiques[i] = IMG_LoadTexture(rendu, chemin);
    }

    const char* ids_textes[] = {"credit", "langue", "volume", "musique"};
    SDL_Color blanc = {255, 255, 255, 255};
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 40);

    // Définition des cadres des 4 lignes d’options
    SDL_Rect boutons[4] = {
        {0, -50, 200, 250},
        {0, 100, 200, 250},
        {0, 250, 200, 250},
        {0, 400, 200, 250}
    };

    // Position de la barre de volume
    SDL_Rect barre = {
        (LARGEUR_FENETRE - 300) / 2,
        boutons[2].y + 100,
        300, 30
    };

    SDL_Rect retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80}; // Position du bouton retour

    // Positionnement des vignettes musicales (2 lignes de 3 vignettes)
    SDL_Rect musiques[6];
    for (int i = 0; i < 6; i++) {
        int ligne = i / 3, colonne = i % 3;
        musiques[i].w = 200;
        musiques[i].h = 150;
        int base_x = LARGEUR_FENETRE / 2 - (280 * 3) / 2;
        musiques[i].x = base_x + colonne * 280;
        if (colonne == 1) musiques[i].x += 90;
        if (colonne == 2) musiques[i].x += 150;
        musiques[i].y = barre.y + 100 + ligne * (150 + 30);
        if (ligne == 0) musiques[i].y -= 50;
        if (ligne == 1) musiques[i].y -= 80;
    }

    // Position des drapeaux de langue
    SDL_Rect drapeaux[4];
    int base_x = 350;
    int base_y = boutons[1].y + (boutons[1].h - 110) / 2;
    for (int i = 0; i < 4; i++) {
        drapeaux[i].x = base_x + i * (110 + 60);
        drapeaux[i].y = base_y;
        drapeaux[i].w = 110;
        drapeaux[i].h = 110;
    }

    bool curseur_actif = false; // Pour glisser le volume en continu
    SDL_Event event;

    // Boucle d’affichage
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        // Affichage des cadres et textes des 4 boutons
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

        // Affichage des drapeaux
        SDL_RenderCopy(rendu, drapeauFrancais, NULL, &drapeaux[0]);
        SDL_RenderCopy(rendu, drapeauAnglais,  NULL, &drapeaux[1]);
        SDL_RenderCopy(rendu, drapeauEspagnol, NULL, &drapeaux[2]);
        SDL_RenderCopy(rendu, drapeauAllemand, NULL, &drapeaux[3]);

        // Affichage barre volume (fond + niveau + curseur)
        float ratio = volume_global / 128.0f;
        SDL_SetRenderDrawColor(rendu, 20, 20, 20, 255); // fond noir
        SDL_RenderFillRect(rendu, &(SDL_Rect){barre.x - 4, barre.y - 4, barre.w + 8, barre.h + 8});
        SDL_SetRenderDrawColor(rendu, 40, 40, 40, 255); // barre grise
        SDL_RenderFillRect(rendu, &barre);
        int r = (int)(255 * (1 - ratio)), g = (int)(255 * ratio);
        SDL_SetRenderDrawColor(rendu, r, g, 60, 255); // barre verte-rouge selon niveau
        SDL_Rect niveau = {barre.x, barre.y, volume_global * barre.w / 128, barre.h};
        SDL_RenderFillRect(rendu, &niveau);

        // Affichage du curseur blanc avec effet de halo
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

        // Texte du pourcentage de volume
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

        // Affichage des vignettes musicales
        for (int i = 0; i < 6; i++) {
            SDL_RenderCopy(rendu, images_musiques[i], NULL, &musiques[i]);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour_rect);
        SDL_RenderPresent(rendu);

        // Gestion des clics
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                return PAGE_QUITTER;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                // Clic sur crédit
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    Page retour = afficher_credit(rendu, PAGE_OPTIONS);
                    if (retour == PAGE_QUITTER) {
                        for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                        return PAGE_QUITTER;
                    }
                }

                // Clic sur retour
                if (x >= retour_rect.x && x <= retour_rect.x + retour_rect.w &&
                    y >= retour_rect.y && y <= retour_rect.y + retour_rect.h) {
                    for (int i = 0; i < 6; i++) SDL_DestroyTexture(images_musiques[i]);
                    return page_prec;
                }

                // Clic dans la barre de volume
                if (x >= barre.x && x <= barre.x + barre.w &&
                    y >= barre.y && y <= barre.y + barre.h) {
                    volume_global = (x - barre.x) * 128 / barre.w;
                    if (volume_global < 0) volume_global = 0;
                    if (volume_global > 128) volume_global = 128;
                    Mix_VolumeMusic(volume_global);
                    curseur_actif = true;
                }

                // Clic sur drapeau (changer langue)
                for (int i = 0; i < 4; i++) {
                    if (x >= drapeaux[i].x && x <= drapeaux[i].x + drapeaux[i].w &&
                        y >= drapeaux[i].y && y <= drapeaux[i].y + drapeaux[i].h) {
                        changerLangue((Langue)i);
                    }
                }

                // Clic sur une des musiques
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

            if (event.type == SDL_MOUSEBUTTONUP) curseur_actif = false;

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
        "Yanis Abtta",
        "  Développement du gameplay (attaques, tours, effets)",
        "  Élaboration des interfaces et logique de jeu",
        "",
        "Malak Slimane",
        "  Sélection et structuration des personnages jouables",
        "  Création des attaques spéciales et des effets de statut",
        "  Équilibrage du système de combat",
        "",
        "Collaborations :",
        "Lukas",
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
        "  Certaines musiques ne sont pas libres de droits,",
        "  mais c’était pour la bonne cause : créer une vraie ambiance !",
        "",
        "Merci d’avoir joué à Project Shonen Smash !",
        "Édition 2025 – Forgé entre lignes de code et éclats de pixels",
        "Le rideau tombe...",
        "Mais l'aventure, elle, ne fait que commencer !"
    };

    int nb_lignes = sizeof(lignes) / sizeof(lignes[0]);
    int y_offset = HAUTEUR_FENETRE;

    Mix_HaltMusic();  // Couper la musique précédente
    jouerMusique("ressource/musique/ogg/credit.ogg", volume_global); // Lancer la musique des crédits

    while (1) {
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        for (int i = 0; i < nb_lignes; i++) {
            if (!lignes[i] || strlen(lignes[i]) == 0) continue;

            SDL_Color couleur;

            if (strcmp(lignes[i], "Project Shonen Smash – Credits") == 0) {
                couleur = (SDL_Color){100, 180, 255, 255}; // Bleu
            } else if (
                strcmp(lignes[i], "Mansour Wajih") == 0 ||
                strcmp(lignes[i], "Yanis Abtta") == 0 ||
                strcmp(lignes[i], "Malak Slimane") == 0 ||
                strcmp(lignes[i], "Lukas") == 0 ||
                strcmp(lignes[i], "Nemu") == 0
            ) {
                couleur = (SDL_Color){255, 70, 70, 255}; // Rouge
            } else if (
                strcmp(lignes[i], "Développement principal :") == 0 ||
                strcmp(lignes[i], "Collaborations :") == 0 ||
                strcmp(lignes[i], "Technologies utilisées :") == 0 ||
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
        y_offset -= 5; // Texte qui défile vers le haut

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

        // Fin automatique si tout a défilé
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
    // Nettoie l'écran avec un fond noir
    SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);
    SDL_RenderClear(rendu);

    const int taille_perso = 100; // Taille carrée des sprites affichés
    const int marge = 50;         // Marge par rapport aux bords

    // Affiche les 3 personnages de l'équipe 1 à gauche
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

    // Affiche les 3 personnages de l'équipe 2 à droite
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

    // Affiche "JEU EN COURS..." en bas de l'écran
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

    SDL_RenderPresent(rendu); // Affichage final

    // Boucle d'attente d'événements : QUIT ou clic/clavier → retour au menu
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                return PAGE_MENU;
            }
        }
        SDL_Delay(16); // ~60 FPS
    }
}







// === PAGE DE SÉLECTION MODE ===
Page afficher_selec_mode(SDL_Renderer* rendu) {
    // Chargement des textures et polices
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    TTF_Font* police_titre = TTF_OpenFont("ressource/langue/police/arial.ttf", 50);
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 65);
    TTF_SetFontStyle(police, TTF_STYLE_BOLD);
    SDL_Color noir = {0, 0, 0, 255};

    // Dimensions des boutons
    int marge = 30;
    int largeur_bouton = (LARGEUR_FENETRE - 3 * marge) / 2;
    int hauteur_bouton = HAUTEUR_FENETRE - 2 * marge - 100;

    // Textes à afficher dans les deux colonnes : J1 VS J2 et J1 VS IA
    const char* textes[2][3] = {
        {"J1", "VS", "J2"},
        {"J1", "VS", "IA"}
    };

    SDL_Rect boutons[2] = {
        {marge, marge + 100, largeur_bouton, hauteur_bouton},
        {2 * marge + largeur_bouton, marge + 100, largeur_bouton, hauteur_bouton}
    };

    SDL_Rect retour = {20, HAUTEUR_FENETRE - 100, 80, 80};

    // Création du titre
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

        // Affichage des 2 gros boutons
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

        // Gestion des clics
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x, y = event.button.y;

                if (x >= retour.x && x <= retour.x + retour.w &&
                    y >= retour.y && y <= retour.y + retour.h)
                    return PAGE_MENU;

                // Bouton J1 vs J2
                if (x >= boutons[0].x && x <= boutons[0].x + boutons[0].w &&
                    y >= boutons[0].y && y <= boutons[0].y + boutons[0].h) {
                    chemin_retour = 0;
                    partieActuelle.iaDifficulte = 0; // Pas d'IA
                    return PAGE_SELECTION_PERSO;
                }

                // Bouton J1 vs IA
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
    // Chargement des textures et polices
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_menu.png");
    SDL_Texture* cadre_bouton = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte_carre.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");

    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 38);
    TTF_Font* police_titre = TTF_OpenFont("ressource/langue/police/arial.ttf", 56);

    SDL_Color doré = {255, 215, 0, 255};
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color noir = {0, 0, 0, 255};

    // Identifiants de texte pour chaque bouton
    const char* ids_difficulte[] = {"facile", "moyen", "difficile"};

    // Positionnement des boutons
    int largeur_bouton = 450;
    int hauteur_bouton = 120;
    int espacement = 50;
    int x_centre = (LARGEUR_FENETRE - largeur_bouton) / 2;
    int start_y = 250;

    SDL_Rect boutons[3];
    for (int i = 0; i < 3; i++) {
        boutons[i].x = x_centre;
        boutons[i].y = start_y + i * (hauteur_bouton + espacement);
        boutons[i].w = largeur_bouton;
        boutons[i].h = hauteur_bouton;
    }

    SDL_Rect retour = {20, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Event event;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);

        // Affichage du titre avec ombre
        SDL_Surface* surf_ombre_titre = TTF_RenderUTF8_Solid(police_titre, "Choix de la Difficulté", noir);
        SDL_Surface* surf_titre = TTF_RenderUTF8_Solid(police_titre, "Choix de la Difficulté", doré);
        SDL_Texture* tex_ombre_titre = SDL_CreateTextureFromSurface(rendu, surf_ombre_titre);
        SDL_Texture* tex_titre = SDL_CreateTextureFromSurface(rendu, surf_titre);

        SDL_Rect rect_titre = {
            (LARGEUR_FENETRE - surf_titre->w) / 2,
            80,
            surf_titre->w,
            surf_titre->h
        };
        SDL_Rect rect_ombre = rect_titre;
        rect_ombre.x += 3;
        rect_ombre.y += 3;

        SDL_RenderCopy(rendu, tex_ombre_titre, NULL, &rect_ombre);
        SDL_RenderCopy(rendu, tex_titre, NULL, &rect_titre);

        SDL_FreeSurface(surf_ombre_titre);
        SDL_FreeSurface(surf_titre);
        SDL_DestroyTexture(tex_ombre_titre);
        SDL_DestroyTexture(tex_titre);

        // Gestion du survol souris et affichage des 3 boutons
        int x, y;
        SDL_GetMouseState(&x, &y);

        for (int i = 0; i < 3; i++) {
            SDL_Rect* btn = &boutons[i];
            bool survol = (x >= btn->x && x <= btn->x + btn->w &&
                           y >= btn->y && y <= btn->y + btn->h);

            SDL_SetTextureColorMod(cadre_bouton, survol ? 255 : 255,
                                                  survol ? 230 : 255,
                                                  survol ? 150 : 255);

            SDL_RenderCopy(rendu, cadre_bouton, NULL, btn);

            SDL_Surface* surf_ombre = TTF_RenderUTF8_Solid(police, getTexte(ids_difficulte[i]), noir);
            SDL_Surface* surf_txt = TTF_RenderUTF8_Solid(police, getTexte(ids_difficulte[i]), blanc);

            SDL_Texture* tex_ombre = SDL_CreateTextureFromSurface(rendu, surf_ombre);
            SDL_Texture* tex_txt = SDL_CreateTextureFromSurface(rendu, surf_txt);

            SDL_Rect rect_txt = {
                btn->x + (btn->w - surf_txt->w) / 2,
                btn->y + (btn->h - surf_txt->h) / 2,
                surf_txt->w, surf_txt->h
            };
            SDL_Rect rect_ombre_txt = rect_txt;
            rect_ombre_txt.x += 2;
            rect_ombre_txt.y += 2;

            SDL_RenderCopy(rendu, tex_ombre, NULL, &rect_ombre_txt);
            SDL_RenderCopy(rendu, tex_txt, NULL, &rect_txt);

            SDL_FreeSurface(surf_ombre);
            SDL_FreeSurface(surf_txt);
            SDL_DestroyTexture(tex_ombre);
            SDL_DestroyTexture(tex_txt);
        }

        SDL_RenderCopy(rendu, bouton_retour, NULL, &retour);
        SDL_RenderPresent(rendu);

        // Gestion des clics
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return PAGE_QUITTER;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                x = event.button.x;
                y = event.button.y;

                if (x >= retour.x && x <= retour.x + retour.w &&
                    y >= retour.y && y <= retour.y + retour.h) {
                    return PAGE_SELEC_MODE;
                }

                for (int i = 0; i < 3; i++) {
                    if (x >= boutons[i].x && x <= boutons[i].x + boutons[i].w &&
                        y >= boutons[i].y && y <= boutons[i].y + boutons[i].h) {
                        partieActuelle.iaDifficulte = i + 1; // 1 = facile, 2 = moyen, 3 = difficile
                        return PAGE_SELECTION_PERSO;
                    }
                }
            }
        }
    }
}




