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

AttaqueSauvegarde tableauAttaqueDuTour [NB_PERSOS_EQUIPE * 2];

// ====================== GAMEPLAY ====================

Fighter appliquer_modificateurs(Fighter* original){
    Fighter copie = *original;

    switch (copie.statutEffet) {
        case 3: // Boost défense
            copie.defense += copie.defense * 0.25;
            break;
        case 4: // Boost attaque
            copie.attaque += copie.attaque * 0.25;
            break;
        case 5: // Boost vitesse
            copie.vitesse += copie.vitesse * 0.25;
            break;
        case 6: // Nerf défense
            copie.defense -= copie.defense * 0.25;
            break;
        case 7: // Nerf attaque
            copie.attaque -= copie.attaque * 0.25;
            break;
        case 8: // Nerf vitesse
            copie.vitesse -= copie.vitesse * 0.25;
            break;
        case 9: // Nerf agilité
            copie.agilite -= copie.agilite * 0.25;
            break;
        case 10: // Boost agilité
            copie.agilite += copie.agilite * 0.25;
            break;
        case 11: // Gel : nerf général léger
            copie.attaque  -= copie.attaque  * 0.1;
            copie.defense  -= copie.defense  * 0.1;
            copie.vitesse  -= copie.vitesse  * 0.1;
            copie.agilite  -= copie.agilite  * 0.1;
            break;
    }

    return copie;
}


