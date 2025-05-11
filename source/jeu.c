#include "data.h"
#include "interface.h"
#include "attaque.h"
#include "langue.h"
#include "son.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>



// Variables globales
int ecartementPont = 40;
extern TTF_Font* policePetite;
int idIncassble;
int dureeMur;
extern int volume_global;

// Tableau des actions à effectuer pour chaque personnage pendant un tour
AttaqueSauvegarde tableauAttaqueDuTour [NB_PERSOS_EQUIPE * 2];

// Applique les effets de statut à un combattant sans modifier l'original directement
Fighter appliquer_modificateurs(Fighter* original) {
    Fighter copie = *original;

    switch (copie.statutEffet) {
        case 3: copie.defense += copie.defense * 0.25; break; // boost def
        case 4: copie.attaque += copie.attaque * 0.25; break; // boost att
        case 5: copie.vitesse += copie.vitesse * 0.25; break; // boost vitesse
        case 6: copie.defense -= copie.defense * 0.25; break; // nerf def
        case 7: copie.attaque -= copie.attaque * 0.25; break; // nerf att
        case 8: copie.vitesse -= copie.vitesse * 0.25; break; // nerf vitesse
        case 9: copie.agilite -= copie.agilite * 0.25; break; // nerf agilité
        case 10: copie.agilite += copie.agilite * 0.25; break; // boost agilité
        case 11: // gel = -10% partout
            copie.attaque  -= copie.attaque  * 0.1;
            copie.defense  -= copie.defense  * 0.1;
            copie.vitesse  -= copie.vitesse  * 0.1;
            copie.agilite  -= copie.agilite  * 0.1;
            break;
        case 13: copie.defense += copie.defense * 0.1; break; // défense classique
    }

    return copie;
}

// Met à jour les effets actifs sur un personnage (perte de PV, fin d'effet...)
void appliquer_et_mettre_a_jour_effets(Fighter* perso) {
    if (perso->dureeEffet <= 0 || perso->statutEffet == 0) return;

    switch (perso->statutEffet) {
        case 1: { // Saignement
            int perte = perso->max_pv * 0.1;
            perso->actu_pv -= perte;
            if (perso->actu_pv < 0) perso->actu_pv = 0;
            break;
        }
        case 2: { // Brûlure
            int perte = perso->max_pv * 0.05;
            perso->actu_pv -= perte;
            if (perso->actu_pv < 0) perso->actu_pv = 0;
            break;
        }
        case 12: break; // Paralysie → rien à faire ici
    }

    perso->dureeEffet--;
    if (perso->dureeEffet <= 0) {
        perso->statutEffet = 0;
    }
}


// Sélectionne visuellement une cible à l'écran pour une attaque
AttaqueSauvegarde choisirCible(SDL_Renderer* rendu, int equipeCible, AttaqueSauvegarde attaque) {
    bool choisi = false;
    SDL_Event event;
    int mx, my;

    // Flèche de sélection visuelle
    SDL_Texture* arrowTexture = IMG_LoadTexture(rendu, "ressource/image/utilité/cibleSelect.png");

    Fighter* cibles[3];
    int x_start, direction;

    // Détermine les personnages à cibler selon l'équipe
    if (equipeCible == 1) {
        cibles[0] = &partieActuelle.joueur1.fighter1;
        cibles[1] = &partieActuelle.joueur1.fighter2;
        cibles[2] = &partieActuelle.joueur1.fighter3;
        x_start = 100;
        direction = 1;
    } else if (equipeCible == 2) {
        cibles[0] = &partieActuelle.joueur2.fighter1;
        cibles[1] = &partieActuelle.joueur2.fighter2;
        cibles[2] = &partieActuelle.joueur2.fighter3;
        x_start = LARGEUR_FENETRE - 200;
        direction = -1;
    } else {
        return attaque; // Équipe inconnue
    }

    while (!choisi) {
        SDL_GetMouseState(&mx, &my);
        renduJeu(rendu); // Affiche les persos normalement

        // Affiche la flèche au survol de chaque personnage
        for (int i = 0; i < 3; i++) {
            int x = x_start + direction * i * (100 + 30);
            int y = (HAUTEUR_FENETRE - 100) / 2 + i * 30 + ecartementPont;
            SDL_Rect zone = {x, y, 100, 100};

            if (mx >= zone.x && mx <= zone.x + zone.w &&
                my >= zone.y && my <= zone.y + zone.h) {
                if (arrowTexture) {
                    SDL_Rect arrowRect = {zone.x + (zone.w - 30) / 2, zone.y - 35, 30, 30};
                    SDL_RenderCopy(rendu, arrowTexture, NULL, &arrowRect);
                }
            }
        }

        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(0);

            // Clic gauche
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 3; i++) {
                    int x = x_start + direction * i * (100 + 30);
                    int y = (HAUTEUR_FENETRE - 100) / 2 + i * 30 + ecartementPont;
                    SDL_Rect zone = {x, y, 100, 100};

                    if (mx >= zone.x && mx <= zone.x + zone.w &&
                        my >= zone.y && my <= zone.y + zone.h) {
                        attaque.cibleEquipe = equipeCible;
                        attaque.cibleNum = (equipeCible == 2) ? i + 3 : i;

                        if (cibles[i]->actu_pv > 0) {
                            choisi = true;
                        }
                    }
                }
            }
        }

        SDL_Delay(8);
    }

    if (arrowTexture) SDL_DestroyTexture(arrowTexture);
    return attaque;
}


