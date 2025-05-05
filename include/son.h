// ----- Include/son.h -----
#ifndef SON_H
#define SON_H

extern void jouerMusique(const char* chemin, int volume);
extern void arreter_musique();
extern void jouer_effet(const char* chemin, int volume);
extern int musiqueRes;
#endif // SON_H