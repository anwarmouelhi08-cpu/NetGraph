/*
    ELTARTICLE.H
    realisation du TDA ELEMENT par un article d'information
*/

#ifndef ELTARTICLE_H
#define ELTARTICLE_H

typedef struct {
    int  id;
    char titre[100];
    char source[50];
    int  score_fiabilite;
    int  jour, mois, annee;
    int  heure, minute;
} articleStruct, *ELEMENT;

#define ELEMENT_VIDE NULL

ELEMENT elementCreer();
void    elementDetruire(ELEMENT art);
void    elementAffecter(ELEMENT *dest, ELEMENT src);
void    elementCopier(ELEMENT *dest, ELEMENT src);
int     elementComparer(ELEMENT art1, ELEMENT art2);
void    elementAfficher(ELEMENT art);

#endif