// Affiche un bouton avec surbrillance et texte centré
void drawButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font) {
    SDL_Color color = btn->hovered ? btn->hoverColor : btn->baseColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &btn->rect);

    if (!font) return;

    // Chargement temporaire de police ajustée pour adapter la taille au bouton
    int fontSize = 40;
    TTF_Font* adjustedFont = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
    if (!adjustedFont) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(adjustedFont, btn->text, (SDL_Color){255, 255, 255, 255});

    while (surf && surf->w > btn->rect.w) {
        SDL_FreeSurface(surf);
        fontSize--;
        TTF_CloseFont(adjustedFont);
        adjustedFont = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
        if (!adjustedFont) return;
        surf = TTF_RenderUTF8_Blended(adjustedFont, btn->text, (SDL_Color){255, 255, 255, 255});
    }

    if (!surf) {
        TTF_CloseFont(adjustedFont);
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        SDL_FreeSurface(surf);
        TTF_CloseFont(adjustedFont);
        return;
    }

    int tw, th;
    SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect textRect = {
        btn->rect.x + (btn->rect.w - tw) / 2,
        btn->rect.y + (btn->rect.h - th) / 2,
        tw, th
    };

    SDL_RenderCopy(renderer, tex, NULL, &textRect);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    TTF_CloseFont(adjustedFont);
}

// Récupère le pointeur vers un fighter à partir d’un index global (0 à 5)
Fighter* get_fighter_by_index(int index) {
    switch(index) {
        case 0: return &partieActuelle.joueur1.fighter1;
        case 1: return &partieActuelle.joueur2.fighter1;
        case 2: return &partieActuelle.joueur1.fighter2;
        case 3: return &partieActuelle.joueur2.fighter2;
        case 4: return &partieActuelle.joueur1.fighter3;
        case 5: return &partieActuelle.joueur2.fighter3;
        default: return NULL;
    }
}

// Vérifie si la souris est au-dessus d’un bouton
bool isMouseOver(Button* btn, int x, int y) {
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}

// Retourne vrai si l'attaque est un soin
bool est_une_attaque_de_soin(int id) {
    return id == ATQ_GLACE_CURATIVE ||
           id == ATQ_VAGUE_GUERISSEUSE ||
           id == ATQ_SOUFFLE_DE_VIE;
}

// Retourne vrai si l'attaque est un boost de défense
bool est_un_boost_de_def(int id){
    return id == ATQ_ESPRIT_FLAMBOYANT ||
           id == ATQ_BARRIERE_DE_PIERRE ||
           id == ATQ_RUGISSEMENT_D_ACIER ||
           id == ATQ_BRUME_PROTECTRICE;
}

// Retourne vrai si l'attaque est un boost d’attaque
bool est_un_boost_att(int id){
    return id == ATQ_EVEIL_SABRE ||
           id == ATQ_ESPRIT_FLAMBOYANT;
}

// Vérifie si l’attaque est Mur Vivant
bool est_mur_vivant(int id) {
    return id == ATQ_MUR_VIVANT;
}

