#include "data.h"
#include "interface.h"
#include "son.h"

#include <stdlib.h>
#include <string.h>


// === Tableau global de toutes les attaques disponibles dans le jeu ===
AttaqueSpecial* toutes_les_attaques[NB_ATTAQUES_TOTAL];

// === Attaque classique et défense de base ===
AttaqueSpecial useAttaque = {
    .nom = "attaque",
    .description = "Une simple attaque physique",
    .id = ATTAQUE_BASIQUE,
    .cout = 0,
    .type = 1,
};
AttaqueSpecial useDefense = {
    .nom = "defense",
    .description = "Se défendre pour se tour",
    .id = DEFENSE,
    .cout = 0,
    .type = 2,
};

// === Définition des attaques spéciales ===
// Chaque AttaqueSpecial contient : nom, description, identifiant, coût en PT, type d'effet
AttaqueSpecial affutageMortal = {
    .nom = "affûtage mortal",
    .description = "Applique Saignement sur attaques normales pendant 3 tours.",
    .id = ATQ_AFFUTAGE_MORTAL,
    .cout = 5,
    .type = 5,
};

AttaqueSpecial assautTranchant ={
    .nom = "assaut tranchant",
    .description = "2 frappes à 60 pourcent chacune.",
    .id = ATQ_ASSAUT_TRANCHANT,
    .cout = 4,
    .type = 1,
};

AttaqueSpecial eveilDuSabre ={
    .nom = "éveil du sabre",
    .description = "+ 25 pourcent attaque pour 2 tours.",
    .id = ATQ_EVEIL_SABRE,
    .cout = 6,
    .type = 5,
};

AttaqueSpecial flammesSolaires ={
    .nom = "flammes solaires",
    .description = "Change le cycle en Jour",
    .id = ATQ_FLAMMES_SOLAIRES,
    .cout = 5,
    .type = 7,
};

AttaqueSpecial explosionArdente ={
    .nom = "explosion ardente",
    .description = "150 pourcent attaque magique + Brûlure.",
    .id = ATQ_EXPLOSION_ARDENTE,
    .cout = 8,
    .type = 9,
};

AttaqueSpecial espritFlamboyant ={
    .nom = "esprit flamboyant",
    .description = "Boost attaque/défense alliées +25 pourcent pour 2 tours.",
    .id = ATQ_ESPRIT_FLAMBOYANT,
    .cout = 7,
    .type = 5,
};

AttaqueSpecial prisonDeGivre ={
    .nom = "Prison de givre",
    .description = "Gel : -30 pourcent défense/agilité d'un ennemi pour 2 tours.",
    .id = ATQ_PRISON_DE_GIVRE,
    .cout = 5,
    .type = 6,
};

AttaqueSpecial blizzard ={
    .nom = "Blizzard",
    .description = "AoE physique (30 pourcent puissance) + 50 pourcent chance de Gel.",
    .id = ATQ_BLIZZARD,
    .cout = 7,
    .type = 3,
};

AttaqueSpecial glaceCurative ={
    .nom = "Glace curative",
    .description = "Soigne un allié de 20 pourcent PV max.",
    .id = ATQ_GLACE_CURATIVE,
    .cout = 4,
    .type = 4,
};

AttaqueSpecial lienDeSang ={
    .nom = "Lien de Sang",
    .description = "Immobilise un ennemi tant qu'il n'est pas blessé. Katara aussi.",
    .id = ATQ_LIEN_DE_SANG,
    .cout = 6,
    .type = 6,
};

AttaqueSpecial vagueGuerisseuse ={
    .nom = "Vague guérisseuse",
    .description = "Restaure 20 pourcent PV max à un allié.",
    .id = ATQ_VAGUE_GUERISSEUSE,
    .cout = 4,
    .type = 4,
};

AttaqueSpecial eveilLunaire ={
    .nom = "Éveil lunaire",
    .description = "Change le cycle en Nuit",
    .id = ATQ_EVEIL_LUNAIRE,
    .cout = 5,
    .type = 8,
};

AttaqueSpecial crepuscule ={
    .nom = "Éveil lunaire",
    .description = "Change le cycle en Nuit",
    .id = ATQ_CREPUSCULE,
    .cout = 5,
    .type = 8,
};

AttaqueSpecial hurlementNoir ={
    .nom = "Hurlement noir",
    .description = "AoE magique (70 pourcent magie).",
    .id = ATQ_HURLEMENT_NOIR,
    .cout = 6,
    .type = 3,
};

AttaqueSpecial brumeProtectrice ={
    .nom = "Brume protectrice",
    .description = "Protège un allié (réduction dégâts -30 pourcent, 2 tours).",
    .id = ATQ_BRUME_PROTECTRICE,
    .cout = 4,
    .type = 5,
};


AttaqueSpecial danseDuVent ={
    .nom = "Danse du vent",
    .description = "Réduit l'attaque ennemie de 25 pourcent pour 2 tours.",
    .id = ATQ_DANSE_DU_VENT,
    .cout = 5,
    .type = 6,
};

