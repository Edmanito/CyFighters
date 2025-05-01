// ----- src/data.c -----
#include "data.h"
#include "logic.h"
#include "interface.h"
#include <stdlib.h>
#include <string.h>

// Le tableau global de tous les personnages
Fighter personnage[8];

// Création d'une attaque spéciale
AttaqueSpecial* creer_attaqueSpe(const char* nom, const char* desc, int degats,
                                 StatutEffet effets[], int nb_effets, int tour) {
    AttaqueSpecial* attaque = malloc(sizeof(AttaqueSpecial));
    if (!attaque) return NULL;
    // Nom et description
    strncpy(attaque->nom, nom, MAX_NOM_ATTAQUE);
    attaque->nom[MAX_NOM_ATTAQUE - 1] = '\0';
    strncpy(attaque->description, desc, MAX_DESCRIPTION);
    attaque->description[MAX_DESCRIPTION - 1] = '\0';
    // Stats
    attaque->degats   = degats;
    attaque->nb_effets = nb_effets > MAX_EFFETS ? MAX_EFFETS : nb_effets;
    for (int i = 0; i < attaque->nb_effets; i++)
        attaque->effets[i] = effets[i];
    attaque->tour     = tour;
    return attaque;
}

// Création d'un fighter
Fighter creer_fighter(const char* nom, int actu_pv, int max_pv, int attaque,
                      int defense, int agilite, int vitesse, ElementType element,
                      AttaqueSpecial** attaques) {
    Fighter f;
    // Nom
    strncpy(f.nom, nom, MAX_NOM_PERSO);
    f.nom[MAX_NOM_PERSO - 1] = '\0';
    // Stats de base
    f.actu_pv  = actu_pv;
    f.max_pv   = max_pv;
    f.attaque  = attaque;
    f.defense  = defense;
    f.agilite  = agilite;
    f.vitesse  = vitesse;
    f.element  = element;
    // Attaques spéciales
    for (int i = 0; i < MAX_SPECIAL; i++)
        f.spe_atq[i] = (attaques ? attaques[i] : NULL);
    return f;
}

// Applique un bonus de map à un fighter
void appliquer_buffs(Fighter* perso, BonusMap bonus) {
    perso->attaque += perso->attaque * bonus.bonus_attaque / 100;
    perso->defense += perso->defense * bonus.bonus_defense / 100;
    perso->vitesse += perso->vitesse * bonus.bonus_vitesse / 100;
    perso->agilite += perso->agilite * bonus.bonus_agilite / 100;
    perso->max_pv  += perso->max_pv  * bonus.bonus_pv      / 100;
    // On rétablit les PV actuels au max après buff
    perso->actu_pv = perso->max_pv;
}

// Retourne 1 si l’élément du perso est avantageux sur la map, 0 sinon
int booste_sur_map(ElementType element_perso, MapType map_actuelle) {
    switch (map_actuelle) {
        case MAP_CRISTAL:  return element_perso == ELEMENT_CRISTAL;
        case MAP_GLACE:    return element_perso == ELEMENT_GLACE;
        case MAP_FEU:      return element_perso == ELEMENT_FEU;
        case MAP_ECLAIR:   return element_perso == ELEMENT_ECLAIR;
        case MAP_VENT:     return element_perso == ELEMENT_VENT;
        case MAP_EAU:      return element_perso == ELEMENT_EAU;
        case MAP_OMBRE:    return element_perso == ELEMENT_OMBRE;
        default:           return 0;
    }
}

// Calcule et renvoie le bonus de map pour un perso donné
BonusMap obtenir_bonus_map(MapType map_actuelle, ElementType element_perso) {
    BonusMap bonus = {0, 0, 0, 0, 0};
    // Si élément aligné → +20% attaque
    if (booste_sur_map(element_perso, map_actuelle)) {
        bonus.bonus_attaque = 20;
    }
    // Exemples de buffs supplémentaires selon la map
    switch (map_actuelle) {
        case MAP_GLACE:
            bonus.bonus_defense = 15;  // +15% défense sur Glace
            break;
        case MAP_FEU:
            bonus.bonus_vitesse = 10;  // +10% vitesse sur Feu
            break;
        case MAP_EAU:
            bonus.bonus_pv     = 25;   // +25% PV sur Eau
            break;
        case MAP_OMBRE:
            bonus.bonus_agilite = 20;  // +20% esquive sur Ombre
            break;
        // Ajoutez d’autres cas si besoin…
        default:
            break;
    }
    return bonus;
}
