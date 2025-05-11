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

int ecartementPont = 40;
extern TTF_Font* policePetite;
int idIncassble;
int dureeMur;

AttaqueSauvegarde tableauAttaqueDuTour [NB_PERSOS_EQUIPE * 2];

// ====================== GAMEPLAY ====================

Fighter appliquer_modificateurs(Fighter* original){
    Fighter copie = *original;
    
    if (copie.statutEffet == 3 && copie.dureeEffet > 0) {
        copie.defense += copie.defense * 0.25;  // +25%
    }
    switch (copie.statutEffet) {
        case 3: // Boost défense
            copie.defense += copie.defense * 0.25;
            SDL_Log("%s boost sa defense", copie.nom);

            break;
        case 4: // Boost attaque
            copie.attaque += copie.attaque * 0.25;
            SDL_Log("%s boost son attaque", copie.nom);

            break;
        case 5: // Boost vitesse
            copie.vitesse += copie.vitesse * 0.25;
            SDL_Log("%s boost sa vitesse", copie.nom);

            break;
        case 6: // Nerf défense
            copie.defense -= copie.defense * 0.25;
            SDL_Log("La defense de %s diminue", copie.nom);

            break;
        case 7: // Nerf attaque
            copie.attaque -= copie.attaque * 0.25;
            SDL_Log("L'attaque de %s diminue", copie.nom);
            break;
        case 8: // Nerf vitesse
            copie.vitesse -= copie.vitesse * 0.25;
            SDL_Log("La vitesse de %s diminue", copie.nom);
            break;
        case 9: // Nerf agilité
            copie.agilite -= copie.agilite * 0.25;
            SDL_Log("L'agilité de %s diminue", copie.nom);
            break;
        case 10: // Boost agilité
            copie.agilite += copie.agilite * 0.25;
            SDL_Log("%s boost son agilité", copie.nom);
            break;
        case 11: // Gel : nerf général léger
            copie.attaque  -= copie.attaque  * 0.1;
            copie.defense  -= copie.defense  * 0.1;
            copie.vitesse  -= copie.vitesse  * 0.1;
            copie.agilite  -= copie.agilite  * 0.1;
            SDL_Log("%s est Gelé", copie.nom);
            break;
        case 13: // Defense classique
            copie.defense += copie.defense * 0.1;
    }


    return *original;
}


void appliquer_et_mettre_a_jour_effets(Fighter* perso) {
    if (perso->dureeEffet <= 0 || perso->statutEffet == 0) return;

    switch (perso->statutEffet) {
        case 1:{    //Saignement : -10% PV max
            int perte = perso->max_pv * 0.1;
            perso->actu_pv -= perte;
            if (perso->actu_pv < 0) perso->actu_pv = 0;
            SDL_Log("%s perds du sang (-%d PV)", perso->nom, perte);
            break;
        }
        case 2: { // Brûlure : -5% PV max
            int perte = perso->max_pv * 0.05;
            perso->actu_pv -= perte;
            if (perso->actu_pv < 0) perso->actu_pv = 0;
            SDL_Log("%s est brûlé (-%d PV)", perso->nom, perte);
            break;
        }
        case 12: { // Paralysie : ne peut pas agir
            SDL_Log("%s est paralysé et ne peut pas agir ce tour", perso->nom);
            break;
        }
        // Les autres effets (boost/nerf) ne font rien ici : ils seront appliqués dans actionPerso ou combat
    }

    perso->dureeEffet--;

    if (perso->dureeEffet <= 0) {
        SDL_Log("L'effet %d de %s a pris fin.", perso->statutEffet, perso->nom);
        perso->statutEffet = 0;
    }
}


    int idAttaque;
    int utilisateurNum; 
    int utilisateurEquipe;
    
    int cibleNum; 
    int cibleEquipe;



