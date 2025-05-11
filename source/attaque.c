#include <stdio.h>
#include <string.h>
#include "attaque.h"
#include "data.h"

typedef void (*FonctionAttaque)(Fighter*, Fighter*);

void init_attaques() {
    toutes_les_attaques[ATTAQUE_BASIQUE] = &useAttaque;
    toutes_les_attaques[DEFENSE] = &useDefense;
    toutes_les_attaques[AFFUTAGE_MORTAL] = &affutageMortal;
    toutes_les_attaques[ASSAUT_TRANCHANT] = &assautTranchant;
    toutes_les_attaques[EVEIL_DU_SABRE] = &eveilDuSabre;
    toutes_les_attaques[FLAMMES_SOLAIRES] = &flammesSolaires;
    toutes_les_attaques[EXPLOSION_ARDENTE] = &explosionArdente;
    toutes_les_attaques[ESPRIT_FLAMBOYANT] = &espritFlamboyant;
    toutes_les_attaques[PRISON_DE_GIVRE] = &prisonDeGivre;
    toutes_les_attaques[BLIZZARD] = &blizzard;
    toutes_les_attaques[GLACE_CURATIVE] = &glaceCurative;
    toutes_les_attaques[LIEN_DE_SANG] = &lienDeSang;
    toutes_les_attaques[VAGUE_GUERISSEUSE] = &vagueGuerisseuse;
    toutes_les_attaques[EVEIL_LUNAIRE] = &eveilLunaire;
    toutes_les_attaques[CREPUSCULE] = &crepuscule;
    toutes_les_attaques[HURLEMENT_NOIR] = &hurlementNoir;
    toutes_les_attaques[BRUME_PROTECTRICE] = &brumeProtectrice;
    toutes_les_attaques[DANSE_DU_VENT] = &danseDuVent;
    toutes_les_attaques[VENT_PERÇANT] = &ventPercant;
    toutes_les_attaques[SOUFFLE_DE_VIE] = &souffleDeVie;
    toutes_les_attaques[FULGURANCE] = &fulgurance;
    toutes_les_attaques[FOUDRE_ENCHAINEE] = &foudreEnchainee;
    toutes_les_attaques[EXECUTION_RAPIDE] = &executionRapide;
    toutes_les_attaques[MUR_VIVANT] = &murVivant;
    toutes_les_attaques[BARRIERE_DE_PIERRE] = &barriereDePierre;
    toutes_les_attaques[RUGISSEMENT_D_ACIER] = &rugissementDacier;
}


FonctionAttaque fonctions_attaques[NB_ATTAQUES_TOTAL] = {
    [ATTAQUE_BASIQUE]       = attaqueClassique,
    [DEFENSE]       = defense,
    [ATQ_AFFUTAGE_MORTAL]       = attaque_affutage_mortal,
    [ATQ_ASSAUT_TRANCHANT]      = attaque_assaut_tranchant,
    [ATQ_EVEIL_SABRE]           = attaque_eveil_du_sabre,
    [ATQ_FLAMMES_SOLAIRES]      = attaque_flammes_solaires,
    [ATQ_EXPLOSION_ARDENTE]     = attaque_explosion_ardente,
    [ATQ_ESPRIT_FLAMBOYANT]     = attaque_esprit_flamboyant,
    [ATQ_PRISON_DE_GIVRE]       = attaque_prison_de_givre,
    [ATQ_BLIZZARD]              = attaque_blizzard,
    [ATQ_GLACE_CURATIVE]        = attaque_glace_curative,
    [ATQ_LIEN_DE_SANG]          = attaque_lien_de_sang,
    [ATQ_VAGUE_GUERISSEUSE]     = attaque_vague_guerisseuse,
    [ATQ_EVEIL_LUNAIRE]         = attaque_eveil_lunaire,
    [ATQ_CREPUSCULE]            = attaque_crepuscule,
    [ATQ_HURLEMENT_NOIR]        = attaque_hurlement_noir,
    [ATQ_BRUME_PROTECTRICE]     = attaque_brume_protectrice,
    [ATQ_DANSE_DU_VENT]         = attaque_danse_du_vent,
    [ATQ_VENT_PERCANT]          = attaque_vent_percant,
    [ATQ_SOUFFLE_DE_VIE]        = attaque_souffle_de_vie,
    [ATQ_FULGURANCE]            = attaque_fulgurance,
    [ATQ_FOUDRE_ENCHAINEE]      = attaque_foudre_enchainee,
    [ATQ_EXECUTION_RAPIDE]      = attaque_execution_rapide,
    [ATQ_MUR_VIVANT]            = attaque_mur_vivant,
    [ATQ_BARRIERE_DE_PIERRE]    = attaque_barriere_de_pierre,
    [ATQ_RUGISSEMENT_D_ACIER]   = attaque_rugissement_d_acier
};

