#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// === Constantes générales ===
#define NB_PERSOS_EQUIPE 3 // Nombre de personnages par équipe
#define MAX_NOM_ATTAQUE 50
#define MAX_DESCRIPTION 300
#define MAX_SPECIAL 3
#define MAX_EFFETS 3
#define MAX_NOM_PERSO 50

// === Types d'élément associés aux personnages/attaques ===
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

// === Identifiants uniques pour les attaques spéciales ===
// Doivent correspondre aux fonctions et aux objets AttaqueSpecial du projet
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

// === Données d'une attaque spéciale ===
typedef struct {
    char nom[MAX_NOM_ATTAQUE];
    char description[MAX_DESCRIPTION];
    int id;    // Doit correspondre à une des valeurs d’AttaqueID
    int cout;  // Coût en points
    int type;  // Type d'effet, pour tri ou logique personnalisée
} AttaqueSpecial;

// === Données d’un combattant (joueur ou IA) ===
typedef struct {
    char nom[MAX_NOM_PERSO];
    int actu_pv, max_pv;
    int attaque, defense, agilite, vitesse, magie;
    int pt; // points à dépenser pour les attaques

    // Effet de statut actif (voir code pour signification des valeurs)
    int statutEffet;
    int dureeEffet;
    int protegePar; // -1 si non protégé, sinon index du protecteur (spécifique à certaines compétences)

    int element; // Utilise ElementType
    AttaqueSpecial spe_atq1, spe_atq2, spe_atq3;
} Fighter;

// === Structure pour stocker les 3 combattants d’un joueur ===
typedef struct {
    Fighter fighter1, fighter2, fighter3;
} Joueur;

// === Informations d’une partie en cours ===
typedef struct {
    Joueur joueur1, joueur2;
    int perso_actif; // index du combattant en action
    int tour;        // numéro du tour actuel
    int equipeQuiCommence;
    bool fin;        // indique si la partie est finie
    int mapType;     // type de map (modificateurs)
    bool nuit;       // effet de nuit
    int iaDifficulte;// 0 = pas d’IA, 1 = facile, etc.
} Partie;

// === Bonus d’environnement (ex : arène avantageuse) ===
typedef struct {
    int bonus_attaque;
    int bonus_defense;
    int bonus_vitesse;
    int bonus_agilite;
    int bonus_pv;
} BonusMap;

// === Sauvegarde d’une attaque à effectuer (utile pour IA, etc.) ===
typedef struct {
    int idAttaque;
    int utilisateurNum;
    int utilisateurEquipe;
    int cibleNum;
    int cibleEquipe;
} AttaqueSauvegarde;

// === Structure pour un bouton SDL interactif ===
typedef struct {
    SDL_Rect rect;
    SDL_Color baseColor, hoverColor;
    bool hovered;
    const char* text;
} Button;

// === Fonctions utilitaires utilisées pendant le combat ===
Fighter appliquer_modificateurs(Fighter* original);
Fighter* get_fighter(int numero);
Fighter* get_fighter_by_index(int index);
int get_equipe_id(int index);
int get_fighter_num(int index);
Fighter creer_fighter(const char* nom, int actu_pv, int max_pv, int attaque, int defense, int agilite, int vitesse, ElementType element, AttaqueSpecial** attaques);
Fighter get_fighter_depuis_nom(int index);
void appliquer_buffs(Fighter* perso, BonusMap bonus);
bool equipe_est_morte(int equipe);
void runGame(SDL_Renderer* rendu); // Lancement de la boucle de jeu principale

// === Variables globales ===
extern int idIncassble; // Index du personnage "Incassable"
extern int dureeMur;    // Durée de son mur protecteur
extern SDL_Window* fenetre;
extern int screenWidth;
extern int screenHeight;
extern Partie partieActuelle;
extern Fighter persoChoisi[];

// Tableau global d’attaques et de fonctions associées
extern AttaqueSpecial* toutes_les_attaques[NB_ATTAQUES_TOTAL];
extern void (*fonctions_attaques[NB_ATTAQUES_TOTAL])(Fighter*, Fighter*);

// === Personnages disponibles ===
extern Fighter darkshadow, hitsugaya, incassable, katara, kirua;
extern Fighter rengoku, temari, zoro, lukas;
extern Joueur equipe1; // équipe de test ?

// === Attaques de test ===
extern AttaqueSpecial Test1, Test2, Test3;

#endif // DATA_H