AttaqueSauvegarde choisirCible(SDL_Renderer* rendu, int equipeCible, AttaqueSauvegarde attaque) {
       
    bool choisi = false;
    SDL_Event event;
    int mx, my;

    SDL_Texture* arrowTexture = IMG_LoadTexture(rendu, "ressource/image/utilité/cibleSelect.png");
    if (!arrowTexture) {
        SDL_Log("Erreur chargement flèche : %s", SDL_GetError());
    }

    Fighter* cibles[3];
    (void)cibles;
    int x_start, direction;

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
        x_start = LARGEUR_FENETRE - 100 - 100;
        direction = -1;
    } else {
        SDL_Log("Erreur : équipe cible inconnue (%d)", equipeCible);
        return attaque;
    }

    while (!choisi) {
        SDL_GetMouseState(&mx, &my);
        renduJeu(rendu); // affiche les persos

        // surbrillance avec flèche
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
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < 3; i++) {
                    int x = x_start + direction * i * (100 + 30);
                    int y = (HAUTEUR_FENETRE - 100) / 2 + i * 30 + ecartementPont;
                    SDL_Rect zone = {x, y, 100, 100};

                    if (mx >= zone.x && mx <= zone.x + zone.w &&
                        my >= zone.y && my <= zone.y + zone.h) {
                        attaque.cibleEquipe = equipeCible;
                        attaque.cibleNum = i;
                        if(equipeCible == 2){
                            attaque.cibleNum += 3;
                        }
                        choisi = true;
                    }
                }
            }
        }

        SDL_Delay(8);
    }

    if (arrowTexture) SDL_DestroyTexture(arrowTexture);
    return attaque;
}







void drawButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font) {
    SDL_Color color = btn->hovered ? btn->hoverColor : btn->baseColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &btn->rect);

    if (!font) {
        SDL_Log("drawButton : police NULL !");
        return;
    }

    // Crée un font réduit si le texte dépasse le bouton
    int fontSize = 40;  // Taille de la police par défaut
    TTF_Font* adjustedFont = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
    if (!adjustedFont) {
        SDL_Log("Erreur ouverture police : %s", TTF_GetError());
        return;
    }

    SDL_Surface* surf = TTF_RenderUTF8_Blended(adjustedFont, btn->text, (SDL_Color){255, 255, 255, 255});

    // Ajuster la taille de la police si le texte dépasse
    while (surf && surf->w > btn->rect.w) {
        SDL_FreeSurface(surf);  // Libère la surface précédente
        fontSize--;
        TTF_CloseFont(adjustedFont); // Fermeture de la police précédente
        adjustedFont = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
        if (!adjustedFont) {
            SDL_Log("Erreur ouverture police : %s", TTF_GetError());
            return;
        }
        surf = TTF_RenderUTF8_Blended(adjustedFont, btn->text, (SDL_Color){255, 255, 255, 255});
    }

    if (!surf) {
        SDL_Log("Erreur rendu texte dans drawButton : %s", TTF_GetError());
        TTF_CloseFont(adjustedFont);  // Libération de la police en cas d'erreur
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        SDL_Log("Erreur création texture bouton : %s", SDL_GetError());
        SDL_FreeSurface(surf);  // Libération de la surface si la texture échoue
        TTF_CloseFont(adjustedFont);  // Libération de la police
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

    // Libération de la mémoire après le rendu
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    TTF_CloseFont(adjustedFont);  // Libération finale de la police
}





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


bool isMouseOver(Button* btn, int x, int y) {
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}



bool est_une_attaque_de_soin(int id) {
    return id == ATQ_GLACE_CURATIVE ||
           id == ATQ_VAGUE_GUERISSEUSE ||
           id == ATQ_SOUFFLE_DE_VIE;
}

bool est_un_boost_de_def(int id){
    return id == ATQ_ESPRIT_FLAMBOYANT ||
           id == ATQ_BARRIERE_DE_PIERRE ||
           id == ATQ_RUGISSEMENT_D_ACIER ||
           id == ATQ_BRUME_PROTECTRICE;
}

bool est_un_boost_att(int id){
    return id == ATQ_EVEIL_SABRE ||
           id == ATQ_ESPRIT_FLAMBOYANT;
}

bool est_mur_vivant(int id) {
    return id == ATQ_MUR_VIVANT;
}

bool attaque_cible_soi_meme(int idAttaque) {
    return idAttaque == AFFUTAGE_MORTAL ||        // Zoro
           idAttaque == EVEIL_DU_SABRE ||         // Zoro
           idAttaque == EVEIL_LUNAIRE ||          // Katara - change le cycle → pas de cible
           idAttaque == CREPUSCULE ||             // Dark Shadow - change le cycle → pas de cible
           idAttaque == FLAMMES_SOLAIRES ||       // Rengoku - change le cycle → pas de cible
           idAttaque == BARRIERE_DE_PIERRE ||     // Incassable - boost défense de soi-même
           idAttaque == MUR_VIVANT ||             // Incassable - protège un allié mais effet passif
           idAttaque == HURLEMENT_NOIR ||         // Dark Shadow - AOE
           idAttaque == VENT_PERÇANT ||           // Temari - AOE
           idAttaque == FOUDRE_ENCHAINEE ||       // Kirua - AOE
           idAttaque == BLIZZARD;                 // Itsugaya - AOE
}


