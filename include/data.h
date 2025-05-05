// Include/data.h
#ifndef DATA_H
#define DATA_H

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>




extern SDL_Window* fenetre;

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

// ==== Définition des éléments ====
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

// ==== Bonus de map ====
typedef struct {
    int bonus_attaque;
    int bonus_defense;
    int bonus_vitesse;
    int bonus_agilite;
    int bonus_pv;
} BonusMap;

// ==== Statuts d'effet ====
typedef enum {
    Aucun = 0,
    BRULE,
    PARALYSIE,
    GEL,
    SAIGNEMENT,
    BOOST_DEF,
    BOOST_PV,
    BOOST_VIT
} StatutEffet;

// ==== Attaques spéciales ====
#define MAX_NOM_ATTAQUE 50
#define MAX_DESCRIPTION 300
#define MAX_SPECIAL 3
#define MAX_EFFETS 3

typedef struct {
    char nom[MAX_NOM_ATTAQUE];
    char description[MAX_DESCRIPTION];
    int id;
    int cout;
} AttaqueSpecial;

// ==== Fighters ====
#define MAX_NOM_PERSO 50
typedef struct {
    char nom[MAX_NOM_PERSO];
    int actu_pv;
    int max_pv;
    int attaque;
    int defense;
    int agilite;
    int vitesse;
    int pt;
    ElementType element;
    AttaqueSpecial spe_atq1;
    AttaqueSpecial spe_atq2;
    AttaqueSpecial spe_atq3;
} Fighter;



typedef struct {
    Fighter fighter1;
    Fighter fighter2;
    Fighter fighter3;

} Joueur;



typedef struct{
    Joueur joueur1;
    Joueur joueur2;

    int perso_actif; 
    int tour;
    bool fin;

    int mapType; // 1 = feu, 2 = glace, etc
    bool nuit;

} Partie;



typedef struct {
    SDL_Rect rect;
    SDL_Color baseColor;
    SDL_Color hoverColor;
    bool hovered;
    const char* text;
} Button;

typedef struct{
    int idAttaque;
    int utilisateurNum;
    int utilisateurEquipe;
    int cibleNum;
    int cibleEquipe;
} AttaqueSauvegarde;

extern AttaqueSpecial* toutes_les_attaques[NB_ATTAQUES_TOTAL];
extern void (*fonctions_attaques[NB_ATTAQUES_TOTAL])(Fighter*, Fighter*);

Fighter* get_fighter_by_index(int index);
int get_equipe_id(int index);
int get_fighter_num(int index);

extern Fighter darkshadow;
extern Fighter hitsugaya;
extern Fighter incassable;
extern Fighter katara;
extern Fighter kirua;
extern Fighter rengoku;
extern Fighter temari;
extern Fighter zoro;
extern Fighter lukas;

extern Joueur equipe1;
extern Partie partieActuelle;
extern Fighter persoChoisi[];

extern AttaqueSpecial Test1;
extern AttaqueSpecial Test2;
extern AttaqueSpecial Test3;

void runGame(SDL_Renderer* rendu);


Fighter        creer_fighter   (const char* nom, int actu_pv, int max_pv, int attaque,
                                int defense, int agilite, int vitesse, ElementType element,
                                AttaqueSpecial** attaques);
void           appliquer_buffs (Fighter* perso, BonusMap bonus);

#endif // DATA_H
