/*
    LSTPRIM.C
    implementation du TDA LISTE par liste simplement chainee
    on suit exactement le cours de Mme / Mr l'enseignant
*/

#include <stdio.h>
#include <stdlib.h>
#include "LSTPRIM.h"

/* ---- fonctions internes sur les noeuds ---- */

/* creer un noeud contenant l'element e */
NOEUD noeudCreer(ELEMENT e) {
    NOEUD n = (NOEUD) malloc(sizeof(structNoeud));
    if (n == NULL) {
        printf("Plus d'espace\n");
        return NULL;
    }
    n->info    = e;
    n->suivant = NULL;
    return n;
}

/* liberer un noeud (sans toucher a l'element) */
void noeudDetruire(NOEUD n) {
    free(n);
}

/* ---- creation / destruction de la liste ---- */

LISTE listeCreer() {
    LISTE L = (LISTE) malloc(sizeof(laStruct));
    if (L == NULL) {
        printf("Probleme de memoire\n");
        return NULL;
    }
    L->lg   = 0;
    L->tete = NULL;
    return L;
}

/* detruire tous les noeuds puis la liste
   attention: on ne detruit pas les elements (articles) 
   car ils appartiennent au graphe */
void listeDetruire(LISTE L) {
    int i;
    NOEUD p, q;
    q = L->tete;
    for (i = 1; i <= L->lg; i++) {
        p = q;
        q = q->suivant;
        noeudDetruire(p);
    }
    free(L);
}

/* ---- predicats ---- */

int estVide(LISTE L) {
    return (L->lg == 0);
}

/* on essaie d'allouer un noeud temporaire pour tester la saturation */
int estSaturee(LISTE L) {
    NOEUD temp;
    int saturee = 1;
    temp = (NOEUD) malloc(sizeof(structNoeud));
    if (temp != NULL) {
        saturee = 0;
        free(temp);
    }
    return saturee;
}

int listeTaille(LISTE L) {
    return L->lg;
}

/* ---- insertion ---- */

/*
    inserer l'element e a la position pos
    cas 1 : insertion en tete (pos == 1)
    cas 2 : cas general (pos > 1) -> on parcourt jusqu'a pos-1
*/
int inserer(LISTE L, ELEMENT e, int pos) {
    int i;
    NOEUD n, p, q;

    if (estSaturee(L)) {
        printf("Liste saturee\n");
        return 0;
    }
    if (pos < 1 || pos > L->lg + 1) {
        printf("Position invalide\n");
        return 0;
    }

    n = noeudCreer(e);

    if (pos == 1) {
        /* insertion en tete */
        n->suivant = L->tete;
        L->tete    = n;
    } else {
        /* cas general : on avance jusqu'a pos-1 */
        q = L->tete;
        for (i = 1; i < pos; i++) {
            p = q;
            q = q->suivant;
        }
        /* p = noeud de rang pos-1, q = noeud de rang pos */
        p->suivant = n;
        n->suivant = q;
    }

    L->lg++;
    return 1;
}

/* ---- suppression ---- */

/*
    supprimer le noeud a la position pos
    on ne detruit pas l'element (gere par le graphe)
*/
int supprimer(LISTE L, int pos) {
    int i;
    NOEUD p, q;

    if (estVide(L)) {
        printf("Liste vide\n");
        return 0;
    }
    if (pos < 1 || pos > L->lg) {
        printf("Position invalide\n");
        return 0;
    }

    q = L->tete;

    if (pos == 1) {
        /* suppression en tete */
        L->tete = L->tete->suivant;
    } else {
        /* cas general */
        for (i = 1; i < pos; i++) {
            p = q;
            q = q->suivant;
        }
        p->suivant = q->suivant;
    }

    noeudDetruire(q);
    L->lg--;
    return 1;
}

/* ---- recuperer ---- */

/* retourne l'element a la position pos (sans le supprimer) */
ELEMENT recuperer(LISTE L, int pos) {
    int i;
    NOEUD p;
    ELEMENT elt = ELEMENT_VIDE;

    if (estVide(L)) {
        printf("Liste vide\n");
        return elt;
    }
    if (pos < 1 || pos > L->lg) {
        printf("Position invalide\n");
        return elt;
    }

    p = L->tete;
    for (i = 1; i < pos; i++)
        p = p->suivant;

    /* stockage indirect : on retourne le pointeur directement */
    return p->info;
}

/* ---- affichage ---- */

void listeAfficher(LISTE L) {
    int i;
    NOEUD p;
    p = L->tete;
    for (i = 1; i <= L->lg; i++) {
        elementAfficher(p->info);
        printf("\n");
        p = p->suivant;
    }
}

/* ---- copie ---- */

LISTE listeCopier(LISTE L) {
    LISTE LR = listeCreer();
    int i;
    ELEMENT elt;
    for (i = 1; i <= L->lg; i++) {
        elt = elementCreer();
        elementCopier(&elt, recuperer(L, i));
        inserer(LR, elt, i);
    }
    return LR;
}

/* ---- comparaison ---- */

int listeComparer(LISTE L1, LISTE L2) {
    int test = 1;
    int i = 1;
    if (listeTaille(L1) != listeTaille(L2)) test = 0;
    while (i <= listeTaille(L1) && test) {
        if (elementComparer(recuperer(L1, i), recuperer(L2, i)) != 0)
            test = 0;
        i++;
    }
    return test;
}
