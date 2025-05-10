#include "data.h"
#include "interface.h"
#include "son.h"
#include "attaque.h"


#include <string.h>
#include <time.h>
#include <stdbool.h>



extern Page afficher_fiche_personnage(SDL_Renderer* rendu, Fighter perso, int joueur);


Fighter persoChoisi[6];
int index_selection[6];

void cleanup(void);
void runGame(SDL_Renderer* rendu);



Page afficher_selection_perso(SDL_Renderer* rendu, SDL_Texture* selections_j1[3], SDL_Texture* selections_j2[3]) {
	
	arreter_musique("ressource/musique/ogg/menu/menu_1.ogg");	
	jouerMusique("ressource/musique/ogg/selection_personnages.ogg", 40);
    init_attaques();

    // --- CHARGEMENT DES TEXTURES ---
    SDL_Texture* fond_texture = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_selection_perso.png");
    if (!fond_texture) {
        SDL_Log("Erreur chargement fond: %s", SDL_GetError());
        return PAGE_QUITTER;
    }

    // --- LOGO VERSUS ---
    SDL_Texture* logo_versus_texture = IMG_LoadTexture(rendu, "ressource/image/utilité/versus.png");
    if (!logo_versus_texture) {
        SDL_Log("Erreur chargement logo: %s", SDL_GetError());
        SDL_DestroyTexture(fond_texture);
        return PAGE_QUITTER;
    }

    SDL_Rect dest_logo = {
        (LARGEUR_FENETRE - 200) / 2,
        10,
        200,
        200
    };

    // --- BOUTON RETOUR ---
    SDL_Texture* btn_retour_texture = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");
    SDL_Rect btn_retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80}; // Nom de variable corrigé

    // --- PORTRAITS DES personnages ---
    const char* noms_portraits[8] = {
        "ressource/image/personnages_pp/pp_droite/pp_darkshadow.png",
        "ressource/image/personnages_pp/pp_droite/pp_hitsugaya.png",
        "ressource/image/personnages_pp/pp_droite/pp_incassable.png",
        "ressource/image/personnages_pp/pp_droite/pp_katara.png",
        "ressource/image/personnages_pp/pp_droite/pp_kirua.png",
        "ressource/image/personnages_pp/pp_droite/pp_rengoku.png",
        "ressource/image/personnages_pp/pp_droite/pp_temari.png",
        "ressource/image/personnages_pp/pp_droite/pp_zoro.png"
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
    TTF_Font* police = TTF_OpenFont("ressource/langue/police/arial.ttf", 24);
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

    //SDL_Color couleur_blanc = {213, 38, 35, 255};
    SDL_Surface* surface_tour = NULL;
    SDL_Texture* texture_tour = NULL;

    // --- BOUCLE PRINCIPALE ---
    bool running = true;
    int a = 0;
    
    while (running && (nb_selections_j1 < 3 || nb_selections_j2 < 3)) {
        // Mise à jour texte tour avec contour et gras
        if (surface_tour) SDL_FreeSurface(surface_tour);
        if (texture_tour) SDL_DestroyTexture(texture_tour);
    
        const char* texte_tour = tour_j1 ? "Tour du Joueur 1" : "Tour du Joueur 2";
        SDL_Color couleur_texte = tour_j1 ? (SDL_Color){213, 38, 35, 255} : (SDL_Color){25, 118, 210, 255};
    
        // Mettre en gras
        TTF_SetFontStyle(police, TTF_STYLE_BOLD);
    
        // Création texte coloré
        TTF_SetFontOutline(police, 0);
        surface_tour = TTF_RenderText_Blended(police, texte_tour, couleur_texte);
        texture_tour = SDL_CreateTextureFromSurface(rendu, surface_tour);
    
        SDL_Rect rect_tour = {
            (LARGEUR_FENETRE - surface_tour->w) / 2,
            marge_haut + mini_size + 80,
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
        	    jouerMusique("ressource/musique/ogg/menu/menu_1.ogg", 20);
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
                            
                            
                            jouer_effet("ressource/musique/ogg/persoClique.ogg", 40);  // ← AJOUT ICI

                            if (tour_j1 && nb_selections_j1 < 3) {
                                selections_j1[nb_selections_j1] = portraits[i];
                                nb_selections_j1++;

                            } else if (!tour_j1 && nb_selections_j2 < 3) {
                                selections_j2[nb_selections_j2] = portraits[i];
                                nb_selections_j2++;

                            }                                
                            perso_disponible[i] = false;
                            persoChoisi[a].pt= 0;
                            switch(i){

                                case 0:
                                    strcpy(persoChoisi[a].nom,"darkshadow");
                                    persoChoisi[a].actu_pv=darkshadow.actu_pv;
                                    persoChoisi[a].max_pv=darkshadow.max_pv;
                                    persoChoisi[a].attaque=darkshadow.attaque;
                                    persoChoisi[a].defense=darkshadow.defense;
                                    persoChoisi[a].agilite=darkshadow.agilite;
                                    persoChoisi[a].vitesse=darkshadow.vitesse;
                                    persoChoisi[a].element=darkshadow.element;
                                    persoChoisi[a].spe_atq1 = crepuscule;
                                    persoChoisi[a].spe_atq2 = hurlementNoir;
                                    persoChoisi[a].spe_atq3 = brumeProtectrice;
                                    index_selection[a] = i;
                                    break;
                               
                                case 1:
                                
                                    strcpy(persoChoisi[a].nom,"hitsugaya");
                                    persoChoisi[a].actu_pv=hitsugaya.actu_pv;
                                    persoChoisi[a].max_pv=hitsugaya.max_pv;
                                    persoChoisi[a].attaque=hitsugaya.attaque;
                                    persoChoisi[a].defense=hitsugaya.defense;
                                    persoChoisi[a].agilite=hitsugaya.agilite;
                                    persoChoisi[a].vitesse=hitsugaya.vitesse;
                                    persoChoisi[a].element=hitsugaya.element;
                                    persoChoisi[a].spe_atq1 = prisonDeGivre;
                                    persoChoisi[a].spe_atq2 = blizzard;
                                    persoChoisi[a].spe_atq3 = glaceCurative;
                                    index_selection[a] = i;
                                    break;
                                case 2:
                                
                                    strcpy(persoChoisi[a].nom,"incassable");
                                    persoChoisi[a].actu_pv=incassable.actu_pv;
                                    persoChoisi[a].max_pv=incassable.max_pv;
                                    persoChoisi[a].attaque=incassable.attaque;
                                    persoChoisi[a].defense=incassable.defense;
                                    persoChoisi[a].agilite=incassable.agilite;
                                    persoChoisi[a].vitesse=incassable.vitesse;
                                    persoChoisi[a].element=incassable.element;
                                    persoChoisi[a].spe_atq1 = murVivant;
                                    persoChoisi[a].spe_atq2 = barriereDePierre;
                                    persoChoisi[a].spe_atq3 = rugissementDacier;
                                    index_selection[a] = i;
                                    break;
                                
                                case 3:
                                    
                                    strcpy(persoChoisi[a].nom,"katara");
                                    persoChoisi[a].actu_pv=katara.actu_pv;
                                    persoChoisi[a].max_pv=katara.max_pv;
                                    persoChoisi[a].attaque=katara.attaque;
                                    persoChoisi[a].defense=katara.defense;
                                    persoChoisi[a].agilite=katara.agilite;
                                    persoChoisi[a].vitesse=katara.vitesse;
                                    persoChoisi[a].element=katara.element;
                                    persoChoisi[a].spe_atq1 = lienDeSang;
                                    persoChoisi[a].spe_atq2 = vagueGuerisseuse;
                                    persoChoisi[a].spe_atq3 = eveilLunaire;
                                    index_selection[a] = i;
                                    break;
                                
                                case 4:
                                
                                    strcpy(persoChoisi[a].nom,"kirua");
                                    persoChoisi[a].actu_pv=kirua.actu_pv;
                                    persoChoisi[a].max_pv=kirua.max_pv;
                                    persoChoisi[a].attaque=kirua.attaque;
                                    persoChoisi[a].defense=kirua.defense;
                                    persoChoisi[a].agilite=kirua.agilite;
                                    persoChoisi[a].vitesse=kirua.vitesse;
                                    persoChoisi[a].element=kirua.element;
                                    persoChoisi[a].spe_atq1 = fulgurance;
                                    persoChoisi[a].spe_atq2 = foudreEnchainee;
                                    persoChoisi[a].spe_atq3 = executionRapide;
                                    index_selection[a] = i;
                                    break;
                                    
                                case 5:
                                
                                    strcpy(persoChoisi[a].nom,"rengoku");
                                    persoChoisi[a].actu_pv=rengoku.actu_pv;
                                    persoChoisi[a].max_pv=rengoku.max_pv;
                                    persoChoisi[a].attaque=rengoku.attaque;
                                    persoChoisi[a].defense=rengoku.defense;
                                    persoChoisi[a].agilite=rengoku.agilite;
                                    persoChoisi[a].vitesse=rengoku.vitesse;
                                    persoChoisi[a].element=rengoku.element;
                                    persoChoisi[a].spe_atq1 = flammesSolaires;
                                    persoChoisi[a].spe_atq2 = explosionArdente;
                                    persoChoisi[a].spe_atq3 = espritFlamboyant;
                                    index_selection[a] = i;
                                    break;
                                    
                                case 6:
                                    
                                    strcpy(persoChoisi[a].nom,"temari");
                                    persoChoisi[a].actu_pv=temari.actu_pv;
                                    persoChoisi[a].max_pv=temari.max_pv;
                                    persoChoisi[a].attaque=temari.attaque;
                                    persoChoisi[a].defense=temari.defense;
                                    persoChoisi[a].agilite=temari.agilite;
                                    persoChoisi[a].vitesse=temari.vitesse;
                                    persoChoisi[a].element=temari.element;
                                    persoChoisi[a].spe_atq1 = danseDuVent;
                                    persoChoisi[a].spe_atq2 = ventPercant;
                                    persoChoisi[a].spe_atq3 = souffleDeVie;
                                    index_selection[a] = i;
                                    break;
                                    
                                case 7:
                                
                                    strcpy(persoChoisi[a].nom,"zoro");
                                    persoChoisi[a].actu_pv=zoro.actu_pv;
                                    persoChoisi[a].max_pv=zoro.max_pv;
                                    persoChoisi[a].attaque=zoro.attaque;
                                    persoChoisi[a].defense=zoro.defense;
                                    persoChoisi[a].agilite=zoro.agilite;
                                    persoChoisi[a].vitesse=zoro.vitesse;
                                    persoChoisi[a].element=zoro.element;
                                    persoChoisi[a].spe_atq1 = affutageMortal;
                                    persoChoisi[a].spe_atq2 = assautTranchant;
                                    persoChoisi[a].spe_atq3 = eveilDuSabre;
                                    index_selection[a] = i;
                                    break;

                                default :
                                    strcpy(persoChoisi[a].nom,"lukas");
                                    persoChoisi[a].actu_pv=lukas.actu_pv;
                                    persoChoisi[a].max_pv=lukas.max_pv;
                                    persoChoisi[a].attaque=lukas.attaque;
                                    persoChoisi[a].defense=lukas.defense;
                                    persoChoisi[a].agilite=lukas.agilite;
                                    persoChoisi[a].vitesse=lukas.vitesse;
                                    persoChoisi[a].element=lukas.element;
            
                                    break;
                            }
                            
                            //Sauvegarde des choix pour afficher la fiche
                            Page retour = afficher_fiche_personnage(rendu, persoChoisi[a], tour_j1 ? 1 : 2);
                            if (retour == PAGE_SELECTION_PERSO) {
                                // Annulation de la sélection
                                perso_disponible[i] = true;
                                if (tour_j1 && nb_selections_j1 > 0) {
                                    nb_selections_j1--;
                                    selections_j1[nb_selections_j1] = NULL;
                                } else if (!tour_j1 && nb_selections_j2 > 0) {
                                    nb_selections_j2--;
                                    selections_j2[nb_selections_j2] = NULL;
                                }
                                continue;
                            }

                            
                           

                            if(tour_j1) SDL_Log("Perso %d l'équipe 1 est : %s",a + 1, persoChoisi[a].nom);
                            else SDL_Log("Perso %d de l'équipe 2 est : %s",a + 1, persoChoisi[a].nom);
                            
                            index_selection[a] = i;
                            a = a + 1;
                            tour_j1 = !tour_j1;


                            //_____________________________________IA____________________________________//

                            if (partieActuelle.iaDifficulte > 0 && !tour_j1 && nb_selections_j2 < 3) {
                                int choix = -1;
                                do {
                                    choix = rand() % 8;
                                    switch(choix){

                                        case 0:
                                            strcpy(persoChoisi[a].nom,"darkshadow");
                                            persoChoisi[a].actu_pv=darkshadow.actu_pv;
                                            persoChoisi[a].max_pv=darkshadow.max_pv;
                                            persoChoisi[a].attaque=darkshadow.attaque;
                                            persoChoisi[a].defense=darkshadow.defense;
                                            persoChoisi[a].agilite=darkshadow.agilite;
                                            persoChoisi[a].vitesse=darkshadow.vitesse;
                                            persoChoisi[a].element=darkshadow.element;
                                            persoChoisi[a].spe_atq1 = crepuscule;
                                            persoChoisi[a].spe_atq2 = hurlementNoir;
                                            persoChoisi[a].spe_atq3 = brumeProtectrice;
                                            index_selection[a] = i;
                                            break;
                                       
                                        case 1:
                                        
                                            strcpy(persoChoisi[a].nom,"hitsugaya");
                                            persoChoisi[a].actu_pv=hitsugaya.actu_pv;
                                            persoChoisi[a].max_pv=hitsugaya.max_pv;
                                            persoChoisi[a].attaque=hitsugaya.attaque;
                                            persoChoisi[a].defense=hitsugaya.defense;
                                            persoChoisi[a].agilite=hitsugaya.agilite;
                                            persoChoisi[a].vitesse=hitsugaya.vitesse;
                                            persoChoisi[a].element=hitsugaya.element;
                                            persoChoisi[a].spe_atq1 = prisonDeGivre;
                                            persoChoisi[a].spe_atq2 = blizzard;
                                            persoChoisi[a].spe_atq3 = glaceCurative;
                                            index_selection[a] = i;
                                            break;
                                        case 2:
                                        
                                            strcpy(persoChoisi[a].nom,"incassable");
                                            persoChoisi[a].actu_pv=incassable.actu_pv;
                                            persoChoisi[a].max_pv=incassable.max_pv;
                                            persoChoisi[a].attaque=incassable.attaque;
                                            persoChoisi[a].defense=incassable.defense;
                                            persoChoisi[a].agilite=incassable.agilite;
                                            persoChoisi[a].vitesse=incassable.vitesse;
                                            persoChoisi[a].element=incassable.element;
                                            persoChoisi[a].spe_atq1 = murVivant;
                                            persoChoisi[a].spe_atq2 = barriereDePierre;
                                            persoChoisi[a].spe_atq3 = rugissementDacier;
                                            index_selection[a] = i;
                                            break;
                                        
                                        case 3:
                                            
                                            strcpy(persoChoisi[a].nom,"katara");
                                            persoChoisi[a].actu_pv=katara.actu_pv;
                                            persoChoisi[a].max_pv=katara.max_pv;
                                            persoChoisi[a].attaque=katara.attaque;
                                            persoChoisi[a].defense=katara.defense;
                                            persoChoisi[a].agilite=katara.agilite;
                                            persoChoisi[a].vitesse=katara.vitesse;
                                            persoChoisi[a].element=katara.element;
                                            persoChoisi[a].spe_atq1 = lienDeSang;
                                            persoChoisi[a].spe_atq2 = vagueGuerisseuse;
                                            persoChoisi[a].spe_atq3 = eveilLunaire;
                                            index_selection[a] = i;
                                            break;
                                        
                                        case 4:
                                        
                                            strcpy(persoChoisi[a].nom,"kirua");
                                            persoChoisi[a].actu_pv=kirua.actu_pv;
                                            persoChoisi[a].max_pv=kirua.max_pv;
                                            persoChoisi[a].attaque=kirua.attaque;
                                            persoChoisi[a].defense=kirua.defense;
                                            persoChoisi[a].agilite=kirua.agilite;
                                            persoChoisi[a].vitesse=kirua.vitesse;
                                            persoChoisi[a].element=kirua.element;
                                            persoChoisi[a].spe_atq1 = fulgurance;
                                            persoChoisi[a].spe_atq2 = foudreEnchainee;
                                            persoChoisi[a].spe_atq3 = executionRapide;
                                            index_selection[a] = i;
                                            break;
                                            
                                        case 5:
                                        
                                            strcpy(persoChoisi[a].nom,"rengoku");
                                            persoChoisi[a].actu_pv=rengoku.actu_pv;
                                            persoChoisi[a].max_pv=rengoku.max_pv;
                                            persoChoisi[a].attaque=rengoku.attaque;
                                            persoChoisi[a].defense=rengoku.defense;
                                            persoChoisi[a].agilite=rengoku.agilite;
                                            persoChoisi[a].vitesse=rengoku.vitesse;
                                            persoChoisi[a].element=rengoku.element;
                                            persoChoisi[a].spe_atq1 = flammesSolaires;
                                            persoChoisi[a].spe_atq2 = explosionArdente;
                                            persoChoisi[a].spe_atq3 = espritFlamboyant;
                                            index_selection[a] = i;
                                            break;
                                            
                                        case 6:
                                            
                                            strcpy(persoChoisi[a].nom,"temari");
                                            persoChoisi[a].actu_pv=temari.actu_pv;
                                            persoChoisi[a].max_pv=temari.max_pv;
                                            persoChoisi[a].attaque=temari.attaque;
                                            persoChoisi[a].defense=temari.defense;
                                            persoChoisi[a].agilite=temari.agilite;
                                            persoChoisi[a].vitesse=temari.vitesse;
                                            persoChoisi[a].element=temari.element;
                                            persoChoisi[a].spe_atq1 = danseDuVent;
                                            persoChoisi[a].spe_atq2 = ventPercant;
                                            persoChoisi[a].spe_atq3 = souffleDeVie;
                                            index_selection[a] = i;
                                            break;
                                            
                                        case 7:
                                        
                                            strcpy(persoChoisi[a].nom,"zoro");
                                            persoChoisi[a].actu_pv=zoro.actu_pv;
                                            persoChoisi[a].max_pv=zoro.max_pv;
                                            persoChoisi[a].attaque=zoro.attaque;
                                            persoChoisi[a].defense=zoro.defense;
                                            persoChoisi[a].agilite=zoro.agilite;
                                            persoChoisi[a].vitesse=zoro.vitesse;
                                            persoChoisi[a].element=zoro.element;
                                            persoChoisi[a].spe_atq1 = affutageMortal;
                                            persoChoisi[a].spe_atq2 = assautTranchant;
                                            persoChoisi[a].spe_atq3 = eveilDuSabre;
                                            index_selection[a] = i;
                                            break;
        
                                        default :
                                            SDL_Log("Crash ce n'est pas normal");
                                            strcpy(persoChoisi[a].nom,"lukas");
                                            persoChoisi[a].actu_pv=lukas.actu_pv;
                                            persoChoisi[a].max_pv=lukas.max_pv;
                                            persoChoisi[a].attaque=lukas.attaque;
                                            persoChoisi[a].defense=lukas.defense;
                                            persoChoisi[a].agilite=lukas.agilite;
                                            persoChoisi[a].vitesse=lukas.vitesse;
                                            persoChoisi[a].element=lukas.element;
                    
                                            break;
                                    }
                                } while (!perso_disponible[choix]);
                            
                                perso_disponible[choix] = false;
                            
                                selections_j2[nb_selections_j2] = portraits[choix];
                                nb_selections_j2++;
                            
                                a++;
                                tour_j1 = !tour_j1;
                            }                            
                            
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

        // personnages disponibles
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






















Page afficher_fiche_personnage(SDL_Renderer* rendu, Fighter perso, int joueur) {
    SDL_Texture* fond = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_selection_perso.png");
    SDL_Texture* cadre_nom = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_titres.png");
    SDL_Texture* bouton_retour = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");
    SDL_Texture* bouton_avancer = IMG_LoadTexture(rendu, "ressource/image/utilité/avance.png");

    char chemin[256];
    snprintf(chemin, sizeof(chemin), "ressource/image/personnages_pixel/%s.png", perso.nom);
    SDL_Texture* sprite_pixel = IMG_LoadTexture(rendu, chemin);

    TTF_Font* font = TTF_OpenFont("ressource/langue/police/arial.ttf", 28);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    SDL_Color noir = {0, 0, 0, 255};
    SDL_Color blanc = {255, 255, 255, 255};

    const char* phrases[] = {
        "zoro",        "« Rien... rien du tout. Je n'ai rien à perdre. »",
        "hitsugaya",   "« Je ne laisserai jamais la glace qui protège cette ville fondre. »",
        "darkshadow",  "« Embrasse les ténèbres... et laisse-les te guider. »",
        "rengoku",     "« Continue d'avancer. Même si tu pleures, protège les autres. »",
        "katara",      "« Je suis peut-être une fille... mais une maître de l'eau. »",
        "temari",      "« Seuls les faibles parlent de justice et de pitié. »",
        "kirua",       "« Si tu es fort tu survis. Si tu es faible, tu meurs. »",
        "incassable",  "« Tu vas voir ce que ça fait de se frotter à du diamant pur ! »"
    };

    const char* presentation = "« ??? »";
    for (int i = 0; i < sizeof(phrases)/sizeof(char*); i += 2) {
        if (strcmp(perso.nom, phrases[i]) == 0) {
            presentation = phrases[i + 1];
            break;
        }
    }

    SDL_Rect rect_sprite = {100, 100, 300, 400};
    SDL_Rect rect_nom = {LARGEUR_FENETRE / 2 - 150, 20, 300, 60};
    SDL_Rect rect_retour = {20, HAUTEUR_FENETRE - 100, 80, 80};
    SDL_Rect rect_avancer = {LARGEUR_FENETRE - 100, HAUTEUR_FENETRE - 100, 80, 80};

    SDL_Event e;
    while (1) {
        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond, NULL, NULL);
        SDL_RenderCopy(rendu, sprite_pixel, NULL, &rect_sprite);
        SDL_RenderCopy(rendu, cadre_nom, NULL, &rect_nom);

        // --- NOM ---
        TTF_SetFontOutline(font, 2);
        SDL_Surface* surf_nom_contour = TTF_RenderUTF8_Solid(font, perso.nom, blanc);
        SDL_Texture* tex_nom_contour = SDL_CreateTextureFromSurface(rendu, surf_nom_contour);
        SDL_Rect rect_nom_texte = {
            rect_nom.x + (rect_nom.w - surf_nom_contour->w)/2,
            rect_nom.y + (rect_nom.h - surf_nom_contour->h)/2,
            surf_nom_contour->w, surf_nom_contour->h
        };
        SDL_RenderCopy(rendu, tex_nom_contour, NULL, &rect_nom_texte);
        SDL_FreeSurface(surf_nom_contour);
        SDL_DestroyTexture(tex_nom_contour);

        TTF_SetFontOutline(font, 0);
        SDL_Surface* surf_nom = TTF_RenderUTF8_Solid(font, perso.nom, noir);
        SDL_Texture* tex_nom = SDL_CreateTextureFromSurface(rendu, surf_nom);
        SDL_RenderCopy(rendu, tex_nom, NULL, &rect_nom_texte);
        SDL_FreeSurface(surf_nom);
        SDL_DestroyTexture(tex_nom);

        // --- STATS ---
        const char* labels[] = {"PV", "Attaque", "Défense", "Agilité", "Vitesse", "Magie"};
        int valeurs[] = {perso.max_pv, perso.attaque, perso.defense, perso.agilite, perso.vitesse, perso.magie};
        for (int i = 0; i < 6; i++) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%s : %d", labels[i], valeurs[i]);

            SDL_Surface* s_contour = TTF_RenderUTF8_Solid(font, buffer, blanc);
            SDL_Texture* t_contour = SDL_CreateTextureFromSurface(rendu, s_contour);
            SDL_Rect r = {500, 120 + i * 50, s_contour->w, s_contour->h};
            SDL_RenderCopy(rendu, t_contour, NULL, &r);
            SDL_FreeSurface(s_contour);
            SDL_DestroyTexture(t_contour);

            SDL_Surface* s = TTF_RenderUTF8_Solid(font, buffer, noir);
            SDL_Texture* t = SDL_CreateTextureFromSurface(rendu, s);
            SDL_RenderCopy(rendu, t, NULL, &r);
            SDL_FreeSurface(s);
            SDL_DestroyTexture(t);
        }

        // --- CITATION ---
        TTF_SetFontOutline(font, 2);
        SDL_Surface* surf_citation_contour = TTF_RenderUTF8_Blended(font, presentation, blanc);
        SDL_Texture* tex_citation_contour = SDL_CreateTextureFromSurface(rendu, surf_citation_contour);
        SDL_Rect r_pres = {
            (LARGEUR_FENETRE - surf_citation_contour->w) / 2,
            HAUTEUR_FENETRE - 160,
            surf_citation_contour->w,
            surf_citation_contour->h
        };
        SDL_RenderCopy(rendu, tex_citation_contour, NULL, &r_pres);
        SDL_FreeSurface(surf_citation_contour);
        SDL_DestroyTexture(tex_citation_contour);

        TTF_SetFontOutline(font, 0);
        SDL_Surface* surf_citation = TTF_RenderUTF8_Blended(font, presentation, noir);
        SDL_Texture* tex_citation = SDL_CreateTextureFromSurface(rendu, surf_citation);
        SDL_RenderCopy(rendu, tex_citation, NULL, &r_pres);
        SDL_FreeSurface(surf_citation);
        SDL_DestroyTexture(tex_citation);

        // --- BOUTONS ---
        SDL_RenderCopy(rendu, bouton_retour, NULL, &rect_retour);
        SDL_RenderCopy(rendu, bouton_avancer, NULL, &rect_avancer);
        SDL_RenderPresent(rendu);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) goto nettoyage;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= rect_retour.x && x <= rect_retour.x + rect_retour.w &&
                    y >= rect_retour.y && y <= rect_retour.y + rect_retour.h) {
                    goto retour_selection;
                }
                if (x >= rect_avancer.x && x <= rect_avancer.x + rect_avancer.w &&
                    y >= rect_avancer.y && y <= rect_avancer.y + rect_avancer.h) {
                    goto retour_confirmation;
                }
            }
        }
    }

