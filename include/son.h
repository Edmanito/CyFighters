// ----- Include/son.h -----
#ifndef SON_H
#define SON_H

// Joue une musique en boucle avec un volume donné (0–128)
extern void jouerMusique(const char* chemin, int volume);

// Arrête la musique actuelle
extern void arreter_musique();

// Joue un effet sonore ponctuel (ex : frappe, clic UI)
extern void jouer_effet(const char* chemin, int volume);

// Indicateur si la musique du menu a déjà été lancée (évite redondance)
extern int musiqueRes;

#endif // SON_H