void appliquer_et_mettre_a_jour_effets(Fighter* perso) {
    if (perso->dureeEffet <= 0 || perso->statutEffet == 0) return;

    switch (perso->statutEffet) {
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


AttaqueSauvegarde choisirCible(SDL_Renderer* rendu, int equipeCible, AttaqueSauvegarde attaque) {
    bool choisi = false;
    SDL_Event event;
    int mx, my;

    SDL_Texture* arrowTexture = IMG_LoadTexture(rendu, "ressource/image/utilité/cibleSelect.png");
    if (!arrowTexture) {
        SDL_Log("Erreur chargement flèche : %s", SDL_GetError());
    }

    Fighter* cibles[3];
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
                        SDL_Log("Cible sélectionnée : perso %d de l'équipe %d", i, equipeCible);
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

    int fontSize = 32;
    TTF_Font* f = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
    if (!f) {
        SDL_Log("Erreur chargement police : %s", TTF_GetError());
        return;
    }

    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, btn->text, (SDL_Color){255, 255, 255, 255});
    while (surf && surf->w > btn->rect.w - 10 && fontSize > 10) {
        SDL_FreeSurface(surf);
        TTF_CloseFont(f);
        fontSize -= 2;
        f = TTF_OpenFont("ressource/langue/police/arial.ttf", fontSize);
        if (!f) {
            SDL_Log("Erreur police taille %d : %s", fontSize, TTF_GetError());
            return;
        }
        surf = TTF_RenderUTF8_Blended(f, btn->text, (SDL_Color){255, 255, 255, 255});
    }

    if (!surf) {
        SDL_Log("Erreur rendu texte : %s", TTF_GetError());
        TTF_CloseFont(f);
        return;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
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
    TTF_CloseFont(f);
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

int get_equipe_id(int index) {
    // Joueur 1 : index pair, Joueur 2 : index impair
    return (index % 2 == 0) ? 1 : 2;
}

int get_fighter_num(int index) {
    // Chaque joueur a les index 0/2/4 (J1) et 1/3/5 (J2)
    return index / 2; // 0 pour 0/1, 1 pour 2/3, 2 pour 4/5
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






void actionPerso(SDL_Renderer* renderer, Fighter* persoActuel, int equipeAdverse) {
    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", TTF_GetError());
        return;
    }

    Button btnAttaque = {{20, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Attaque"};
    Button btnDefense = {{40 + btnAttaque.rect.w, screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, "Defense"};
    Button btnComp1 = {{screenWidth - (btnAttaque.rect.w + 20), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq1.nom};
    Button btnComp2 = {{screenWidth - (btnAttaque.rect.w * 2 + 40), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq2.nom};
    Button btnComp3 = {{screenWidth - (btnAttaque.rect.w * 3 + 60), screenHeight - 100, 150, 60}, {60, 60, 60, 255}, {120, 120, 120, 255}, false, persoActuel->spe_atq3.nom};

    bool quit = false;
    SDL_Event event;

    while (!quit) {
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

        drawButton(renderer, &btnAttaque, font);
        drawButton(renderer, &btnDefense, font);
        drawButton(renderer, &btnComp1, font);
        drawButton(renderer, &btnComp2, font);
        drawButton(renderer, &btnComp3, font);

        // === Coûts des compétences ===
        TTF_Font* costFont = TTF_OpenFont("ressource/langue/police/arial.ttf", 22);
        if (costFont) {
            SDL_Color jaune = {255, 255, 0, 255};
            char cout1[16], cout2[16], cout3[16];
            snprintf(cout1, sizeof(cout1), "%d PT", persoActuel->spe_atq1.cout);
            snprintf(cout2, sizeof(cout2), "%d PT", persoActuel->spe_atq2.cout);
            snprintf(cout3, sizeof(cout3), "%d PT", persoActuel->spe_atq3.cout);

            SDL_Surface* s1 = TTF_RenderUTF8_Blended(costFont, cout1, jaune);
            SDL_Surface* s2 = TTF_RenderUTF8_Blended(costFont, cout2, jaune);
            SDL_Surface* s3 = TTF_RenderUTF8_Blended(costFont, cout3, jaune);

            SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
            SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);
            SDL_Texture* t3 = SDL_CreateTextureFromSurface(renderer, s3);

            SDL_Rect r1 = {btnComp1.rect.x + (btnComp1.rect.w - s1->w)/2, btnComp1.rect.y - s1->h - 5, s1->w, s1->h};
            SDL_Rect r2 = {btnComp2.rect.x + (btnComp2.rect.w - s2->w)/2, btnComp2.rect.y - s2->h - 5, s2->w, s2->h};
            SDL_Rect r3 = {btnComp3.rect.x + (btnComp3.rect.w - s3->w)/2, btnComp3.rect.y - s3->h - 5, s3->w, s3->h};

            SDL_RenderCopy(renderer, t1, NULL, &r1);
            SDL_RenderCopy(renderer, t2, NULL, &r2);
            SDL_RenderCopy(renderer, t3, NULL, &r3);

            SDL_DestroyTexture(t1); SDL_FreeSurface(s1);
            SDL_DestroyTexture(t2); SDL_FreeSurface(s2);
            SDL_DestroyTexture(t3); SDL_FreeSurface(s3);
            TTF_CloseFont(costFont);
        }


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
                        .utilisateurEquipe = get_equipe_id(id),
                        .utilisateurNum = get_fighter_num(id),
                    };
                    *attaque = choisirCible(renderer, equipeAdverse, *attaque);
                    if (persoActuel->pt < 10) persoActuel->pt += 1;
                    quit = true;
        
                } else if (btnDefense.hovered) {
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = DEFENSE,
                        .utilisateurEquipe = get_equipe_id(id),
                        .utilisateurNum = get_fighter_num(id),
                        .cibleEquipe = -1,
                        .cibleNum = -1
                    };
                    if (persoActuel->pt < 10) persoActuel->pt += 1;
                    if (persoActuel->pt < 10) persoActuel->pt += 1;
                    quit = true;
        
                } else if (btnComp1.hovered && persoActuel->pt >= persoActuel->spe_atq1.cout) {
                    persoActuel->pt -= persoActuel->spe_atq1.cout;
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq1.id,
                        .utilisateurEquipe = get_equipe_id(id),
                        .utilisateurNum = get_fighter_num(id),
                    };
                    int equipeCible = est_une_attaque_de_soin(attaque->idAttaque) ?
                                      get_equipe_id(id) : equipeAdverse;
                    *attaque = choisirCible(renderer, equipeCible, *attaque);
                    quit = true;
        
                } else if (btnComp2.hovered && persoActuel->pt >= persoActuel->spe_atq2.cout) {
                    persoActuel->pt -= persoActuel->spe_atq2.cout;
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq2.id,
                        .utilisateurEquipe = get_equipe_id(id),
                        .utilisateurNum = get_fighter_num(id),
                    };
                    int equipeCible = est_une_attaque_de_soin(attaque->idAttaque) ?
                                      get_equipe_id(id) : equipeAdverse;
                    *attaque = choisirCible(renderer, equipeCible, *attaque);
                    quit = true;
        
                } else if (btnComp3.hovered && persoActuel->pt >= persoActuel->spe_atq3.cout) {
                    persoActuel->pt -= persoActuel->spe_atq3.cout;
                    *attaque = (AttaqueSauvegarde){
                        .idAttaque = persoActuel->spe_atq3.id,
                        .utilisateurEquipe = get_equipe_id(id),
                        .utilisateurNum = get_fighter_num(id),
                    };
                    int equipeCible = est_une_attaque_de_soin(attaque->idAttaque) ?
                                      get_equipe_id(id) : equipeAdverse;
                    *attaque = choisirCible(renderer, equipeCible, *attaque);
                    quit = true;
                }
            }
        }
        

        SDL_Delay(8);
    }
}



bool equipe_est_morte(int equipe) {
    for (int i = 0; i < 3; i++) {
        Fighter* p = get_fighter(equipe, i);
        if (p->actu_pv > 0) return false;
    }
    return true;
}



