// Vérifie si l’attaque cible forcément soi-même ou est globale
bool attaque_cible_soi_meme(int idAttaque) {
    return idAttaque == AFFUTAGE_MORTAL ||
           idAttaque == EVEIL_DU_SABRE ||
           idAttaque == EVEIL_LUNAIRE ||
           idAttaque == CREPUSCULE ||
           idAttaque == FLAMMES_SOLAIRES ||
           idAttaque == BARRIERE_DE_PIERRE ||
           idAttaque == MUR_VIVANT ||
           idAttaque == HURLEMENT_NOIR ||
           idAttaque == VENT_PERÇANT ||
           idAttaque == FOUDRE_ENCHAINEE ||
           idAttaque == BLIZZARD;
}

// Affiche la page de pause avec trois boutons : Continuer, Son, Quitter
bool afficher_pause(SDL_Renderer* rendu) {
    SDL_Event event;
    bool continuer = true;
    bool quitter_pause = false;
    bool dansPageSon = false;

    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 50);
    if (!font) return true;

    // Déclaration des boutons
    Button btnContinuer = {{screenWidth/2 - 150, 200, 300, 80}, {80,80,80,255}, {150,150,150,255}, false, "Continuer"};
    Button btnSon       = {{screenWidth/2 - 150, 310, 300, 80}, {80,80,80,255}, {150,150,150,255}, false, "Son"};
    Button btnQuitter   = {{screenWidth/2 - 150, 420, 300, 80}, {80,80,80,255}, {150,150,150,255}, false, "Quitter"};

    // Barre de volume
    SDL_Rect sliderBar = {screenWidth/2 - 150, 300, 300, 10};
    SDL_Rect sliderKnob = {sliderBar.x + 150 - 5, sliderBar.y - 5, 10, 20}; // position au milieu

    SDL_Texture* flecheRetour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");
    SDL_Rect rectRetour = {30, 30, 60, 60};

    while (!quitter_pause && continuer) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        SDL_SetRenderDrawColor(rendu, 10, 10, 10, 240); // fond noir
        SDL_RenderClear(rendu);

        if (!dansPageSon) {
            // Gère le survol des boutons
            btnContinuer.hovered = isMouseOver(&btnContinuer, mx, my);
            btnSon.hovered = isMouseOver(&btnSon, mx, my);
            btnQuitter.hovered = isMouseOver(&btnQuitter, mx, my);

            // Affiche les boutons
            drawButton(rendu, &btnContinuer, font);
            drawButton(rendu, &btnSon, font);
            drawButton(rendu, &btnQuitter, font);
        } else {
            // Page du son
            SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);
            SDL_RenderFillRect(rendu, &sliderBar);

            SDL_SetRenderDrawColor(rendu, 200, 0, 0, 255);
            SDL_RenderFillRect(rendu, &sliderKnob);

            if (flecheRetour)
                SDL_RenderCopy(rendu, flecheRetour, NULL, &rectRetour);
        }

        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(0);

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (!dansPageSon) {
                    if (btnContinuer.hovered) quitter_pause = true;
                    else if (btnSon.hovered) dansPageSon = true;
                    else if (btnQuitter.hovered) exit(0);
                } else {
                    // Retour
                    if (mx >= rectRetour.x && mx <= rectRetour.x + rectRetour.w &&
                        my >= rectRetour.y && my <= rectRetour.y + rectRetour.h) {
                        dansPageSon = false;
                    }
                    // Ajuste le volume
                    else if (mx >= sliderBar.x && mx <= sliderBar.x + sliderBar.w) {
                        sliderKnob.x = mx - sliderKnob.w / 2;
                        int volume = (sliderKnob.x - sliderBar.x) * 128 / sliderBar.w;
                        if (volume < 0) volume = 0;
                        if (volume > 128) volume = 128;
                        Mix_VolumeMusic(volume);
                    }
                }
            }
        }

        SDL_Delay(16);
    }

    if (flecheRetour) SDL_DestroyTexture(flecheRetour);
    TTF_CloseFont(font);
    return true;
}