void actionPerso(SDL_Renderer* renderer, Fighter* persoActuel, int equipeAdverse) {
    if (!policePrincipale || !policePetite) {
        SDL_Log("Police non chargée !");
        return;
    }

    Button btnAttaque = {{20, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Attaque"};
    Button btnDefense = {{40 + btnAttaque.rect.w, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Defense"};
    Button btnComp1 = {{screenWidth - (btnAttaque.rect.w + 20), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq1.nom};
    Button btnComp2 = {{screenWidth - (btnAttaque.rect.w * 2 + 40), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq2.nom};
    Button btnComp3 = {{screenWidth - (btnAttaque.rect.w * 3 + 60), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq3.nom};

    bool quit = false;
    SDL_Event event;

    // === Coûts des compétences ===
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

        // Hover
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

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
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

                } else if (btnDefense.hovered) {
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

                } else if (btnComp1.hovered && persoActuel->pt >= persoActuel->spe_atq1.cout) {
                    persoActuel->pt -= persoActuel->spe_atq1.cout;

                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq1.id,
                        .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                        .utilisateurNum = id,
                    };

                    // Détermine si c'est une attaque de soutien (soin ou boost def)
                    int equipeCible = (est_une_attaque_de_soin(attaque->idAttaque) || est_un_boost_de_def(attaque->idAttaque))
                                    ? ((equipeAdverse == 1) ? 2 : 1)
                                    : equipeAdverse;

                    // Si l'attaque cible soi-même uniquement
                    if (attaque_cible_soi_meme(attaque->idAttaque)) {
                        attaque->cibleEquipe = attaque->utilisateurEquipe;
                        attaque->cibleNum = attaque->utilisateurNum;
                    } else {
                        *attaque = choisirCible(renderer, equipeCible, *attaque);
                    }

                    quit = true;


                } else if (btnComp2.hovered && persoActuel->pt >= persoActuel->spe_atq2.cout) {
                    persoActuel->pt -= persoActuel->spe_atq2.cout;

                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq2.id,
                        .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                        .utilisateurNum = id,
                    };

                    // Détermine si c'est une attaque de soutien (soin ou boost def)
                    int equipeCible = (est_une_attaque_de_soin(attaque->idAttaque) || est_un_boost_de_def(attaque->idAttaque))
                                    ? ((equipeAdverse == 1) ? 2 : 1)
                                    : equipeAdverse;

                    // Si l'attaque cible soi-même uniquement
                    if (attaque_cible_soi_meme(attaque->idAttaque)) {
                        attaque->cibleEquipe = attaque->utilisateurEquipe;
                        attaque->cibleNum = attaque->utilisateurNum;
                    } else {
                        *attaque = choisirCible(renderer, equipeCible, *attaque);
                    }

                    quit = true;


                } else if (btnComp3.hovered && persoActuel->pt >= persoActuel->spe_atq3.cout) {
                    persoActuel->pt -= persoActuel->spe_atq3.cout;

                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq3.id,
                        .utilisateurEquipe = (equipeAdverse == 1) ? 2 : 1,
                        .utilisateurNum = id,
                    };

                    // Détermine si c'est une attaque de soutien (soin ou boost def)
                    int equipeCible = (est_une_attaque_de_soin(attaque->idAttaque) || est_un_boost_de_def(attaque->idAttaque))
                                    ? ((equipeAdverse == 1) ? 2 : 1)
                                    : equipeAdverse;

                    // Si l'attaque cible soi-même uniquement
                    if (attaque_cible_soi_meme(attaque->idAttaque)) {
                        attaque->cibleEquipe = attaque->utilisateurEquipe;
                        attaque->cibleNum = attaque->utilisateurNum;
                    } else {
                        *attaque = choisirCible(renderer, equipeCible, *attaque);
                    }

                    quit = true;


                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_p:  // touche "p" → donne 10 PT
                        persoActuel->pt = 10;
                        SDL_Log("[DEBUG] %s a maintenant 10 PT.", persoActuel->nom);
                        break;
                    case SDLK_m:  // touche "m" → vie à fond
                        persoActuel->actu_pv = persoActuel->max_pv;
                        SDL_Log("[DEBUG] %s récupère toute sa vie.", persoActuel->nom);
                        break;
                    case SDLK_DELETE:  // touche "Suppr" → vie à 0
                        persoActuel->actu_pv = 0;
                        SDL_Log("[DEBUG] %s est mis KO (PV = 0).", persoActuel->nom);
                        quit = true;
                        break;
                }
            }
            
        }

        SDL_Delay(16);
    }
    SDL_DestroyTexture(t1); SDL_FreeSurface(s1);
    SDL_DestroyTexture(t2); SDL_FreeSurface(s2);
    SDL_DestroyTexture(t3); SDL_FreeSurface(s3);
}























