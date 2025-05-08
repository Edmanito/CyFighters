#ifndef LANGUE_H
#define LANGUE_H

typedef enum {
    FR,
    EN,
    ES,
    DE
} Langue;

extern Langue langueActuelle;

void changerLangue(Langue nouvelle);
const char* getTexte(const char* id);

#endif