// Affiche les boutons d’action d’un personnage et gère l’entrée joueur
void actionPerso(SDL_Renderer* renderer, Fighter* persoActuel, int equipeAdverse) {
    if (!policePrincipale || !policePetite) return;

    // Définition des boutons
    Button btnAttaque = {{20, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Attaque"};
    Button btnDefense = {{40 + btnAttaque.rect.w, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Défense"};
    Button btnComp1 = {{screenWidth - 170, screenHeight - 100, 150, 60}, {60,60,60,255}, {120,120,120,255}, false, persoActuel->spe_atq1.nom};
    Button btnComp2 = {{screenWidth - 340, screenHeight - 100, 150, 60}, {60,60,60,255}, {120,120,120,255}, false, persoActuel->spe_atq2.nom};
    Button btnComp3 = {{screenWidth - 510, screenHeight - 100, 150, 60}, {60,60,60,255}, {120,120,120,255}, false, persoActuel->spe_atq3.nom};

    SDL_Event event;
    bool quit = false;

    // Affiche les coûts des attaques spéciales
    SDL_Color jaune = {255, 255, 0, 255};
    char cout1[16], cout2[16], cout3[16];
    snprintf(cout1, sizeof(cout1), "%d PT", persoActuel->spe_atq1.cout);
    snprintf(cout2, sizeof(cout2), "%d PT", persoActuel->spe_atq2.cout);
    snprintf(cout3, sizeof(cout3), "%d PT", persoActuel->spe_atq3.cout);

    SDL_Surface* s1 = TTF_RenderUTF8_Blended(policePetite, cout1, jaune);
    SDL_Surface* s2 = TTF_RenderUTF8_Blended(policePetite, cout2, jaune);
    SDL_Surface* s3 = TTF_RenderUTF8_Blended(policePetite, cout3, jaune);

    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);
    SDL_Texture* t3 = SDL_CreateTextureFromSurface(renderer, s3);

    SDL_Rect r1 = {btnComp1.rect.x + (btnComp1.rect.w - s1->w)/2, btnComp1.rect.y - s1->h - 5, s1->w, s1->h};
    SDL_Rect r2 = {btnComp2.rect.x + (btnComp2.rect.w - s2->w)/2, btnComp2.rect.y - s2->h - 5, s2->w, s2->h};
    SDL_Rect r3 = {btnComp3.rect.x + (btnComp3.rect.w - s3->w)/2, btnComp3.rect.y - s3->h - 5, s3->w, s3->h};

    while (!quit) {
        if (tableauAttaqueDuTour[partieActuelle.perso_actif].idAttaque >= 0) return;

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        // Mise à jour du survol des boutons
        btnAttaque.hovered = isMouseOver(&btnAttaque, mx, my);
        btnDefense.hovered = isMouseOver(&btnDefense, mx, my);
        btnComp1.hovered = isMouseOver(&btnComp1, mx, my);
        btnComp2.hovered = isMouseOver(&btnComp2, mx, my);
        btnComp3.hovered = isMouseOver(&btnComp3, mx, my);

        // Affichage
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        renduJeu(renderer);

        drawButton(renderer, &btnAttaque, policePrincipale);
        drawButton(renderer, &btnDefense, policePrincipale);
        drawButton(renderer, &btnComp1, policePrincipale);
        drawButton(renderer, &btnComp2, policePrincipale);
        drawButton(renderer, &btnComp3, policePrincipale);

        SDL_RenderCopy(renderer, t1, NULL, &r1);
        SDL_RenderCopy(renderer, t2, NULL, &r2);
        SDL_RenderCopy(renderer, t3, NULL, &r3);
        SDL_RenderPresent(renderer);

        // Événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(0);
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int id = partieActuelle.perso_actif;
                AttaqueSauvegarde* attaque = &tableauAttaqueDuTour[id];

                if (btnAttaque.hovered) {
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = ATTAQUE_BASIQUE,
                        .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                        .utilisateurNum = id,
                    };
                    *attaque = choisirCible(renderer, equipeAdverse, *attaque);
                    if (persoActuel->pt < 10) persoActuel->pt += 1;
                    quit = true;
                    break;
                }

                else if (btnDefense.hovered) {
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = DEFENSE,
                        .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                        .utilisateurNum = id,
                        .cibleEquipe = -1,
                        .cibleNum = -1
                    };
                    if (persoActuel->pt < 10) persoActuel->pt += 2;
                    quit = true;
                    break;
                }

                else {
                    // Gestion des trois compétences spéciales
                    AttaqueSpecial* specs[] = {&persoActuel->spe_atq1, &persoActuel->spe_atq2, &persoActuel->spe_atq3};
                    Button* btns[] = {&btnComp1, &btnComp2, &btnComp3};
                    //SDL_Texture* txts[] = {t1, t2, t3};
                    //SDL_Surface* surfs[] = {s1, s2, s3};

                    for (int k = 0; k < 3; k++) {
                        if (btns[k]->hovered && persoActuel->pt >= specs[k]->cout) {
                            persoActuel->pt -= specs[k]->cout;
                            *attaque = (AttaqueSauvegarde){
                                .idAttaque = specs[k]->id,
                                .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                                .utilisateurNum = id,
                            };

                            int equipeCible = (est_une_attaque_de_soin(attaque->idAttaque) || est_un_boost_de_def(attaque->idAttaque))
                                ? ((equipeAdverse == 1) ? 2 : 1)
                                : equipeAdverse;

                            if (attaque_cible_soi_meme(attaque->idAttaque)) {
                                attaque->cibleEquipe = attaque->utilisateurEquipe;
                                attaque->cibleNum = attaque->utilisateurNum;
                            } else {
                                *attaque = choisirCible(renderer, equipeCible, *attaque);
                            }

                            quit = true;
                            break;
                        }
                    }
                }

            } else if (event.type == SDL_KEYDOWN) {
                // Débogage
                switch (event.key.keysym.sym) {
                    case SDLK_p: persoActuel->pt = 10; break;
                    case SDLK_m: persoActuel->actu_pv = persoActuel->max_pv; break;
                    case SDLK_DELETE: persoActuel->actu_pv = 0; quit = true; break;
                    case SDLK_ESCAPE: afficher_pause(renderer); break;
                }
            }
        }

        SDL_Delay(16);
    }

    // Nettoyage mémoire
    SDL_DestroyTexture(t1); SDL_FreeSurface(s1);
    SDL_DestroyTexture(t2); SDL_FreeSurface(s2);
    SDL_DestroyTexture(t3); SDL_FreeSurface(s3);
}
