void runGame(SDL_Renderer* rendu) {
    arreter_musique("ressource/musique/ogg/selection_personnages.ogg");
    SDL_GetWindowSize(fenetre, &screenWidth, &screenHeight);
    
    partieActuelle.mapType = rand() % 9;
    
    
    partieActuelle.joueur1.fighter1 = persoChoisi[0];
    if(partieActuelle.joueur1.fighter1.element == partieActuelle.mapType){ 
        partieActuelle.joueur1.fighter1.max_pv += 20;
        partieActuelle.joueur1.fighter1.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur1.fighter1.nom);
    } 
    partieActuelle.joueur1.fighter2 = persoChoisi[2];
    if(partieActuelle.joueur1.fighter2.element == partieActuelle.mapType){
        partieActuelle.joueur1.fighter2.max_pv += 20;
        partieActuelle.joueur1.fighter2.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur1.fighter2.nom);
    }

    partieActuelle.joueur1.fighter3 = persoChoisi[4];
    if(partieActuelle.joueur1.fighter3.element == partieActuelle.mapType){ 
        partieActuelle.joueur1.fighter3.max_pv += 20;
        partieActuelle.joueur1.fighter3.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur1.fighter3.nom);
    }

    partieActuelle.joueur2.fighter1 = persoChoisi[1];
    if(partieActuelle.joueur2.fighter1.element == partieActuelle.mapType){
        partieActuelle.joueur2.fighter1.max_pv += 20;
        partieActuelle.joueur2.fighter1.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur2.fighter1.nom);
    }


    partieActuelle.joueur2.fighter2 = persoChoisi[3];
    if(partieActuelle.joueur2.fighter2.element == partieActuelle.mapType){ 
        partieActuelle.joueur2.fighter2.max_pv += 20;
        partieActuelle.joueur2.fighter2.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur2.fighter2.nom);
    }

    partieActuelle.joueur2.fighter3 = persoChoisi[5];
    if(partieActuelle.joueur2.fighter3.element == partieActuelle.mapType){
        partieActuelle.joueur2.fighter3.max_pv += 20;
        partieActuelle.joueur2.fighter3.actu_pv += 20;
        SDL_Log("%s recoit un boost d'élement !",partieActuelle.joueur2.fighter3.nom);
    }

    

    for (int i = 0; i < 6; i++) {
        if (strcmp(persoChoisi[i].nom, "incassable") == 0) {
            if (strcmp(persoChoisi[i].nom, partieActuelle.joueur1.fighter1.nom) == 0) idIncassble = 0;
            else if (strcmp(persoChoisi[i].nom, partieActuelle.joueur1.fighter2.nom) == 0) idIncassble = 1;
            else if (strcmp(persoChoisi[i].nom, partieActuelle.joueur1.fighter3.nom) == 0) idIncassble = 2;
            else if (strcmp(persoChoisi[i].nom, partieActuelle.joueur2.fighter1.nom) == 0) idIncassble = 3;
            else if (strcmp(persoChoisi[i].nom, partieActuelle.joueur2.fighter2.nom) == 0) idIncassble = 4;
            else if (strcmp(persoChoisi[i].nom, partieActuelle.joueur2.fighter3.nom) == 0) idIncassble = 5;
            break;
        }
    }
    
    dureeMur = 0;

    dureeMur = 0;
    partieActuelle.perso_actif = 0;
    partieActuelle.tour = 1;
    partieActuelle.equipeQuiCommence = rand() % 2 + 1;
    partieActuelle.fin = false;
    
    

    char musiquePath[128];
    snprintf(musiquePath, sizeof(musiquePath), "ressource/musique/ogg/jeu/combat_%d.ogg", partieActuelle.mapType);
    jouerMusique(musiquePath, 20);

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
        default: break;
    }

    while (!partieActuelle.fin) {
        for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
            tableauAttaqueDuTour[i] = (AttaqueSauvegarde){ .idAttaque = -1 };
        }

        renduJeu(rendu);
        animationNouveauTour(rendu, partieActuelle.tour);
        int equipeDebut = (partieActuelle.tour % 2 == 0) ? partieActuelle.equipeQuiCommence : 3 - partieActuelle.equipeQuiCommence;

        for (int e = 0; e < 2; e++) {
            int equipe = (e == 0) ? equipeDebut : 3 - equipeDebut;
            for (int i = 0; i < 3; i++) {
                int index = (equipe == 1) ? i : i + 3;
                Fighter* perso = get_fighter(index);
                if (perso->actu_pv <= 0) continue;
                appliquer_et_mettre_a_jour_effets(perso);
                partieActuelle.perso_actif = index;


                //IA Facile


                if (equipe == 2 && partieActuelle.iaDifficulte > 0) {
                    int choix = rand() % 100;
                    AttaqueSauvegarde* action = &tableauAttaqueDuTour[index];

                    if (choix <= 10 && perso->pt >= perso->spe_atq1.cout){
                        perso->pt -= perso->spe_atq1.cout;
                        *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq1.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                    } else if (choix <= 20 && perso->pt >= perso->spe_atq2.cout) {
                        perso->pt -= perso->spe_atq2.cout;
                        *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq2.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                    } else if (choix <= 30 && perso->pt >= perso->spe_atq3.cout) {
                        perso->pt -= perso->spe_atq3.cout;
                        *action = (AttaqueSauvegarde){.idAttaque = perso->spe_atq3.id, .utilisateurEquipe = 2, .utilisateurNum = index};
                    } else if (choix <= 65) {
                        *action = (AttaqueSauvegarde){.idAttaque = DEFENSE, .utilisateurEquipe = 2, .utilisateurNum = index, .cibleEquipe = -1, .cibleNum = -1};
                        if (perso->pt < 10) perso->pt += 2;
                        SDL_Log("%s Choisi de ce defendre", perso->nom);
                    } else {
                        *action = (AttaqueSauvegarde){.idAttaque = ATTAQUE_BASIQUE, .utilisateurEquipe = 2, .utilisateurNum = index};
                        if (perso->pt < 10) perso->pt += 1;
                    }

                    if (action->idAttaque != DEFENSE) {
                        int vivants[3], n = 0;
                        for (int j = 0; j < 3; j++) {
                            Fighter* f = get_fighter(j); // J1 est 0,1,2
                            if (f->actu_pv > 0) vivants[n++] = j;
                        }
                        if (n > 0) {
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

        partieActuelle.perso_actif = 0;

        bool actionTrouvee = false;
        for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
            if (tableauAttaqueDuTour[i].idAttaque >= 0) {
                actionTrouvee = true;
                break;
            }
        }

        if (!actionTrouvee) continue;

        int tabIdVitesse[6] = {0, 1, 2, 3, 4, 5};
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 6; j++) {
                Fighter modI = appliquer_modificateurs(&persoChoisi[tabIdVitesse[i]]);
                Fighter modJ = appliquer_modificateurs(&persoChoisi[tabIdVitesse[j]]);
                if (modJ.vitesse > modI.vitesse) {
                    int tmp = tabIdVitesse[i];
                    tabIdVitesse[i] = tabIdVitesse[j];
                    tabIdVitesse[j] = tmp;
                }
            }
        }

        for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
            int index = tabIdVitesse[i];
            AttaqueSauvegarde action = tableauAttaqueDuTour[index];
            Fighter* utilisateur = get_fighter(action.utilisateurNum);
            Fighter* cible = get_fighter(action.cibleNum);

            if (action.idAttaque >= 0 && action.idAttaque < NB_ATTAQUES_TOTAL) {
                if (!toutes_les_attaques[action.idAttaque]) continue;
                AttaqueSpecial* atq = toutes_les_attaques[action.idAttaque];
                SDL_Rect rectUtilisateur = get_rect_fighter(utilisateur);
                SDL_Rect rectCible = get_rect_fighter(cible);
                jouerAnimationAttaque(rendu, atq->type, rectUtilisateur, rectCible, utilisateur->element);
                renduJeu(rendu);
                if (cible && cible->pt < 10) cible->pt += 1;
                fonctions_attaques[action.idAttaque](utilisateur, cible);
            }
        }

        if (equipe_est_morte(1)) {
            SDL_Log("L'équipe 1 est éliminée. L'équipe 2 gagne !");
            partieActuelle.fin = true;
        } else if (equipe_est_morte(2)) {
            SDL_Log("L'équipe 2 est éliminée. L'équipe 1 gagne !");
            partieActuelle.fin = true;
        }

        for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
            Fighter* f = get_fighter(i);
            if (f->dureeEffet > 0) {
                f->dureeEffet--;
                if (f->dureeEffet == 0 && f->statutEffet == 3) {
                    f->statutEffet = 0;
                    SDL_Log("%s perd son bonus de défense.", f->nom);
                }
            }
            persoChoisi[i].protegePar = -1;
        }

        if (dureeMur > 0) dureeMur--;
        partieActuelle.tour++;
    }

    SDL_Log("\n\n_______Fin du jeu ! Merci d'avoir joué._______\n\n");
    exit(0);
}