retour_selection:
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(cadre_nom);
    SDL_DestroyTexture(bouton_retour);
    SDL_DestroyTexture(bouton_avancer);
    SDL_DestroyTexture(sprite_pixel);
    TTF_CloseFont(font);
    return PAGE_SELECTION_PERSO;

retour_confirmation:
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(cadre_nom);
    SDL_DestroyTexture(bouton_retour);
    SDL_DestroyTexture(bouton_avancer);
    SDL_DestroyTexture(sprite_pixel);
    TTF_CloseFont(font);
    return PAGE_CONFIRMATION_PERSO;

nettoyage:
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(cadre_nom);
    SDL_DestroyTexture(bouton_retour);
    SDL_DestroyTexture(bouton_avancer);
    SDL_DestroyTexture(sprite_pixel);
    TTF_CloseFont(font);
    return PAGE_QUITTER;
}

















 // === LIBÉRATION DES TEXTURES COMMUNES ===
void cleanup(void) {
    extern SDL_Texture* textures_pixel[6];
    extern SDL_Texture* fond_texture;
    extern SDL_Texture* btn_avancer_texture;
    extern SDL_Texture* btn_retour_texture;
    extern SDL_Texture* vs_texture;
    extern SDL_Texture* cadre_texte;
    extern TTF_Font* font_titre;

    for (int i = 0; i < 6; i++) {
        if (textures_pixel[i]) {
            SDL_DestroyTexture(textures_pixel[i]);
            textures_pixel[i] = NULL;
        }
    }
    if (fond_texture) SDL_DestroyTexture(fond_texture), fond_texture = NULL;
    if (btn_avancer_texture) SDL_DestroyTexture(btn_avancer_texture), btn_avancer_texture = NULL;
    if (btn_retour_texture) SDL_DestroyTexture(btn_retour_texture), btn_retour_texture = NULL;
    if (vs_texture) SDL_DestroyTexture(vs_texture), vs_texture = NULL;
    if (cadre_texte) SDL_DestroyTexture(cadre_texte), cadre_texte = NULL;
    if (font_titre) TTF_CloseFont(font_titre), font_titre = NULL;
}



