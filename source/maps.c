/*
#include "maps.h"

// Vérifie si l'élément du personnage correspond à la map actuelle
int booste_sur_map(ElementType element_perso, MapType map_actuelle) {
    switch (map_actuelle) {
        case MAP_CRISTAL:
            return (element_perso == ELEMENT_CRISTAL);
        case MAP_GLACE:
            return (element_perso == ELEMENT_GLACE);
        case MAP_FEU:
            return (element_perso == ELEMENT_FEU);
        case MAP_ECLAIR:
            return (element_perso == ELEMENT_ECLAIR);
        case MAP_VENT:
            return (element_perso == ELEMENT_VENT);
        case MAP_EAU:
            return (element_perso == ELEMENT_EAU);
        case MAP_OMBRE:
            return (element_perso == ELEMENT_OMBRE);
        case MAP_TEMPS:
        case MAP_MYSTERE:
        default:
            return 0; // Sur MAP_TEMPS ou MAP_MYSTERE : jamais de boost c'est des maps neutre
    }
}

// Renvoie les bonus à appliquer si le personnage est sur sa map
BonusMap obtenir_bonus_map(MapType map_actuelle, ElementType element_perso) {
    BonusMap bonus = {0, 0, 0, 0, 0}; // Tout à 0 par défaut

    // Si pas boosté sur la map, on retourne 0 direct
    if (!booste_sur_map(element_perso, map_actuelle)) {
        return bonus;
    }

    switch (element_perso) {
        case ELEMENT_CRISTAL:
            bonus.bonus_defense = 20;  // +20% DEF pour Incassable
            bonus.bonus_pv = 20;        // +20% PV aussi pour Incassable (comme tu voulais)
            break;
        case ELEMENT_GLACE:
            bonus.bonus_defense = 15;  // +15% DEF pour Hitsugaya
            bonus.bonus_vitesse = -10; // -10% vitesse (il se "gèle" lui-même un peu)
            break;
        case ELEMENT_FEU:
            bonus.bonus_attaque = 25;  // +25% ATT pour Rengoku
            break;
        case ELEMENT_ECLAIR:
            bonus.bonus_vitesse = 30;  // +30% vitesse pour Kirua
            break;
        case ELEMENT_VENT:
            bonus.bonus_agilite = 20;  // +20% esquive pour Temari
            break;
        case ELEMENT_EAU:
            bonus.bonus_pv = 30;        // +30% PV pour Katara
            break;
        case ELEMENT_OMBRE:
            bonus.bonus_attaque = 15;  // +15% ATT pour DarkShadow
            bonus.bonus_agilite = 10;  // +10% esquive pour DarkShadow
            break;
        default:
            break;
    }

    return bonus;
}
*/