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

int screenWidth = 0;
int screenHeight = 0;


// ============================ AFFICHAGE =========================================

Fighter* get_fighter(int equipe, int numero) {
    if (equipe == 1) {
        if (numero == 0) return &partieActuelle.joueur1.fighter1;
        if (numero == 1) return &partieActuelle.joueur1.fighter2;
        if (numero == 2) return &partieActuelle.joueur1.fighter3;
    } else {
        if (numero == 0) return &partieActuelle.joueur2.fighter1;
        if (numero == 1) return &partieActuelle.joueur2.fighter2;
        if (numero == 2) return &partieActuelle.joueur2.fighter3;
    }
    return NULL;
}

void renduJeu(SDL_Renderer* rendu) {
    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 32);
    if (!font) {
        SDL_Log("Erreur police: %s", TTF_GetError());
        return;
    }
    SDL_Texture* cadreTexte = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");
    if (!cadreTexte) {
        SDL_Log("Erreur chargement cadre texte : %s", SDL_GetError());
    }

    // === Affichage du fond ===
    char fondPath[128];
    snprintf(fondPath, sizeof(fondPath), "ressource/image/fonds/maps/map_%d.png", partieActuelle.mapType);
    SDL_Texture* fond_texture = IMG_LoadTexture(rendu, fondPath);
    if (!fond_texture) {
        SDL_Log("Erreur chargement fond de map : %s", SDL_GetError());
    } else {
        SDL_RenderCopy(rendu, fond_texture, NULL, NULL); // plein écran
    }

    // === Portrait et infos personnage actif ===
    int equipe = (partieActuelle.perso_actif < 3) ? 1 : 2;
    int numero = (equipe == 1) ? partieActuelle.perso_actif : partieActuelle.perso_actif - 3;
    Fighter* actif = get_fighter(equipe, numero);


    // Chargement du portrait stylisé
    char pathPortrait[128];
    snprintf(pathPortrait, sizeof(pathPortrait), "ressource/image/personnages_pp/pp_%s/pp_%s.png",
        (equipe == 1 ? "droite" : "gauche"), actif->nom);
    SDL_Texture* texPortrait = IMG_LoadTexture(rendu, pathPortrait);
    int portraitX = (equipe == 1) ? 30 : LARGEUR_FENETRE - 330;
    int portraitY = 30;
    SDL_Rect destPortrait = {portraitX, portraitY, 200, 200};

    if (texPortrait) {
        SDL_RenderCopy(rendu, texPortrait, NULL, &destPortrait);
        SDL_DestroyTexture(texPortrait);
    }

    // === Texte du tour ===
    char texteTour[64];
    snprintf(texteTour, sizeof(texteTour), "Tour %d", partieActuelle.tour);
    SDL_Surface* surfTour = TTF_RenderUTF8_Blended(font, texteTour, (SDL_Color){255, 255, 255, 255});
    SDL_Texture* txtTour = SDL_CreateTextureFromSurface(rendu, surfTour);
    SDL_Rect rectTour = {LARGEUR_FENETRE/2 - surfTour->w/2, 20, surfTour->w, surfTour->h};
    SDL_RenderCopy(rendu, txtTour, NULL, &rectTour);
    SDL_FreeSurface(surfTour);
    SDL_DestroyTexture(txtTour);

    // === Texte joueur actif ===
    const char* joueurTexte = (equipe == 1) ? "Joueur 1" : "Joueur 2";
    SDL_Surface* surfJoueur = TTF_RenderUTF8_Blended(font, joueurTexte, (SDL_Color){255, 200, 0, 255});
    SDL_Texture* txtJoueur = SDL_CreateTextureFromSurface(rendu, surfJoueur);
    SDL_Rect rectJoueur = {LARGEUR_FENETRE/2 - surfJoueur->w/2, 60, surfJoueur->w, surfJoueur->h};
    SDL_RenderCopy(rendu, txtJoueur, NULL, &rectJoueur);
    SDL_FreeSurface(surfJoueur);
    SDL_DestroyTexture(txtJoueur);


    const int largeur_perso = 100;
    const int hauteur_perso = 100;
    const int espacement = 30;
    const int y_perso = (HAUTEUR_FENETRE - hauteur_perso) / 2;

    Fighter* perso1[3] = {
        &partieActuelle.joueur1.fighter1,
        &partieActuelle.joueur1.fighter2,
        &partieActuelle.joueur1.fighter3
    };

    Fighter* perso2[3] = {
        &partieActuelle.joueur2.fighter1,
        &partieActuelle.joueur2.fighter2,
        &partieActuelle.joueur2.fighter3
    };

    // === ÉQUIPE 1 ===
    for (int i = 0; i < 3; i++) {
        int x = 100 + i * (largeur_perso + espacement);
        int y = y_perso + i * 30 + ecartementPont;
        SDL_Rect dest = {x, y, largeur_perso, hauteur_perso};
    
        char path[128];
        snprintf(path, sizeof(path), "ressource/image/personnages_pixel/%s.png", perso1[i]->nom);
        SDL_Texture* tex = IMG_LoadTexture(rendu, path);
        if (tex) SDL_RenderCopy(rendu, tex, NULL, &dest);
    
        // === Texte PV
        char infosPV[64];
        snprintf(infosPV, sizeof(infosPV), "PV: %d/%d", perso1[i]->actu_pv, perso1[i]->max_pv);
        SDL_Surface* surfPV = TTF_RenderUTF8_Blended(font, infosPV, (SDL_Color){255, 255, 255, 255});
        SDL_Texture* txtPV = SDL_CreateTextureFromSurface(rendu, surfPV);
        SDL_Rect txtRectPV = {dest.x + (largeur_perso - surfPV->w) / 2, dest.y - surfPV->h - 10, surfPV->w, surfPV->h};
    
        // Cadre autour du texte PV
        if (cadreTexte) {
            SDL_Rect cadreRect = {txtRectPV.x - 15, txtRectPV.y - 15, txtRectPV.w + 30, txtRectPV.h + 30};
            SDL_RenderCopy(rendu, cadreTexte, NULL, &cadreRect);
        }
        SDL_RenderCopy(rendu, txtPV, NULL, &txtRectPV);
        SDL_FreeSurface(surfPV);
        SDL_DestroyTexture(txtPV);
    
        // === Texte PT
        char infosPT[64];
        snprintf(infosPT, sizeof(infosPT), "PT: %d", perso1[i]->pt);
        SDL_Surface* surfPT = TTF_RenderUTF8_Blended(font, infosPT, (SDL_Color){255, 255, 0, 255});
        SDL_Texture* txtPT = SDL_CreateTextureFromSurface(rendu, surfPT);
        SDL_Rect txtRectPT = {dest.x + (largeur_perso - surfPT->w) / 2, dest.y + hauteur_perso + 5, surfPT->w, surfPT->h};
    
        // Cadre autour du texte PT
        if (cadreTexte) {
            SDL_Rect cadreRect = {txtRectPT.x - 15, txtRectPT.y - 15, txtRectPT.w + 30, txtRectPT.h + 30};
            SDL_RenderCopy(rendu, cadreTexte, NULL, &cadreRect);
        }
        SDL_RenderCopy(rendu, txtPT, NULL, &txtRectPT);
        SDL_FreeSurface(surfPT);
        SDL_DestroyTexture(txtPT);
    
        if (tex) SDL_DestroyTexture(tex);
    }    

    // === ÉQUIPE 2 ===
    for (int i = 0; i < 3; i++) {
        int x = LARGEUR_FENETRE - 100 - largeur_perso - i * (largeur_perso + espacement);
        int y = y_perso + i * 30 + ecartementPont;
        SDL_Rect dest = {x, y, largeur_perso, hauteur_perso};
    
        char path[128];
        snprintf(path, sizeof(path), "ressource/image/personnages_pixel/%s_reverse.png", perso2[i]->nom);
        SDL_Texture* tex = IMG_LoadTexture(rendu, path);
        if (tex) SDL_RenderCopy(rendu, tex, NULL, &dest);
    
        // === Texte PV
        char infosPV[64];
        snprintf(infosPV, sizeof(infosPV), "PV: %d/%d", perso2[i]->actu_pv, perso2[i]->max_pv);
        SDL_Surface* surfPV = TTF_RenderUTF8_Blended(font, infosPV, (SDL_Color){255, 255, 255, 255});
        SDL_Texture* txtPV = SDL_CreateTextureFromSurface(rendu, surfPV);
        SDL_Rect txtRectPV = {dest.x + (largeur_perso - surfPV->w) / 2, dest.y - surfPV->h - 10, surfPV->w, surfPV->h};
    
        if (cadreTexte) {
            SDL_Rect cadreRect = {txtRectPV.x - 15, txtRectPV.y - 15, txtRectPV.w + 30, txtRectPV.h + 30};
            SDL_RenderCopy(rendu, cadreTexte, NULL, &cadreRect);
        }
        SDL_RenderCopy(rendu, txtPV, NULL, &txtRectPV);
        SDL_FreeSurface(surfPV);
        SDL_DestroyTexture(txtPV);
    
        // === Texte PT
        char infosPT[64];
        snprintf(infosPT, sizeof(infosPT), "PT: %d", perso2[i]->pt);
        SDL_Surface* surfPT = TTF_RenderUTF8_Blended(font, infosPT, (SDL_Color){255, 255, 0, 255});
        SDL_Texture* txtPT = SDL_CreateTextureFromSurface(rendu, surfPT);
        SDL_Rect txtRectPT = {dest.x + (largeur_perso - surfPT->w) / 2, dest.y + hauteur_perso + 5, surfPT->w, surfPT->h};
    
        if (cadreTexte) {
            SDL_Rect cadreRect = {txtRectPT.x - 15, txtRectPT.y - 15, txtRectPT.w + 30, txtRectPT.h + 30};
            SDL_RenderCopy(rendu, cadreTexte, NULL, &cadreRect);
        }
        SDL_RenderCopy(rendu, txtPT, NULL, &txtRectPT);
        SDL_FreeSurface(surfPT);
        SDL_DestroyTexture(txtPT);
    
        if (tex) SDL_DestroyTexture(tex);

    }    

    // Libération du fond
    if (fond_texture) SDL_DestroyTexture(fond_texture);

}