AttaqueSpecial ventPercant ={
    .nom = "Vent perçant",
    .description = "AoE magique (70 pourcent magie)",
    .id = ATQ_VENT_PERCANT,
    .cout = 5,
    .type = 3,
};

AttaqueSpecial souffleDeVie ={
    .nom = "Souffle de vie",
    .description = "Restaure 20 pourcent PV max à un allié.",
    .id = ATQ_SOUFFLE_DE_VIE,
    .cout = 4,
    .type = 4,
};

AttaqueSpecial fulgurance ={
    .nom = "Fulgurance",
    .description = "Ignore 50 pourcent de la défense sur l'attaque.",
    .id = ATQ_FULGURANCE,
    .cout = 5,
    .type = 9,
};

AttaqueSpecial foudreEnchainee ={
    .nom = "Foudre enchaînée",
    .description = "AoE physique (40 pourcent attaque)",
    .id = ATQ_FOUDRE_ENCHAINEE,
    .cout = 6,
    .type = 3,
};

AttaqueSpecial executionRapide ={
    .nom = "Exécution rapide",
    .description = "Inflige 200 pourcent attaque si cible <30 pourcent PV.",
    .id = ATQ_EXECUTION_RAPIDE,
    .cout = 7,
    .type = 9,
};

AttaqueSpecial murVivant ={
    .nom = "Mur vivant",
    .description = "Protège un allié : subit 100 pourcent de ses dégâts.",
    .id = ATQ_MUR_VIVANT,
    .cout = 5,
    .type = 2,
};

AttaqueSpecial barriereDePierre ={
    .nom = "Barrière de pierre",
    .description = "Augmente sa propre défense de 50 pourcent pour 2 tours.",
    .id = ATQ_BARRIERE_DE_PIERRE,
    .cout = 4,
    .type = 5,
};

AttaqueSpecial rugissementDacier ={
    .nom = "Rugissement d'acier",
    .description = "Boost défense alliée +25 pourcent pour 2 tours.",
    .id = ATQ_RUGISSEMENT_D_ACIER,
    .cout = 5,
    .type = 5,
};

// ===========================================================================
// === Déclaration de tous les personnages jouables avec leurs statistiques ===
// ===========================================================================

Fighter zoro={
    .nom ="zoro",
    .actu_pv =250,
    .max_pv =250,
    .attaque = 30,
    .defense = 15,
    .agilite = 20,
    .vitesse = 5,
    .magie = 15,
    .element = 7,
    .pt = 0
};

Fighter rengoku={
    .nom = "rengoku",
    .actu_pv = 280,
    .max_pv =280,
    .attaque = 20,
    .defense = 35,
    .agilite = 10,
    .vitesse = 10,
    .magie = 15,
    .element = 3,
    .pt = 0
};

Fighter hitsugaya={
    .nom = "hitsugaya",
    .actu_pv = 270,
    .max_pv =270,
    .attaque = 20,
    .defense = 30,
    .agilite = 15,
    .vitesse = 20,
    .magie = 25,
    .element = 2,
    .pt = 0
};

Fighter katara={
    .nom = "katara",
    .actu_pv = 230,
    .max_pv =230,
    .attaque = 20,
    .defense = 20,
    .agilite = 15,
    .vitesse = 30,
    .magie = 40,
    .element = 5,
    .pt = 0
};

Fighter darkshadow={
    .nom = "darkshadow",
    .actu_pv = 240,
    .max_pv =240,
    .attaque = 20,
    .defense = 20,
    .agilite = 20,
    .vitesse = 35,
    .magie = 35,
    .element = 6,
    .pt = 0
};

Fighter temari={
    .nom = "temari",
    .actu_pv = 220,
    .max_pv =220,
    .attaque = 15,
    .defense = 20,
    .agilite = 40,
    .vitesse = 40,
    .magie = 40,
    .element = 0,
    .pt = 0
};

Fighter kirua={
    .nom = "kirua",
    .actu_pv = 240,
    .max_pv =240,
    .attaque = 30,
    .defense = 10,
    .agilite = 25,
    .vitesse = 50,
    .magie = 20,
    .element = 4,
    .pt = 0
};

Fighter incassable={
    .nom = "incassable",
    .actu_pv = 300,
    .max_pv =300,
    .attaque = 10,
    .defense = 40,
    .agilite = 10,
    .vitesse = 1,
    .magie = 5,
    .element = 1,
    .pt = 0
};







// === Fonction pour récupérer un personnage par son index ===

Fighter get_fighter_depuis_nom(int index) {
    switch (index) {
        case 0: return darkshadow;
        case 1: return hitsugaya;
        case 2: return incassable;
        case 3: return katara;
        case 4: return kirua;
        case 5: return rengoku;
        case 6: return temari;
        case 7: return zoro;
        default: return darkshadow;
    }
}


// === Déclaration des variables globales du jeu ===
Joueur equipe1;
Partie partieActuelle;