Page afficher_ecran_fin(SDL_Renderer* rendu, int gagnant) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fin.png");
    if (!fond) {
        SDL_Log("Erreur chargement fond victoire : %s", SDL_GetError());
        return PAGE_MENU;
    }

    // Chargement police et message
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 60);
    SDL_Color blanc = {255, 255, 255, 255};

    const char* message = "";
    if (partieActuelle.iaDifficulte > 0) {
        message = (gagnant == 1) ? "Victoire du Joueur !" : "Défaite... L'IA gagne.";
        jouerMusique((gagnant == 1) ? "ressource/musique/ogg/victoire.ogg" : "ressource/musique/ogg/defaite.ogg", volume_global);
    } else {
        message = (gagnant == 1) ? "Victoire de l'équipe 1 !" : "Victoire de l'équipe 2 !";
        jouerMusique("ressource/musique/ogg/victoire.ogg", volume_global);
    }

    SDL_Surface* texte = TTF_RenderUTF8_Blended(police, message, blanc);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rendu, texte);

    SDL_Rect rectTexte = {
        (LARGEUR_FENETRE - texte->w) / 2,
        (HAUTEUR_FENETRE - texte->h) / 2,
        texte->w,
        texte->h
    };

    // Affichage fond + message
    SDL_RenderClear(rendu);
    SDL_RenderCopy(rendu, fond, NULL, NULL);
    SDL_RenderCopy(rendu, tex, NULL, &rectTexte);
    SDL_RenderPresent(rendu);

    SDL_FreeSurface(texte);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(fond);
    TTF_CloseFont(police);

    // Pause 3 secondes avant de revenir au menu
    SDL_Delay(10000);
    return PAGE_MENU;
}