void animationNouveauTour(SDL_Renderer* renderer, int numeroTour) {
    // Charger police
    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 64);
    if (!font) {
        SDL_Log("Erreur chargement police: %s", TTF_GetError());
        return;
    }

    // Texte principal
    char texte[64];
    snprintf(texte, sizeof(texte), "Tour %d", numeroTour);
    SDL_Color couleur = {255, 255, 255, 255};

    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, texte, couleur);
    SDL_Texture* textureTexte = SDL_CreateTextureFromSurface(renderer, surf);

    int texW = surf->w;
    int texH = surf->h;
    SDL_FreeSurface(surf);

    SDL_Rect posTexte = {
        (LARGEUR_FENETRE - texW) / 2,
        (HAUTEUR_FENETRE - texH) / 2,
        texW,
        texH
    };

    Uint32 start = SDL_GetTicks();
    const Uint32 duree = 2000;

    // Boucle de l’animation
    while (SDL_GetTicks() - start < duree) {
        float ratio = (float)(SDL_GetTicks() - start) / duree;
        Uint8 alpha = (Uint8)(255 * (1 - fabs(2 * ratio - 1)));  // fondu entrant puis sortant

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderClear(renderer);

        // Rendu de fond noir semi-transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect fond = {0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE};
        SDL_RenderFillRect(renderer, &fond);

        // Rendu texte
        SDL_SetTextureAlphaMod(textureTexte, alpha);
        SDL_RenderCopy(renderer, textureTexte, NULL, &posTexte);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60 FPS
    }

    SDL_DestroyTexture(textureTexte);
    TTF_CloseFont(font);
}

