#include "langue.h"
#include <string.h>

Langue langueActuelle = FR;

const char* getTexte(const char* id) {
    static const struct {
        const char* id;
        const char* fr;
        const char* en;
        const char* es;
        const char* de;
    } dico[] = {
        // === Menu principal ===
        {"jouer", "Jouer", "Play", "Jugar", "Spielen"},
        {"options", "Options", "Options", "Opciones", "Optionen"},
        {"quitter", "Quitter", "Quit", "Salir", "Beenden"},

        // === Options ===
        {"credit", "Crédits", "Credits", "Créditos", "Credits"},
        {"langue", "Langue", "Language", "Idioma", "Sprache"},
        {"volume", "Volume", "Volume", "Volumen", "Lautstärke"},
        {"musique", "Musique", "Music", "Música", "Musik"},
        {"musique_1", "Musique 1", "Music 1", "Música 1", "Musik 1"},
        {"musique_2", "Musique 2", "Music 2", "Música 2", "Musik 2"},
        {"musique_3", "Musique 3", "Music 3", "Música 3", "Musik 3"},

        // === Difficulté ===
        {"facile", "Facile", "Easy", "Fácil", "Leicht"},
        {"moyen", "Moyen", "Medium", "Medio", "Mittel"},
        {"difficile", "Difficile", "Hard", "Difícil", "Schwierig"},

        // === Jeu ===
        {"jeu_en_cours", "JEU EN COURS...", "GAME IN PROGRESS...", "JUEGO EN CURSO...", "SPIEL LÄUFT..."},

        // === Chargement ===
        {"chargement", "Chargement en cours...", "Loading...", "Cargando...", "Wird geladen..."},

        // === Histoire ===
        {"hist_1", "Dans un monde divisé par les royaumes...", 
                  "In a world divided by kingdoms...", 
                  "En un mundo dividido por los reinos...", 
                  "In einer von Reichen geteilten Welt..."},
        {"hist_2", "Un phénomène étrange a bouleversé l’équilibre.", 
                  "A strange phenomenon disrupted the balance.", 
                  "Un fenómeno extraño alteró el equilibrio.", 
                  "Ein seltsames Phänomen brachte das Gleichgewicht durcheinander."},
        {"hist_3", "Les artefacts Shōnen refont surface...", 
                  "The Shōnen artifacts resurface...", 
                  "Los artefactos Shōnen resurgen...", 
                  "Die Shōnen-Artefakte tauchen wieder auf..."},
        {"hist_4", "Toi, jeune combattant, relèveras-tu le défi ?", 
                  "You, young fighter, will you take on the challenge?", 
                  "¿Tú, joven guerrero, aceptarás el desafío?", 
                  "Du, junger Kämpfer, wirst du die Herausforderung annehmen?"},
        {"hist_5", "Bienvenue dans Project Shōnen Smash.", 
                  "Welcome to Project Shōnen Smash.", 
                  "Bienvenido a Project Shōnen Smash.", 
                  "Willkommen bei Project Shōnen Smash."},
    };

    for (unsigned int i = 0; i < sizeof(dico)/sizeof(dico[0]); i++) {
        if (strcmp(id, dico[i].id) == 0) {
            switch (langueActuelle) {
                case EN: return dico[i].en;
                case ES: return dico[i].es;
                case DE: return dico[i].de;
                default: return dico[i].fr;
            }
        }
    }

    return id;  // fallback
}

void changerLangue(Langue nouvelle) {
    langueActuelle = nouvelle;
}