void runGame(SDL_Renderer* rendu) {
    arreter_musique("ressource/musique/ogg/selection_personnages.ogg");
    
    SDL_GetWindowSize(fenetre, &screenWidth, &screenHeight);

    partieActuelle.joueur1.fighter1 = persoChoisi[0];
    partieActuelle.joueur1.fighter2 = persoChoisi[2];
    partieActuelle.joueur1.fighter3 = persoChoisi[4];

    partieActuelle.joueur2.fighter1 = persoChoisi[1];
    partieActuelle.joueur2.fighter2 = persoChoisi[3];
    partieActuelle.joueur2.fighter3 = persoChoisi[5];

    partieActuelle.perso_actif = 0;
    partieActuelle.tour = 1;
    partieActuelle.equipeQuiCommence = rand() % 2 + 1;
    partieActuelle.fin = false;
    partieActuelle.mapType = 9;

    partieActuelle.mapType = rand()%9;
    char musiquePath[128];
    snprintf(musiquePath, sizeof(musiquePath), "ressource/musique/ogg/jeu/combat_%d.ogg", partieActuelle.mapType);

    switch (partieActuelle.mapType){
    case 0:     ecartementPont = -25;    break;
    case 1:     ecartementPont = 20;    break;
    case 2:     ecartementPont = -25;    break;
    case 3:     ecartementPont = -10;    break;
    case 4:     ecartementPont = -20;    break;
    case 5:     ecartementPont = 10;    break;
    case 6:     ecartementPont = -20;    break;
    case 7:     ecartementPont = 70;    break;
    case 8:     ecartementPont = -50;    break;    
    default:
        break;
    }
    jouerMusique(musiquePath, 20);
    for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
        tableauAttaqueDuTour[i] = (AttaqueSauvegarde){ .idAttaque = -1 };
    }

    while (!partieActuelle.fin) {
        renduJeu(rendu);
        SDL_Log("==================================== Tour %d =======================================", partieActuelle.tour);
        animationNouveauTour(rendu, partieActuelle.tour);

        int equipeDebut = (partieActuelle.tour % 2 == 0) ? partieActuelle.equipeQuiCommence : 3 - partieActuelle.equipeQuiCommence;



        // Tour de la première équipe
        for (int i = 0; i < 3; i++) {
            Fighter* perso = get_fighter(equipeDebut, i);
            if (perso->actu_pv <= 0) continue;

            appliquer_et_mettre_a_jour_effets(perso);
            partieActuelle.perso_actif = (equipeDebut == 1) ? i : i + 3;
            actionPerso(rendu, perso, (equipeDebut == 1) ? 2 : 1);
        }

        // Tour de la seconde équipe
        for (int i = 0; i < 3; i++) {
            Fighter* perso = get_fighter(3 - equipeDebut, i);
            if (perso->actu_pv <= 0) continue;

            appliquer_et_mettre_a_jour_effets(perso);
            partieActuelle.perso_actif = (equipeDebut == 1) ? i + 3 : i;
            actionPerso(rendu, perso, (equipeDebut == 1) ? 1 : 2);
        }

        partieActuelle.perso_actif = 0;

        // Tri des attaques par vitesse 
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
            Fighter* utilisateur = get_fighter(action.utilisateurEquipe, action.utilisateurNum);
            Fighter* cible = get_fighter(action.cibleEquipe, action.cibleNum);
        
            if (action.idAttaque >= 0 && action.idAttaque < NB_ATTAQUES_TOTAL) {
                if (!toutes_les_attaques[action.idAttaque]) {
                    SDL_Log("Erreur : attaque id %d n'est pas initialisée dans toutes_les_attaques", action.idAttaque);
                    continue;
                }
        
                AttaqueSpecial* atq = toutes_les_attaques[action.idAttaque];            
                SDL_Rect rectUtilisateur = get_rect_fighter(utilisateur);
                SDL_Rect rectCible = get_rect_fighter(cible);
                jouerAnimationAttaque(rendu, atq->type, rectUtilisateur, rectCible, utilisateur->element);
                renduJeu(rendu);

                if (cible != NULL) {
                    if (cible->pt < 10) {
                        cible->pt += 1;
                    }
                }
        
                
        
                fonctions_attaques[action.idAttaque](utilisateur, cible);
            } else {
                SDL_Log("Erreur : attaque id %d invalide ou cible/utilisateur manquant", action.idAttaque);
            }
        }        

        if (equipe_est_morte(1)) {
            SDL_Log("L'équipe 1 est éliminée. L'équipe 2 gagne !");
            partieActuelle.fin = true;
        } else if (equipe_est_morte(2)) {
            SDL_Log("L'équipe 2 est éliminée. L'équipe 1 gagne !");
            partieActuelle.fin = true;
        }

        partieActuelle.tour++;
    }

    SDL_Log("\n\n_______Fin du jeu ! Merci d'avoir joué._______\n\n");
    exit(0);
}