//recuperation d'indice pour mur vivant

extern Fighter persoChoisi[NB_PERSOS_EQUIPE * 2]; // accès au tableau

int get_index_fighter(Fighter* f) {
    for (int i = 0; i < NB_PERSOS_EQUIPE * 2; i++) {
        if (f == &persoChoisi[i]) return i;
    }
    return -1;
}


// Effet de soin

void soin_effet(Fighter* lanceur, Fighter* cible, int quantite) {
    int avant = cible->actu_pv;
    int apres = avant + quantite;
    if (apres > cible->max_pv) apres = cible->max_pv;
    cible->actu_pv = apres;
    SDL_Log("%s utilise Soin sur %s (+%d PV)\n", lanceur->nom, cible->nom, apres - avant);
}

void defense(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 13 ;
    attaquant->dureeEffet = 1;
    SDL_Log("%s Choisi de ce defendre\n", attaquant->nom);
}

void attaqueClassique(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter modifCible;
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
    switch (idIncassble) {
    case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
    case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
    case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
    case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
    case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
    case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
    }
    }


    modifCible = appliquer_modificateurs(vraieCible);

    int degats = a.attaque * 2 - modifCible.defense;

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

    if (vraieCible->pt < 10) vraieCible->pt++;

    SDL_Log("%s utilise Attaque Classique sur %s (-%d PV)", a.nom, vraieCible->nom, degats);
}



void attaque_affutage_mortal(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 1;
    attaquant->dureeEffet = 3;
    SDL_Log("%s applique l'effet Saignement\n", attaquant->nom);
}

void attaque_assaut_tranchant(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter modifCible;
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
    switch (idIncassble) {
    case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
    case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
    case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
    case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
    case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
    case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
    }
    }


    modifCible = appliquer_modificateurs(vraieCible);

    int degats = (a.attaque *0.6 )* 2 - modifCible.defense;

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;
    

    if (vraieCible->pt < 10) vraieCible->pt++;

    SDL_Log("%s utilise Assaut Tranchant sur %s (-%d PV)\n", a.nom, cible->nom, degats * 2);
}



void attaque_eveil_du_sabre(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 4;
    attaquant->dureeEffet = 2;
    SDL_Log("%s augmente son attaque (Éveil du Sabre)\n", attaquant->nom);
}



void attaque_flammes_solaires(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = false;
    SDL_Log("%s invoque le jour (Flammes Solaires)\n", attaquant->nom);
}

void attaque_explosion_ardente(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter modifCible;
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
        switch (idIncassble) {
            case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
            case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
            case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
            case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
            case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
            case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
        }
    }


    modifCible = appliquer_modificateurs(vraieCible);

    int degats = a.magie* 2 - modifCible.magie;

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;
    

    if (vraieCible->pt < 10) vraieCible->pt++;
    SDL_Log("%s utilise Explosion Ardente sur %s (-%d PV)\n", a.nom, cible->nom, degats);
}



void attaque_esprit_flamboyant(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 4;
    attaquant->dureeEffet = 2;
    SDL_Log("%s augmente son attaque (Esprit Flamboyant)\n", attaquant->nom);
}

void attaque_prison_de_givre(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 11;
    cible->dureeEffet = 2;
    SDL_Log("%s gèle %s (Prison de Givre)\n", attaquant->nom, cible->nom);
}

void attaque_blizzard(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter modifCible;
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
    switch (idIncassble) {
    case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
    case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
    case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
    case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
    case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
    case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
    }
    }


    modifCible = appliquer_modificateurs(vraieCible);

    int degats = a.attaque * 2 - modifCible.defense;

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

    if (vraieCible->pt < 10) vraieCible->pt++;

    SDL_Log("%s utilise Blizzard sur %s (-%d PV + Gel)\n", a.nom, cible->nom, degats);
}


void attaque_glace_curative(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
    SDL_Log("%s utilise glace curative sur %s (+%d PV)\n", attaquant->nom, cible->nom, gain);}

void attaque_lien_de_sang(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 12;
    cible->statutEffet = 12;
    attaquant->dureeEffet = 2;
    cible->dureeEffet = 2;
    SDL_Log("%s crée un Lien de Sang avec %s\n", attaquant->nom, cible->nom);
}