SDL_Texture* textures_pixel[6] = {NULL};
SDL_Texture* fond_texture = NULL;
SDL_Texture* btn_avancer_texture = NULL;
SDL_Texture* btn_retour_texture = NULL;
SDL_Texture* vs_texture = NULL;
SDL_Texture* cadre_texte = NULL;
TTF_Font* font_titre = NULL;




Page afficher_confirmation_perso(SDL_Renderer* rendu, SDL_Texture* equipe1[3], SDL_Texture* equipe2[3]) {
    
   
    
    SDL_Texture* fond_texture = IMG_LoadTexture(rendu, "ressource/image/fonds/fond_selection_perso.png");
    SDL_Texture* btn_avancer_texture = IMG_LoadTexture(rendu, "ressource/image/utilité/avance.png");
    SDL_Texture* btn_retour_texture = IMG_LoadTexture(rendu, "ressource/image/utilité/retour.png");
    SDL_Texture* vs_texture = IMG_LoadTexture(rendu, "ressource/image/utilité/versus_recap.png");
    SDL_Texture* cadre_texte = IMG_LoadTexture(rendu, "ressource/image/cadres/cadre_texte.png");

    SDL_Texture* textures_pixel[6] = {NULL};
    TTF_Font* font_titre = TTF_OpenFont("ressource/langue/police/arial.ttf", 36);
    SDL_Color blanc = {255, 255, 255, 255};

    // --- Vérification des chargements ---
    if (!fond_texture || !btn_avancer_texture || !btn_retour_texture || !vs_texture || !cadre_texte || !font_titre) goto quitter;

    for (int i = 0; i < 3; i++) {
        int idx_j1 = i * 2, idx_j2 = i * 2 + 1;

        if (idx_j1 < 6) {
            char chemin[256];
            snprintf(chemin, sizeof(chemin), "ressource/image/personnages_pixel/%s.png", persoChoisi[idx_j1].nom);
            textures_pixel[i] = IMG_LoadTexture(rendu, chemin);
        }
        if (idx_j2 < 6) {
            char chemin[256];
            snprintf(chemin, sizeof(chemin), "ressource/image/personnages_pixel/%s_reverse.png", persoChoisi[idx_j2].nom);
            textures_pixel[i + 3] = IMG_LoadTexture(rendu, chemin);
        }
    }

    const SDL_Rect btn_retour_rect = {20, HAUTEUR_FENETRE - 100, 80, 80};
    const SDL_Rect btn_avancer_rect = {LARGEUR_FENETRE - 100, HAUTEUR_FENETRE - 100, 80, 80};
    const SDL_Rect vs_rect = {(LARGEUR_FENETRE - 200) / 2, (HAUTEUR_FENETRE - 200) / 2, 200, 200};

    const int largeur_perso = 150, hauteur_perso = 150;
    const int marge_haut = 130, espacement = 30;
    const int start_x_j1 = 100, start_x_j2 = LARGEUR_FENETRE - 100 - largeur_perso;

    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) goto quitter;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Point mouse = {event.button.x, event.button.y};
                if (SDL_PointInRect(&mouse, &btn_avancer_rect)) {
                    cleanup();
                    runGame(rendu);
                    return PAGE_COMBAT;
                } else if (SDL_PointInRect(&mouse, &btn_retour_rect)) {
                    cleanup();
                    return PAGE_SELECTION_PERSO;
                }
            }
        }

        SDL_RenderClear(rendu);
        SDL_RenderCopy(rendu, fond_texture, NULL, NULL);

        for (int i = 0; i < 3; i++) {
            if (textures_pixel[i]) {
                SDL_Rect dest1 = {start_x_j1, marge_haut + i * (hauteur_perso + espacement), largeur_perso, hauteur_perso};
                SDL_RenderCopy(rendu, textures_pixel[i], NULL, &dest1);
            }
            if (textures_pixel[i + 3]) {
                SDL_Rect dest2 = {start_x_j2, marge_haut + i * (hauteur_perso + espacement), largeur_perso, hauteur_perso};
                SDL_RenderCopy(rendu, textures_pixel[i + 3], NULL, &dest2);
            }
        }

        // --- Titres équipes ---
        const char* label_gauche = "Équipe 1";
        const char* label_droite = "Équipe 2";
        SDL_Surface* surf_eq1 = TTF_RenderUTF8_Blended(font_titre, label_gauche, blanc);
        SDL_Surface* surf_eq2 = TTF_RenderUTF8_Blended(font_titre, label_droite, blanc);
        SDL_Texture* tex_eq1 = SDL_CreateTextureFromSurface(rendu, surf_eq1);
        SDL_Texture* tex_eq2 = SDL_CreateTextureFromSurface(rendu, surf_eq2);

        SDL_Rect cadre_eq1 = {start_x_j1 + largeur_perso / 2 - (surf_eq1->w + 40) / 2, marge_haut - 70, surf_eq1->w + 40, surf_eq1->h + 30};
        SDL_Rect cadre_eq2 = {start_x_j2 + largeur_perso / 2 - (surf_eq2->w + 40) / 2, marge_haut - 70, surf_eq2->w + 40, surf_eq2->h + 30};
        SDL_Rect texte_eq1 = {cadre_eq1.x + (cadre_eq1.w - surf_eq1->w) / 2, cadre_eq1.y + (cadre_eq1.h - surf_eq1->h) / 2, surf_eq1->w, surf_eq1->h};
        SDL_Rect texte_eq2 = {cadre_eq2.x + (cadre_eq2.w - surf_eq2->w) / 2, cadre_eq2.y + (cadre_eq2.h - surf_eq2->h) / 2, surf_eq2->w, surf_eq2->h};

        SDL_RenderCopy(rendu, cadre_texte, NULL, &cadre_eq1);
        SDL_RenderCopy(rendu, cadre_texte, NULL, &cadre_eq2);
        SDL_RenderCopy(rendu, tex_eq1, NULL, &texte_eq1);
        SDL_RenderCopy(rendu, tex_eq2, NULL, &texte_eq2);

        SDL_FreeSurface(surf_eq1); SDL_DestroyTexture(tex_eq1);
        SDL_FreeSurface(surf_eq2); SDL_DestroyTexture(tex_eq2);

        SDL_RenderCopy(rendu, vs_texture, NULL, &vs_rect);
        SDL_RenderCopy(rendu, btn_retour_texture, NULL, &btn_retour_rect);
        SDL_RenderCopy(rendu, btn_avancer_texture, NULL, &btn_avancer_rect);
        SDL_RenderPresent(rendu);
    }

quitter:
    cleanup();
    return PAGE_QUITTER;
}



