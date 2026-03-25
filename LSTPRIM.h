/*
    LSTPRIM.H
    declaration du TDA LISTE par liste simplement chainee
    (exactement comme vu en cours)
*/

#ifndef LSTPRIM_H
#define LSTPRIM_H

#include "ELTARTICLE.h"

/* structure d'un noeud */
typedef struct structNoeud {
    ELEMENT            info;
    struct structNoeud *suivant;
} structNoeud, *NOEUD;

/* structure de la liste */
typedef struct {
    NOEUD tete;
    int   lg;
} laStruct, *LISTE;

/* primitives du cours */
LISTE   listeCreer();
void    listeDetruire(LISTE L);
int     estVide(LISTE L);
int     estSaturee(LISTE L);
int     listeTaille(LISTE L);
int     inserer(LISTE L, ELEMENT e, int pos);
int     supprimer(LISTE L, int pos);
ELEMENT recuperer(LISTE L, int pos);
void    listeAfficher(LISTE L);
LISTE   listeCopier(LISTE L);
int     listeComparer(LISTE L1, LISTE L2);

#endif
