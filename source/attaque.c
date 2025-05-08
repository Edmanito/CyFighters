// ----- attaque.c -----
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


// Effet de soin
void soin_effet(Fighter* lanceur, Fighter* cible, int quantite) {
    int avant = cible->actu_pv;
    int apres = avant + quantite;

    if (apres > cible->max_pv)
        apres = cible->max_pv;

    cible->actu_pv = apres;

    printf("%s soigne %s (+%d PV)\n", lanceur->nom, cible->nom, apres - avant);
}

void defense(Fighter* attaquant, Fighter* cible) {
    SDL_Log("attaque_affutage_mortal");
}

//structure avec le nom de l'effet et un cool down

void attaqueClassique(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = a.attaque * 2 - c.defense;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
    SDL_Log("attaqueClassique");
}

void attaque_affutage_mortal(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 1; // Saignement
    SDL_Log("attaque_affutage_mortal");
}

void attaque_assaut_tranchant(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = (a.attaque * 0.6) * 2 - c.defense;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats * 2;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_assaut_tranchant");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats * 2);
}

void attaque_eveil_du_sabre(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 4; // Boost attaque
    SDL_Log("attaque_eveil_du_sabre");
}

void attaque_flammes_solaires(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = false;
    SDL_Log("attaque_flammes_solaires");
}

void attaque_explosion_ardente(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    attaquant->statutEffet = 2; // Brûlure
    int degats = a.magie * 2 - c.magie;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_explosion_ardente");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_esprit_flamboyant(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 4; // Boost attaque
    SDL_Log("attaque_esprit_flamboyant");
}

void attaque_prison_de_givre(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 11; // Gel
    SDL_Log("attaque_prison_de_givre");
}

void attaque_blizzard(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    cible->statutEffet = 11; // Gel
    int degats = (a.attaque * 0.3) * 2 - c.defense;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_blizzard");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_glace_curative(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
}

void attaque_lien_de_sang(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 12;
    cible->statutEffet = 12;
    SDL_Log("attaque_lien_de_sang");
}

void attaque_vague_guerisseuse(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
}

void attaque_eveil_lunaire(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = true;
    SDL_Log("attaque_eveil_lunaire");
}

void attaque_crepuscule(Fighter* attaquant, Fighter* cible) {
    partieActuelle.nuit = true;
    SDL_Log("attaque_crepuscule");
}

void attaque_hurlement_noir(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = (a.magie * 0.7) * 2 - c.magie;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_hurlement_noir");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_brume_protectrice(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 3; // Boost défense
    SDL_Log("attaque_brume_protectrice");
}

void attaque_danse_du_vent(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 7; // Nerf attaque
    SDL_Log("attaque_danse_du_vent");
}

void attaque_vent_percant(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = (a.magie * 0.7) * 2 - c.magie;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_vent_percant");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_souffle_de_vie(Fighter* attaquant, Fighter* cible) {
    int gain = cible->max_pv * 0.2;
    soin_effet(attaquant, cible, gain);
}

void attaque_fulgurance(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = a.attaque * 2 - c.defense * 0.5;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_fulgurance");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_foudre_enchainee(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats = (a.attaque * 0.4) * 2 - c.defense;
    if (degats < 10) degats = 10;

    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_foudre_enchainee");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_execution_rapide(Fighter* attaquant, Fighter* cible) {
    Fighter a = appliquer_modificateurs(attaquant);
    Fighter c = appliquer_modificateurs(cible);

    int degats;
    if (cible->actu_pv < cible->max_pv * 0.3) {
        degats = (a.attaque * 2) * 2 - c.defense;
    } else {
        degats = (a.attaque * 0.5) * 2 - c.defense;
    }

    if (degats < 10) degats = 10;
    cible->actu_pv -= degats;
    if (cible->actu_pv < 0) cible->actu_pv = 0;
    if (cible->pt < 10) cible->pt++;

    SDL_Log("attaque_execution_rapide");
    printf("%s attaque %s (-%d PV)\n", a.nom, c.nom, degats);
}

void attaque_mur_vivant(Fighter* attaquant, Fighter* cible) {
    SDL_Log("attaque_mur_vivant (effet à définir)");
}

void attaque_barriere_de_pierre(Fighter* attaquant, Fighter* cible) {
    attaquant->statutEffet = 3; // Boost défense
    SDL_Log("attaque_barriere_de_pierre");
}

void attaque_rugissement_d_acier(Fighter* attaquant, Fighter* cible) {
    cible->statutEffet = 3; // Boost défense
    SDL_Log("attaque_rugissement_d_acier");
}