SDL_Rect get_rect_fighter(Fighter* fighter) {
    const int largeur = 100;
    const int hauteur = 100;
    const int espacement = 30;
    const int y_perso = (HAUTEUR_FENETRE - hauteur) / 2;

    for (int i = 0; i < 3; i++) {
        if (fighter == &partieActuelle.joueur1.fighter1 + i) {
            int x = 100 + i * (largeur + espacement);
            int y = y_perso + i * 30 + ecartementPont;
            return (SDL_Rect){x, y, largeur, hauteur};
        }
        if (fighter == &partieActuelle.joueur2.fighter1 + i) {
            int x = LARGEUR_FENETRE - 100 - largeur - i * (largeur + espacement);
            int y = y_perso + i * 30 + ecartementPont;
            return (SDL_Rect){x, y, largeur, hauteur};
        }
    }

    // Par défaut (au cas où)
    return (SDL_Rect){0, 0, largeur, hauteur};
}

void jouerAnimationAttaque(SDL_Renderer* renderer, int type, SDL_Rect lanceur, SDL_Rect cible, ElementType element) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    switch (type) {
        case 1: // Attaque physique
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100 + i * 50);
                SDL_RenderFillRect(renderer, &cible);
                SDL_RenderPresent(renderer);
                SDL_Delay(100);
            }
            break;
        case 2: // Défense
            for (int i = 0; i < 2; i++) {
                SDL_SetRenderDrawColor(renderer, 0, 100, 255, 150 - i * 50);
                SDL_RenderDrawRect(renderer, &lanceur);
                SDL_RenderPresent(renderer);
                SDL_Delay(150);
            }
            break;
        case 3: // AOE (Zone)
            for (int frame = 0; frame < 3; frame++) {
                for (int i = 0; i < 3; i++) {
                    SDL_Rect aoeCible = get_rect_fighter(get_fighter((lanceur.x < cible.x) ? 2 : 1, i));
                    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 80 + frame * 50);
                    SDL_RenderFillRect(renderer, &aoeCible);
                }
                SDL_RenderPresent(renderer);
                SDL_Delay(120);
            }
            break;
        case 4: // Soin
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100 + i * 50);
                SDL_RenderFillRect(renderer, &cible);
                SDL_RenderPresent(renderer);
                SDL_Delay(100);
            }
            break;
        case 5: case 6: { // Buff / Debuff
            SDL_Color color = (type == 5) ? (SDL_Color){255, 255, 0, 150} : (SDL_Color){150, 0, 150, 150};
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 150 - i * 30);
                SDL_RenderDrawRect(renderer, &cible);
                SDL_RenderPresent(renderer);
                SDL_Delay(120);
            }
            break;
        }
        case 7: // Changement jour
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 80 + i * 40); // Jaune pâle
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
                SDL_Delay(120);
            }
            break;
        case 8: // Changement nuit
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, 30, 30, 60, 80 + i * 40); // Bleu nuit
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
                SDL_Delay(120);
            }
            break;
        case 9: // Attaque magique
            for (int i = 0; i < 3; i++) {
                SDL_SetRenderDrawColor(renderer, 100, 0, 255, 100 + i * 50);
                SDL_RenderFillRect(renderer, &cible);
                SDL_RenderPresent(renderer);
                SDL_Delay(100);
            }
            break;
        default:
            SDL_Log("Erreur : animation type %d inconnue", type);
            break;
    }

    SDL_Delay(300);
}
