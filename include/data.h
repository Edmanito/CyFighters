#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// === Constantes générales ===
#define NB_PERSOS_EQUIPE 3
#define MAX_NOM_ATTAQUE 50
#define MAX_DESCRIPTION 300
#define MAX_SPECIAL 3
#define MAX_EFFETS 3
#define MAX_NOM_PERSO 50

// === Éléments ===
typedef enum {
    ELEMENT_NONE,
    ELEMENT_CRISTAL,
    ELEMENT_GLACE,
    ELEMENT_FEU,
    ELEMENT_ECLAIR,
    ELEMENT_VENT,
    ELEMENT_EAU,
    ELEMENT_OMBRE
} ElementType;

// === ID des attaques spéciales ===
typedef enum {
    ATTAQUE_BASIQUE = 0,
    DEFENSE,
    ATQ_AFFUTAGE_MORTAL,
    ATQ_ASSAUT_TRANCHANT,
    ATQ_EVEIL_SABRE,
    ATQ_FLAMMES_SOLAIRES,
    ATQ_EXPLOSION_ARDENTE,
    ATQ_ESPRIT_FLAMBOYANT,
    ATQ_PRISON_DE_GIVRE,
    ATQ_BLIZZARD,
    ATQ_GLACE_CURATIVE,
    ATQ_LIEN_DE_SANG,
    ATQ_VAGUE_GUERISSEUSE,
    ATQ_EVEIL_LUNAIRE,
    ATQ_CREPUSCULE,
    ATQ_HURLEMENT_NOIR,
    ATQ_BRUME_PROTECTRICE,
    ATQ_DANSE_DU_VENT,
    ATQ_VENT_PERCANT,
    ATQ_SOUFFLE_DE_VIE,
    ATQ_FULGURANCE,
    ATQ_FOUDRE_ENCHAINEE,
    ATQ_EXECUTION_RAPIDE,
    ATQ_MUR_VIVANT,
    ATQ_BARRIERE_DE_PIERRE,
    ATQ_RUGISSEMENT_D_ACIER,
    NB_ATTAQUES_TOTAL
} AttaqueID;

// === Structures principales ===
typedef struct {
    char nom[MAX_NOM_ATTAQUE];
    char description[MAX_DESCRIPTION];
    int id;
    int cout;
    int type;
} AttaqueSpecial;

typedef struct {
    char nom[MAX_NOM_PERSO];
    int actu_pv, max_pv;
    int attaque, defense, agilite, vitesse, magie;
    int pt;
    int statutEffet;        // 1 = Saignement, 2 = Brulur, 3 = Boost def, 4 = Boost attaque, 5 = Boost vitesse, 6 = Nerf def,
                            // 7 = Nerf attaque , 8 = Nerf vitesse, 9 = Nerf Agilité, 10 = Boost Agilité, 11 = Gel; 12 = paralysie
    int dureeEffet;         // durée en tours restants

    ElementType element;
    AttaqueSpecial spe_atq1, spe_atq2, spe_atq3;
} Fighter;

typedef struct {
    Fighter fighter1, fighter2, fighter3;
} Joueur;

typedef struct {
    Joueur joueur1, joueur2;
    int perso_actif;
    int tour;
    int equipeQuiCommence;
    bool fin;
    int mapType;
    bool nuit;
} Partie;

typedef struct {
    int bonus_attaque;
    int bonus_defense;
    int bonus_vitesse;
    int bonus_agilite;
    int bonus_pv;
} BonusMap;

typedef struct {
    int idAttaque;
    int utilisateurNum, utilisateurEquipe;
    int cibleNum, cibleEquipe;
} AttaqueSauvegarde;

typedef struct {
    SDL_Rect rect;
    SDL_Color baseColor, hoverColor;
    bool hovered;
    const char* text;
} Button;

// === Fonctions utilitaires ===
Fighter appliquer_modificateurs(Fighter* original);
Fighter* get_fighter(int equipe, int numero);
Fighter* get_fighter_by_index(int index);
int get_equipe_id(int index);
int get_fighter_num(int index);
Fighter creer_fighter(const char* nom, int actu_pv, int max_pv, int attaque, int defense, int agilite, int vitesse, ElementType element, AttaqueSpecial** attaques);
Fighter get_fighter_depuis_nom(int index);
void appliquer_buffs(Fighter* perso, BonusMap bonus);
bool equipe_est_morte(int equipe);
void runGame(SDL_Renderer* rendu);

// === Variables globales ===
extern SDL_Window* fenetre;
extern int screenWidth;
extern int screenHeight;
extern Partie partieActuelle;
extern Fighter persoChoisi[];
extern AttaqueSpecial* toutes_les_attaques[NB_ATTAQUES_TOTAL];
extern void (*fonctions_attaques[NB_ATTAQUES_TOTAL])(Fighter*, Fighter*);

// === Personnages disponibles ===
extern Fighter darkshadow, hitsugaya, incassable, katara, kirua;
extern Fighter rengoku, temari, zoro, lukas;
extern Joueur equipe1;

// === Attaques de test ===
extern AttaqueSpecial Test1, Test2, Test3;

#endif // DATA_H