// Fonction principale de déroulement du combat tour par tour
void runGame(SDL_Renderer* rendu) {
    arreter_musique("ressource/musique/ogg/selection_personnages.ogg");
    SDL_GetWindowSize(fenetre, &screenWidth, &screenHeight);

    // Sélection aléatoire d'une map
    partieActuelle.mapType = rand() % 9;

    // Affectation des personnages aux équipes selon la sélection
    partieActuelle.joueur1.fighter1 = persoChoisi[0];
    partieActuelle.joueur1.fighter2 = persoChoisi[2];
    partieActuelle.joueur1.fighter3 = persoChoisi[4];
    partieActuelle.joueur2.fighter1 = persoChoisi[1];
    partieActuelle.joueur2.fighter2 = persoChoisi[3];
    partieActuelle.joueur2.fighter3 = persoChoisi[5];

    // Application des boosts d’élément et détection d’"incassable"
    Fighter* team[] = {
        &partieActuelle.joueur1.fighter1,
        &partieActuelle.joueur1.fighter2,
        &partieActuelle.joueur1.fighter3,
        &partieActuelle.joueur2.fighter1,
        &partieActuelle.joueur2.fighter2,
        &partieActuelle.joueur2.fighter3
    };

    for (int i = 0; i < 6; i++) {
        if (team[i]->element == partieActuelle.mapType) {
            team[i]->max_pv += 20;
            team[i]->actu_pv += 20;
        }
        if (strcmp(team[i]->nom, "incassable") == 0) {
            idIncassble = i;
        }
    }

    dureeMur = 0;
    partieActuelle.perso_actif = 0;
    partieActuelle.tour = 1;
    partieActuelle.equipeQuiCommence = rand() % 2 + 1;
    partieActuelle.fin = false;

    // Musique de combat
    char musiquePath[128];
    snprintf(musiquePath, sizeof(musiquePath), "ressource/musique/ogg/jeu/combat_%d.ogg", partieActuelle.mapType);
    jouerMusique(musiquePath, 20);

    // Ajustement de l'espacement selon la map
    switch (partieActuelle.mapType){
        case 0: ecartementPont = -25; break;
        case 1: ecartementPont = 20; break;
        case 2: ecartementPont = -25; break;
        case 3: ecartementPont = -10; break;
        case 4: ecartementPont = -20; break;
        case 5: ecartementPont = 10; break;
        case 6: ecartementPont = -20; break;
        case 7: ecartementPont = 70; break;
        case 8: ecartementPont = -50; break;
    }

    // Boucle principale du jeu
    while (!partieActuelle.fin) {
        for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++)
            tableauAttaqueDuTour[i] = (AttaqueSauvegarde){ .idAttaque = -1 };

        renduJeu(rendu);
        SDL_Log("==================================== Tour %d =======================================", partieActuelle.tour);
        animationNouveauTour(rendu, partieActuelle.tour);

        int equipeDebut = (partieActuelle.tour % 2 == 0)
            ? partieActuelle.equipeQuiCommence
            : 3 - partieActuelle.equipeQuiCommence;

        // Tour de chaque équipe
        for (int e = 0; e < 2; e++) {
            int equipe = (e == 0) ? equipeDebut : 3 - equipeDebut;
            for (int i = 0; i < 3; i++) {
                int index = (equipe == 1) ? i : i + 3;
                Fighter* perso = get_fighter(index);
                if (tableauAttaqueDuTour[index].idAttaque >= 0 || perso->actu_pv <= 0) continue;

                appliquer_et_mettre_a_jour_effets(perso);
                partieActuelle.perso_actif = index;

                if (equipe == 2 && partieActuelle.iaDifficulte > 0) {
                    AttaqueSauvegarde* action = &tableauAttaqueDuTour[index];
                    int choix = rand() % 100;

                    // === IA Facile ===
                    if (partieActuelle.iaDifficulte == 1) {
                        if (choix <= 10 && perso->pt >= perso->spe_atq1.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq1.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else if (choix <= 20 && perso->pt >= perso->spe_atq2.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq2.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else if (choix <= 30 && perso->pt >= perso->spe_atq3.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq3.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else if (choix <= 65) {
                            *action = (AttaqueSauvegarde){.idAttaque = DEFENSE, .utilisateurEquipe = 2, .utilisateurNum = index, .cibleEquipe = -1, .cibleNum = -1};
                            if (perso->pt < 10) perso->pt += 2;
                        } else {
                            *action = (AttaqueSauvegarde){.idAttaque = ATTAQUE_BASIQUE, .utilisateurEquipe = 2, .utilisateurNum = index};
                            if (perso->pt < 10) perso->pt += 1;
                        }
                    }

                    // === IA Moyenne ===
                    else if (partieActuelle.iaDifficulte == 2) {
                        if (perso->pt >= perso->spe_atq3.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq3.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else if (perso->pt >= perso->spe_atq2.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq2.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else if (perso->pt >= perso->spe_atq1.cout)
                            *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq1.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                        else {
                            *action = (AttaqueSauvegarde){.idAttaque = ATTAQUE_BASIQUE, .utilisateurEquipe = 2, .utilisateurNum = index};
                            if (perso->pt < 10) perso->pt += 1;
                        }
                    }

                    // === IA Difficile ===
                    else if (partieActuelle.iaDifficulte >= 3) {
                        int cibleFaible = -1, minPV = 999999;
                        for (int j = 0; j < 3; j++) {
                            Fighter* f = get_fighter(j);
                            if (f->actu_pv > 0 && f->actu_pv < minPV) {
                                cibleFaible = j;
                                minPV = f->actu_pv;
                            }
                        }
                        *action = (AttaqueSauvegarde){
                            .idAttaque = perso->pt >= perso->spe_atq3.cout ? perso->spe_atq3.id :
                                         perso->pt >= perso->spe_atq2.cout ? perso->spe_atq2.id :
                                         ATTAQUE_BASIQUE,
                            .utilisateurEquipe = 2,
                            .utilisateurNum = index
                        };
                        if (cibleFaible != -1) {
                            action->cibleEquipe = 1;
                            action->cibleNum = cibleFaible;
                        }
                    }

                    if (action->idAttaque != DEFENSE) {
                        int vivants[3], n = 0;
                        for (int j = 0; j < 3; j++) {
                            if (get_fighter(j)->actu_pv > 0) vivants[n++] = j;
                        }
                        if (n > 0 && action->cibleEquipe != 1) {
                            int cibleIndex = vivants[rand() % n];
                            action->cibleEquipe = 1;
                            action->cibleNum = cibleIndex;
                        }
                    }

                    SDL_Delay(500);
                } else {
                    actionPerso(rendu, perso, (equipe == 1) ? 2 : 1);
                }
            }
        }

        // Tri des personnages selon la vitesse
        int ordre[6] = {0,1,2,3,4,5};
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 6; j++) {
                Fighter a = appliquer_modificateurs(get_fighter(ordre[i]));
                Fighter b = appliquer_modificateurs(get_fighter(ordre[j]));
                if (b.vitesse > a.vitesse) {
                    int tmp = ordre[i];
                    ordre[i] = ordre[j];
                    ordre[j] = tmp;
                }
            }
        }

        // Exécution des attaques
        for (int i = 0; i < 6; i++) {
            int idx = ordre[i];
            AttaqueSauvegarde a = tableauAttaqueDuTour[idx];
            if (a.idAttaque >= 0 && toutes_les_attaques[a.idAttaque]) {
                Fighter* u = get_fighter(a.utilisateurNum);
                Fighter* c = get_fighter(a.cibleNum);
                SDL_Rect ru = get_rect_fighter(u), rc = get_rect_fighter(c);
                jouerAnimationAttaque(rendu, toutes_les_attaques[a.idAttaque]->type, ru, rc, u->element);
                renduJeu(rendu);
                if (c && c->pt < 10) c->pt++;
                fonctions_attaques[a.idAttaque](u, c);
            }
        }

        // Vérifie fin du jeu
        if (equipe_est_morte(1)) {
            SDL_Log("Victoire équipe 2 !");
            break;
        }
        if (equipe_est_morte(2)) {
            SDL_Log("Victoire équipe 1 !");
            break;
        }

        // Nettoyage des effets temporaires
        for (int i = 0; i < 6; i++) {
            Fighter* f = get_fighter(i);
            if (f->dureeEffet > 0) {
                f->dureeEffet--;
                if (f->dureeEffet == 0) f->statutEffet = 0;
            }
            persoChoisi[i].protegePar = -1;
        }

        if (dureeMur > 0) dureeMur--;
        partieActuelle.tour++;
    }

     // Détermination du gagnant
    int gagnant = -1;
    if (equipe_est_morte(1)) gagnant = 2;
    else if (equipe_est_morte(2)) gagnant = 1;

    if (gagnant != -1) {
        SDL_Log("Victoire équipe %d !", gagnant);
        afficher_ecran_fin(rendu, gagnant);
        return;
    }

    SDL_Log("Fin du jeu !");
    exit(0);
}