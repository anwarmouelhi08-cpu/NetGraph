/*
    GRAPHE.H
    declaration du TDA grapheReseau
    
    Le reseau est un graphe oriente :
      - chaque noeud = un article
      - une arete A -> B signifie "A cite B"
    
    Representation par liste d'adjacence :
      - articles[]  : tableau des articles (indice = id)
      - adjList[]   : adjList[i] = liste des articles cites par i
      - degre_in[]  : nombre d'articles qui citent i
*/

#ifndef GRAPHE_H
#define GRAPHE_H

#include "ELTARTICLE.h"
#include "LSTPRIM.h"

typedef struct {
    int      V;
    ELEMENT *articles;
    LISTE   *adjList;
    int     *degre_in;
} grapheReseauStruct, *grapheReseau;

/* -- section 1 : gestion du graphe -- */
grapheReseau creerGraphe(int V);
grapheReseau chargerGraphe(const char *filename);
void         detruireGraphe(grapheReseau g);
int          ajouterArticle(grapheReseau g, ELEMENT art);
int          supprimerArticle(grapheReseau g, int idArt);
int          ajouterCitation(grapheReseau g, int idSrc, int idDest);
int          supprimerCitation(grapheReseau g, int idSrc, int idDest);
void         afficherGraphe(grapheReseau g);

/* -- section 2 : interrogation -- */
void    articlesCites(grapheReseau g, int idSrc);
void    articlesCitants(grapheReseau g, int idDest);
void    sourcesOriginales(grapheReseau g);
void    articlesIsoles(grapheReseau g);
ELEMENT articlePlusCite(grapheReseau g);

/* -- section 3 : chronologie -- */
int  comparerDates(ELEMENT art1, ELEMENT art2);
void trierParDate(grapheReseau g);
void premierCitant(grapheReseau g, int idDest);
void chainePropagation(grapheReseau g, int idSrc);

/* -- section 4 : BFS -- */
void simulerPropagation(grapheReseau g, int idSrc);
void articlesAccessibles(grapheReseau g, int idSrc);

/* -- section 5 : detection fake news -- */
int  analyserArticle(ELEMENT art);
void analyserReseau(grapheReseau g);
void articlesSuspectsCites(grapheReseau g);

/* -- section 6 : bonus -- */
void simulerSuppression(grapheReseau g, int idArt);
int  neutraliserPropagation(grapheReseau g, int idSrc, int idDest);

#endif
