/*
    ELTARTICLE.C
    implementation des fonctions du TDA ELEMENT (article)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ELTARTICLE.h"

/* creer un article vide en memoire */
ELEMENT elementCreer() {
    ELEMENT art = (ELEMENT) malloc(sizeof(articleStruct));
    if (art == NULL) {
        printf("erreur malloc elementCreer\n");
        return ELEMENT_VIDE;
    }
    art->id = -1;
    art->titre[0] = '\0';
    art->source[0] = '\0';
    art->score_fiabilite = 0;
    art->jour = 0; art->mois = 0; art->annee = 0;
    art->heure = 0; art->minute = 0;
    return art;
}

/* liberer la memoire d'un article */
void elementDetruire(ELEMENT art) {
    if (art != ELEMENT_VIDE)
        free(art);
}

/* copier les champs de src dans *dest */
void elementAffecter(ELEMENT *dest, ELEMENT src) {
    if (dest == NULL || src == NULL) return;
    (*dest)->id = src->id;
    (*dest)->score_fiabilite = src->score_fiabilite;
    (*dest)->jour   = src->jour;
    (*dest)->mois   = src->mois;
    (*dest)->annee  = src->annee;
    (*dest)->heure  = src->heure;
    (*dest)->minute = src->minute;
    strcpy((*dest)->titre,  src->titre);
    strcpy((*dest)->source, src->source);
}

/* alias de affecter */
void elementCopier(ELEMENT *dest, ELEMENT src) {
    elementAffecter(dest, src);
}

/*
    comparer deux articles par date/heure de publication
    retourne negatif si art1 est avant art2, 0 si egal, positif sinon
*/
int elementComparer(ELEMENT art1, ELEMENT art2) {
    if (art1->annee  != art2->annee)  return art1->annee  - art2->annee;
    if (art1->mois   != art2->mois)   return art1->mois   - art2->mois;
    if (art1->jour   != art2->jour)   return art1->jour   - art2->jour;
    if (art1->heure  != art2->heure)  return art1->heure  - art2->heure;
    return art1->minute - art2->minute;
}

/* afficher un article */
void elementAfficher(ELEMENT art) {
    if (art == NULL) { printf("(vide)\n"); return; }
    printf("%s (id:%d, source:%s, score:%d, %02d/%02d/%04d %02dh%02d)",
           art->titre, art->id, art->source, art->score_fiabilite,
           art->jour, art->mois, art->annee, art->heure, art->minute);
}
