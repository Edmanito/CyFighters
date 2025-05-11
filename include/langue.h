#ifndef LANGUE_H
#define LANGUE_H

// === Enumération des langues disponibles ===
typedef enum {
    FR, // Français
    EN, // Anglais
    ES, // Espagnol
    DE  // Allemand
} Langue;

// Langue actuellement utilisée dans le jeu (modifiable via les options)
extern Langue langueActuelle;

// === Fonctions de gestion de la langue ===

// Change la langue active (ex : après un clic sur un drapeau)
void changerLangue(Langue nouvelle);

// Récupère un texte traduit à partir d’un identifiant (ex : "btn_jouer")
const char* getTexte(const char* id);

#endif