void attaque_vague_guerisseuse(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
    SDL_Log("%s utilise vague guerrisseuse sur %s (+%d PV)\n", attaquant->nom, cible->nom, gain);
}

void attaque_eveil_lunaire(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = true;
    SDL_Log("%s invoque la nuit (Éveil Lunaire)\n", attaquant->nom);
}

void attaque_crepuscule(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = true;
    SDL_Log("%s plonge le combat dans le Crépuscule\n", attaquant->nom);
}

//aoe
void attaque_hurlement_noir(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    int equipeAdverse = (attaquant == &partieActuelle.joueur1.fighter1 ||
                         attaquant == &partieActuelle.joueur1.fighter2 ||
                         attaquant == &partieActuelle.joueur1.fighter3) ? 2 : 1;

    Fighter* cibles[3];
    if (equipeAdverse == 1) {
        cibles[0] = &partieActuelle.joueur1.fighter1;
        cibles[1] = &partieActuelle.joueur1.fighter2;
        cibles[2] = &partieActuelle.joueur1.fighter3;
    } else {
        cibles[0] = &partieActuelle.joueur2.fighter1;
        cibles[1] = &partieActuelle.joueur2.fighter2;
        cibles[2] = &partieActuelle.joueur2.fighter3;
    }

    for (int i = 0; i < 3; i++) {
        Fighter* vraieCible = cibles[i];

        // Skip les morts
        if (vraieCible->actu_pv <= 0) continue;

        // Mur vivant
        int equipeCibleInitiale = equipeAdverse;
        int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

        if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
            switch (idIncassble) {
                case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
                case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
                case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
                case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
                case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
                case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
            }
        }

        Fighter modifCible = appliquer_modificateurs(vraieCible);
        int degats = a.magie * 2 - modifCible.magie;

        if (vraieCible->statutEffet == 13) {
            degats *= 0.1;
            vraieCible->statutEffet = 0;
            SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
        }

        if (degats < 10) degats = 10;
        vraieCible->actu_pv -= degats;
        if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

        if (vraieCible->pt < 10) vraieCible->pt++;

        SDL_Log("%s subit Hurlement Noir (-%d PV)", vraieCible->nom, degats);
    }
}


void attaque_brume_protectrice(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 3;
    cible->dureeEffet = 2;
    SDL_Log("%s augmente la défense de %s (Brume Protectrice)\n", attaquant->nom, cible->nom);
}

void attaque_danse_du_vent(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 7;
    cible->dureeEffet = 2;
    SDL_Log("%s affaiblit l’attaque de %s (Danse du Vent)\n", attaquant->nom, cible->nom);
}

//aoe
void attaque_vent_percant(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    int equipeAdverse = (attaquant == &partieActuelle.joueur1.fighter1 ||
                         attaquant == &partieActuelle.joueur1.fighter2 ||
                         attaquant == &partieActuelle.joueur1.fighter3) ? 2 : 1;

    Fighter* cibles[3];
    if (equipeAdverse == 1) {
        cibles[0] = &partieActuelle.joueur1.fighter1;
        cibles[1] = &partieActuelle.joueur1.fighter2;
        cibles[2] = &partieActuelle.joueur1.fighter3;
    } else {
        cibles[0] = &partieActuelle.joueur2.fighter1;
        cibles[1] = &partieActuelle.joueur2.fighter2;
        cibles[2] = &partieActuelle.joueur2.fighter3;
    }

    for (int i = 0; i < 3; i++) {
        Fighter* vraieCible = cibles[i];

        // Skip les morts
        if (vraieCible->actu_pv <= 0) continue;

        // Mur vivant
        int equipeCibleInitiale = equipeAdverse;
        int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

        if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
            switch (idIncassble) {
                case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
                case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
                case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
                case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
                case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
                case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
            }
        }

        Fighter modifCible = appliquer_modificateurs(vraieCible);
        int degats = a.magie * 2 - modifCible.magie;

        if (vraieCible->statutEffet == 13) {
            degats *= 0.1;
            vraieCible->statutEffet = 0;
            SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
        }

        if (degats < 10) degats = 10;
        vraieCible->actu_pv -= degats;
        if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

        if (vraieCible->pt < 10) vraieCible->pt++;

        SDL_Log("%s utilise Vent Perçant sur %s (-%d PV)\n", a.nom, cible->nom, degats);
    }
}



void attaque_souffle_de_vie(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
    SDL_Log("%s utilise souffle de vie sur %s (+%d PV)\n", attaquant->nom, cible->nom, gain);
}

