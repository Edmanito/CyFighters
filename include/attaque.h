
#ifndef ATTAQUE_H
#define ATTAQUE_H

#include <stdio.h>
#include <string.h>

#include "data.h" 

#define ATTAQUE_BASIQUE 0
#define DEFENSE 1
#define AFFUTAGE_MORTAL 2
#define ASSAUT_TRANCHANT 3
#define EVEIL_DU_SABRE 4
#define FLAMMES_SOLAIRES 5
#define EXPLOSION_ARDENTE 6
#define ESPRIT_FLAMBOYANT 7
#define PRISON_DE_GIVRE 8
#define BLIZZARD 9
#define GLACE_CURATIVE 10
#define LIEN_DE_SANG 11
#define VAGUE_GUERISSEUSE 12
#define EVEIL_LUNAIRE 13
#define CREPUSCULE 14
#define HURLEMENT_NOIR 15
#define BRUME_PROTECTRICE 16
#define DANSE_DU_VENT 17
#define VENT_PERÇANT 18
#define SOUFFLE_DE_VIE 19
#define FULGURANCE 20
#define FOUDRE_ENCHAINEE 21
#define EXECUTION_RAPIDE 22
#define MUR_VIVANT 23
#define BARRIERE_DE_PIERRE 24
#define RUGISSEMENT_D_ACIER 25
#define NB_ATTAQUES_TOTAL 26

void attaqueClassique(Fighter *attaquant, Fighter *cible);
void defense(Fighter* attaquant, Fighter* cible);
void init_attaques();

void attaque_affutage_mortal(Fighter*, Fighter*);
void attaque_assaut_tranchant(Fighter*, Fighter*);
void attaque_eveil_du_sabre(Fighter*, Fighter*);
void attaque_flammes_solaires(Fighter*, Fighter*);
void attaque_explosion_ardente(Fighter*, Fighter*);
void attaque_esprit_flamboyant(Fighter*, Fighter*);
void attaque_prison_de_givre(Fighter*, Fighter*);
void attaque_blizzard(Fighter*, Fighter*);
void attaque_glace_curative(Fighter*, Fighter*);
void attaque_lien_de_sang(Fighter*, Fighter*);
void attaque_vague_guerisseuse(Fighter*, Fighter*);
void attaque_eveil_lunaire(Fighter*, Fighter*);
void attaque_crepuscule(Fighter*, Fighter*);
void attaque_hurlement_noir(Fighter*, Fighter*);
void attaque_brume_protectrice(Fighter*, Fighter*);
void attaque_danse_du_vent(Fighter*, Fighter*);
void attaque_vent_percant(Fighter*, Fighter*);
void attaque_souffle_de_vie(Fighter*, Fighter*);
void attaque_fulgurance(Fighter*, Fighter*);
void attaque_foudre_enchainee(Fighter*, Fighter*);
void attaque_execution_rapide(Fighter*, Fighter*);
void attaque_mur_vivant(Fighter*, Fighter*);
void attaque_barriere_de_pierre(Fighter*, Fighter*);
void attaque_rugissement_d_acier(Fighter*, Fighter*);

extern AttaqueSpecial useAttaque;
extern AttaqueSpecial useDefense;
// Déclarations externes des attaques spéciales
extern AttaqueSpecial affutageMortal;
extern AttaqueSpecial assautTranchant;
extern AttaqueSpecial eveilDuSabre;
extern AttaqueSpecial flammesSolaires;
extern AttaqueSpecial explosionArdente;
extern AttaqueSpecial espritFlamboyant;
extern AttaqueSpecial prisonDeGivre;
extern AttaqueSpecial blizzard;
extern AttaqueSpecial glaceCurative;
extern AttaqueSpecial lienDeSang;
extern AttaqueSpecial vagueGuerisseuse;
extern AttaqueSpecial eveilLunaire;
extern AttaqueSpecial crepuscule;
extern AttaqueSpecial hurlementNoir;
extern AttaqueSpecial brumeProtectrice;
extern AttaqueSpecial danseDuVent;
extern AttaqueSpecial ventPercant;
extern AttaqueSpecial souffleDeVie;
extern AttaqueSpecial fulgurance;
extern AttaqueSpecial foudreEnchainee;
extern AttaqueSpecial executionRapide;
extern AttaqueSpecial murVivant;
extern AttaqueSpecial barriereDePierre;
extern AttaqueSpecial rugissementDacier;

#endif