void attaque_fulgurance(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter modifCible;
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
    switch (idIncassble) {
    case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
    case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
    case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
    case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
    case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
    case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
    }
    }


    modifCible = appliquer_modificateurs(vraieCible);

    int degats = a.attaque * 2 - modifCible.defense * 0.5;

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

    if (vraieCible->pt < 10) vraieCible->pt++;

    SDL_Log("%s utilise Fulgurance sur %s (-%d PV)\n", a.nom, cible->nom, degats);
}


//aoe
void attaque_foudre_enchainee(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    int equipeAdverse = (attaquant == &partieActuelle.joueur1.fighter1 ||
                         attaquant == &partieActuelle.joueur1.fighter2 ||
                         attaquant == &partieActuelle.joueur1.fighter3) ? 2 : 1;

    Fighter* cibles[3];
    if (equipeAdverse == 1) {
        cibles[0] = &partieActuelle.joueur1.fighter1;
        cibles[1] = &partieActuelle.joueur1.fighter2;
        cibles[2] = &partieActuelle.joueur1.fighter3;
    } else {
        cibles[0] = &partieActuelle.joueur2.fighter1;
        cibles[1] = &partieActuelle.joueur2.fighter2;
        cibles[2] = &partieActuelle.joueur2.fighter3;
    }

    for (int i = 0; i < 3; i++) {
        Fighter* vraieCible = cibles[i];

        // Skip les morts
        if (vraieCible->actu_pv <= 0) continue;

        // Mur vivant
        int equipeCibleInitiale = equipeAdverse;
        int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

        if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
            switch (idIncassble) {
                case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
                case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
                case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
                case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
                case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
                case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
            }
        }

        Fighter modifCible = appliquer_modificateurs(vraieCible);
        int degats = (a.attaque * 0.4) * 2 - modifCible.defense;

        if (vraieCible->statutEffet == 13) {
            degats *= 0.1;
            vraieCible->statutEffet = 0;
            SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
        }

        if (degats < 10) degats = 10;
        vraieCible->actu_pv -= degats;
        if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

        if (vraieCible->pt < 10) vraieCible->pt++;
        
        SDL_Log("%s utilise Foudre Enchaînée sur %s (-%d PV)\n", a.nom, cible->nom, degats);
    }
}


void attaque_execution_rapide(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter* vraieCible = cible;

    int equipeCibleInitiale = (cible == &partieActuelle.joueur1.fighter1 || 
        cible == &partieActuelle.joueur1.fighter2 || 
        cible == &partieActuelle.joueur1.fighter3) ? 1 : 2;

    int equipeIncassable = (idIncassble <= 2) ? 1 : 2;

    if (dureeMur > 0 && equipeCibleInitiale == equipeIncassable) {
        switch (idIncassble) {
            case 0: vraieCible = &partieActuelle.joueur1.fighter1; break;
            case 1: vraieCible = &partieActuelle.joueur1.fighter2; break;
            case 2: vraieCible = &partieActuelle.joueur1.fighter3; break;
            case 3: vraieCible = &partieActuelle.joueur2.fighter1; break;
            case 4: vraieCible = &partieActuelle.joueur2.fighter2; break;
            case 5: vraieCible = &partieActuelle.joueur2.fighter3; break;
        }
    }

    // Calcul des dégâts basé sur la cible originale
    int degats;
    if (cible->actu_pv < cible->max_pv * 0.3) {
        degats = (a.attaque * 2) * 2;
    } else {
        degats = (a.attaque * 0.5) * 2;
    }

    if (vraieCible->statutEffet == 13) {
        degats *= 0.1;
        vraieCible->statutEffet = 0;
        SDL_Log("Défense active : dégâts réduits pour %s !", vraieCible->nom);
    }

    if (degats < 10) degats = 10;

    vraieCible->actu_pv -= degats;
    if (vraieCible->actu_pv < 0) vraieCible->actu_pv = 0;

    if (vraieCible->pt < 10) vraieCible->pt++;

    SDL_Log("%s utilise Exécution Rapide sur %s (-%d PV)\n", a.nom, cible->nom, degats);
}


void attaque_mur_vivant(Fighter* attaquant, Fighter* cible) {
    dureeMur = 3;
    SDL_Log("%s protége ses alliés (Mur Vivant)\n", attaquant->nom);
}



void attaque_barriere_de_pierre(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 3;
    attaquant->dureeEffet = 2;
    SDL_Log("%s augmente sa défense (Barrière de Pierre)\n", attaquant->nom);
}

void attaque_rugissement_d_acier(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 3;
    cible->dureeEffet = 2;
    SDL_Log("%s augmente la défense de %s (Rugissement d’Acier)\n", attaquant->nom, cible->nom);
}