/*
    GRAPHE.C
    implementation du graphe reseau d'information
    structure : liste d'adjacence (on utilise le TDA LISTE du cours)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "graphe.h"
#include "fakesdb.h"

/* ============================================================
   SECTION 1 : gestion du graphe
   ============================================================ */

grapheReseau creerGraphe(int V) {
    int i;
    grapheReseau g = (grapheReseau) malloc(sizeof(grapheReseauStruct));
    if (g == NULL) {
        printf("erreur malloc creerGraphe\n");
        return NULL;
    }

    g->V        = V;
    g->articles = (ELEMENT *) malloc(V * sizeof(ELEMENT));
    g->adjList  = (LISTE *)   malloc(V * sizeof(LISTE));
    g->degre_in = (int *)     malloc(V * sizeof(int));

    for (i = 0; i < V; i++) {
        g->articles[i] = ELEMENT_VIDE;
        g->adjList[i]  = listeCreer();
        g->degre_in[i] = 0;
    }

    return g;
}

/* charger le graphe depuis un fichier texte
   format : A id "titre" source score jour mois annee heure minute
            C id_src id_dest
            # commentaire */
grapheReseau chargerGraphe(const char *filename) {
    FILE *f;
    char ligne[300];
    int nb = 0;
    grapheReseau g;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    /* premiere passe : compter les articles pour savoir la taille du graphe */
    while (fgets(ligne, sizeof(ligne), f)) {
        if (ligne[0] == 'A') nb++;
    }
    rewind(f);

    g = creerGraphe(nb);
    if (g == NULL) { fclose(f); return NULL; }

    /* deuxieme passe : lire les articles puis les citations */
    while (fgets(ligne, sizeof(ligne), f)) {

        if (ligne[0] == '#' || ligne[0] == '\n') continue;

        if (ligne[0] == 'A') {
            ELEMENT art = elementCreer();
            int lu = sscanf(ligne, "A %d \"%99[^\"]\" %49s %d %d %d %d %d %d",
                            &art->id, art->titre, art->source,
                            &art->score_fiabilite,
                            &art->jour, &art->mois, &art->annee,
                            &art->heure, &art->minute);
            if (lu < 9) {
                printf("ligne mal formee ignoree : %s", ligne);
                elementDetruire(art);
                continue;
            }
            ajouterArticle(g, art);
        }

        if (ligne[0] == 'C') {
            int src, dest;
            if (sscanf(ligne, "C %d %d", &src, &dest) == 2)
                ajouterCitation(g, src, dest);
        }
    }

    fclose(f);
    printf("graphe charge : %d articles\n\n", nb);
    return g;
}

/* liberer toute la memoire du graphe */
void detruireGraphe(grapheReseau g) {
    int i;
    if (g == NULL) return;

    for (i = 0; i < g->V; i++) {
        listeDetruire(g->adjList[i]);
        if (g->articles[i] != ELEMENT_VIDE)
            elementDetruire(g->articles[i]);
    }

    free(g->articles);
    free(g->adjList);
    free(g->degre_in);
    free(g);
}

/* ajouter un article dans le tableau */
int ajouterArticle(grapheReseau g, ELEMENT art) {
    if (art->id < 0 || art->id >= g->V) {
        printf("id %d invalide\n", art->id);
        return 0;
    }
    if (g->articles[art->id] != ELEMENT_VIDE) {
        printf("article %d existe deja\n", art->id);
        return 0;
    }
    g->articles[art->id] = art;
    return 1;
}

/* supprimer un article et toutes ses citations (entrantes et sortantes) */
int supprimerArticle(grapheReseau g, int idArt) {
    int i, pos;
    ELEMENT e;

    if (idArt < 0 || idArt >= g->V || g->articles[idArt] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idArt);
        return 0;
    }

    /* vider la liste de citations sortantes */
    while (!estVide(g->adjList[idArt])) {
        e = recuperer(g->adjList[idArt], 1);
        if (e != NULL) g->degre_in[e->id]--;
        supprimer(g->adjList[idArt], 1);
    }

    /* supprimer les citations entrantes vers cet article */
    for (i = 0; i < g->V; i++) {
        if (i == idArt || g->articles[i] == ELEMENT_VIDE) continue;
        pos = 1;
        while (pos <= listeTaille(g->adjList[i])) {
            e = recuperer(g->adjList[i], pos);
            if (e != NULL && e->id == idArt) {
                supprimer(g->adjList[i], pos);
                g->degre_in[idArt]--;
            } else {
                pos++;
            }
        }
    }

    elementDetruire(g->articles[idArt]);
    g->articles[idArt] = ELEMENT_VIDE;
    g->degre_in[idArt] = 0;
    return 1;
}

/* ajouter une citation orientee src -> dest */
int ajouterCitation(grapheReseau g, int idSrc, int idDest) {
    int pos;
    ELEMENT e;

    if (g->articles[idSrc] == ELEMENT_VIDE) {
        printf("article source %d introuvable\n", idSrc);
        return 0;
    }
    if (g->articles[idDest] == ELEMENT_VIDE) {
        printf("article destination %d introuvable\n", idDest);
        return 0;
    }
    if (idSrc == idDest) {
        printf("un article ne peut pas se citer lui-meme\n");
        return 0;
    }

    /* verifier que la citation n'existe pas deja */
    for (pos = 1; pos <= listeTaille(g->adjList[idSrc]); pos++) {
        e = recuperer(g->adjList[idSrc], pos);
        if (e != NULL && e->id == idDest) {
            printf("citation %d -> %d deja existante\n", idSrc, idDest);
            return 0;
        }
    }

    /* on ajoute l'article dest a la fin de la liste d'adjacence de src */
    inserer(g->adjList[idSrc], g->articles[idDest],
            listeTaille(g->adjList[idSrc]) + 1);
    g->degre_in[idDest]++;
    return 1;
}

/* supprimer la citation src -> dest */
int supprimerCitation(grapheReseau g, int idSrc, int idDest) {
    int pos;
    ELEMENT e;

    if (g->articles[idSrc] == ELEMENT_VIDE || g->articles[idDest] == ELEMENT_VIDE) {
        printf("article(s) inexistant(s)\n");
        return 0;
    }

    for (pos = 1; pos <= listeTaille(g->adjList[idSrc]); pos++) {
        e = recuperer(g->adjList[idSrc], pos);
        if (e != NULL && e->id == idDest) {
            supprimer(g->adjList[idSrc], pos);
            g->degre_in[idDest]--;
            return 1;
        }
    }

    printf("citation %d -> %d introuvable\n", idSrc, idDest);
    return 0;
}

/* afficher le graphe complet */
void afficherGraphe(grapheReseau g) {
    int i, pos;
    ELEMENT e;

    printf("\n===== Reseau d'Information =====\n");
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;

        e = g->articles[i];
        printf("\n%s (id:%d, source:%s, score:%d, %02d/%02d/%04d %02dh%02d)\n",
               e->titre, e->id, e->source, e->score_fiabilite,
               e->jour, e->mois, e->annee, e->heure, e->minute);

        if (estVide(g->adjList[i])) {
            printf("  (ne cite aucun article)\n");
        } else {
            for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
                ELEMENT cite = recuperer(g->adjList[i], pos);
                if (cite != NULL)
                    printf("  --> %s\n", cite->titre);
            }
        }
    }
    printf("\n================================\n");
}


/* ============================================================
   SECTION 2 : interrogation du reseau
   ============================================================ */

/* afficher les articles cites par idSrc */
void articlesCites(grapheReseau g, int idSrc) {
    int pos;
    ELEMENT e;

    if (g->articles[idSrc] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idSrc);
        return;
    }

    printf("Articles cites par %s :\n", g->articles[idSrc]->titre);

    if (estVide(g->adjList[idSrc])) {
        printf("  (ne cite aucun article)\n");
        return;
    }

    for (pos = 1; pos <= listeTaille(g->adjList[idSrc]); pos++) {
        e = recuperer(g->adjList[idSrc], pos);
        if (e != NULL)
            printf("  --> %s\n", e->titre);
    }
}

/* afficher les articles qui citent idDest
   on parcourt toutes les listes d'adjacence */
void articlesCitants(grapheReseau g, int idDest) {
    int i, pos, trouve;
    ELEMENT e;

    if (g->articles[idDest] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idDest);
        return;
    }

    printf("Articles qui citent %s :\n", g->articles[idDest]->titre);
    trouve = 0;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;
        for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
            e = recuperer(g->adjList[i], pos);
            if (e != NULL && e->id == idDest) {
                printf("  --> %s\n", g->articles[i]->titre);
                trouve = 1;
                break;
            }
        }
    }

    if (!trouve) printf("  (personne ne cite cet article)\n");
}

/* sources originales = articles qui ne citent personne (adjList vide) */
void sourcesOriginales(grapheReseau g) {
    int i, trouve = 0;
    printf("Sources originales (ne citent aucun autre article) :\n");
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && estVide(g->adjList[i])) {
            printf("  --> %s\n", g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve) printf("  (aucune)\n");
}

/* articles isoles = articles pas cites par personne (degre_in == 0) */
void articlesIsoles(grapheReseau g) {
    int i, trouve = 0;
    printf("Articles non cites (isoles) :\n");
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && g->degre_in[i] == 0) {
            printf("  --> %s\n", g->articles[i]->titre);
            trouve = 1;
        }
    }
    if (!trouve) printf("  (tous les articles sont cites)\n");
}

/* retourner et afficher le(s) article(s) le(s) plus cite(s) */
ELEMENT articlePlusCite(grapheReseau g) {
    int i, max = -1;
    ELEMENT res = ELEMENT_VIDE;

    /* trouver le degre_in maximum */
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && g->degre_in[i] > max)
            max = g->degre_in[i];
    }

    if (max <= 0) {
        printf("aucun article n'est cite\n");
        return ELEMENT_VIDE;
    }

    printf("Article(s) le(s) plus cite(s) (%d citation(s)) :\n", max);
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && g->degre_in[i] == max) {
            printf("  --> %s (cite par %d articles)\n",
                   g->articles[i]->titre, g->degre_in[i]);
            res = g->articles[i];
        }
    }
    return res;
}


/* ============================================================
   SECTION 3 : analyse chronologique
   ============================================================ */

/* comparer deux articles par date - on utilise elementComparer */
int comparerDates(ELEMENT art1, ELEMENT art2) {
    return elementComparer(art1, art2);
}

/* trier les articles par date de publication (tri par insertion)
   on cree un tableau temporaire pour ne pas modifier le graphe */
void trierParDate(grapheReseau g) {
    int i, j, n;
    ELEMENT cle;
    ELEMENT *tab;

    tab = (ELEMENT *) malloc(g->V * sizeof(ELEMENT));
    if (tab == NULL) { printf("erreur malloc\n"); return; }

    /* remplir le tableau avec les articles existants */
    n = 0;
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE)
            tab[n++] = g->articles[i];
    }

    /* tri par insertion (du cours) */
    for (i = 1; i < n; i++) {
        cle = tab[i];
        j = i - 1;
        while (j >= 0 && comparerDates(tab[j], cle) > 0) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = cle;
    }

    printf("Articles tries par date de publication :\n");
    for (i = 0; i < n; i++) {
        printf("  %d. %s (%02d/%02d/%04d %02dh%02d)\n",
               i + 1, tab[i]->titre,
               tab[i]->jour, tab[i]->mois, tab[i]->annee,
               tab[i]->heure, tab[i]->minute);
    }

    free(tab);
}

/* afficher le premier article (le plus ancien) qui cite idDest */
void premierCitant(grapheReseau g, int idDest) {
    int i, pos;
    ELEMENT e, premier;

    if (g->articles[idDest] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idDest);
        return;
    }

    premier = ELEMENT_VIDE;

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;
        for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
            e = recuperer(g->adjList[i], pos);
            if (e != NULL && e->id == idDest) {
                /* on garde le plus ancien */
                if (premier == ELEMENT_VIDE || comparerDates(g->articles[i], premier) < 0)
                    premier = g->articles[i];
                break;
            }
        }
    }

    if (premier == ELEMENT_VIDE)
        printf("aucun article ne cite %s\n", g->articles[idDest]->titre);
    else
        printf("Premier article citant %s :\n  --> %s (%02d/%02d/%04d %02dh%02d)\n",
               g->articles[idDest]->titre, premier->titre,
               premier->jour, premier->mois, premier->annee,
               premier->heure, premier->minute);
}

/*
    chaine de propagation depuis idSrc :
    on affiche les articles dans l'ordre chronologique
    qui citent un article deja dans la chaine.
    
    principe : au debut seul idSrc est dans la chaine.
    A chaque etape on cherche l'article le plus ancien
    qui cite quelqu'un deja dans la chaine, on l'ajoute, 
    et on recommence jusqu'a ne plus en trouver.
*/
void chainePropagation(grapheReseau g, int idSrc)
{
    int  i, pos;
    int *dansChaine;
    ELEMENT prochain;
    int procId;

    if (!g || idSrc < 0 || idSrc >= g->V || g->articles[idSrc] == ELEMENT_VIDE) {
        printf("Article %d introuvable.\n", idSrc);
        return;
    }

    dansChaine = (int *)calloc(g->V, sizeof(int));
    if (!dansChaine) { printf("Erreur d'allocation.\n"); return; }

    dansChaine[idSrc] = 1;

    printf("Chaine de propagation depuis '%s' :\n", g->articles[idSrc]->titre);
    printf("  %s (%02d/%02d/%04d %02dh%02d)\n",
           g->articles[idSrc]->titre,
           g->articles[idSrc]->jour,  g->articles[idSrc]->mois,
           g->articles[idSrc]->annee,
           g->articles[idSrc]->heure, g->articles[idSrc]->minute);

    do {
        prochain = ELEMENT_VIDE;
        procId   = -1;

        /*
         * On parcourt TOUS les articles du graphe.
         * Pour chaque article i pas encore dans la chaine,
         * on regarde sa liste de citations (adjList[i]).
         * S'il cite au moins un article deja dans la chaine,
         * il est candidat a rejoindre la chaine.
         * On garde le candidat a la date la plus ancienne.
         */
        for (i = 0; i < g->V; i++) {
            if (dansChaine[i] || g->articles[i] == ELEMENT_VIDE) continue;

            /* Verifier si l'article i cite quelqu'un deja dans la chaine */
            for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
                ELEMENT cite = recuperer(g->adjList[i], pos);
                if (cite == NULL || cite->id < 0 || cite->id >= g->V) continue;

                if (dansChaine[cite->id]) {
                    /* i cite quelqu'un dans la chaine : i est candidat */
                    if (prochain == ELEMENT_VIDE ||
                        comparerDates(g->articles[i], prochain) < 0) {
                        prochain = g->articles[i];
                        procId   = i;
                    }
                    break; /* inutile de continuer pour cet article i */
                }
            }
        }

        if (procId >= 0) {
            dansChaine[procId] = 1;
            printf("      --> cite par : %s (%02d/%02d/%04d %02dh%02d)\n",
                   prochain->titre,
                   prochain->jour,  prochain->mois,  prochain->annee,
                   prochain->heure, prochain->minute);
        }

    } while (procId >= 0);

    free(dansChaine);
}


/* ============================================================
   SECTION 4 : simulation de propagation BFS
   
   On utilise le TDA LISTE comme file FIFO :
     - enfiler en queue : inserer(file, e, listeTaille(file)+1)
     - defiler en tete  : recuperer(file, 1) + supprimer(file, 1)
   
   Le graphe est "inverse" ici : si A cite B, on va de B vers A.
   Donc depuis une source, on trouve tous ceux qui la citent,
   puis ceux qui citent ces citants, etc. (propagation de la rumeur)
   ============================================================ */

void simulerPropagation(grapheReseau g, int idSrc) {
    int *visite, *niveaux;
    int pos, niv, nbAtteints, nivMax;
    ELEMENT courant, e;
    LISTE file;

    if (g->articles[idSrc] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idSrc);
        return;
    }

    visite  = (int *) calloc(g->V, sizeof(int));
    niveaux = (int *) calloc(g->V, sizeof(int));
    if (visite == NULL || niveaux == NULL) {
        printf("erreur malloc\n");
        free(visite); free(niveaux);
        return;
    }

    file = listeCreer();

    inserer(file, g->articles[idSrc], 1);
    visite[idSrc]  = 1;
    niveaux[idSrc] = 0;

    nbAtteints = 0;
    nivMax     = 0;

    printf("Simulation BFS depuis %s :\n\n", g->articles[idSrc]->titre);


    while (!estVide(file)) {
        courant = recuperer(file, 1);
        supprimer(file, 1);

        niv = niveaux[courant->id];
        if (niv > nivMax) nivMax = niv;

        printf("  Niveau %d : %s\n", niv, courant->titre);
        nbAtteints++;

        /* enfiler les articles directement cites par courant */
        /* chercher les articles qui citent courant */

        /********************************************/
        for (int i = 0; i < g->V; i++) {
            if (!visite[i]) {
                for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {

                    e = recuperer(g->adjList[i], pos);

                    if (e != NULL && e->id == courant->id) {

                        inserer(file, g->articles[i], listeTaille(file) + 1);
                        visite[i] = 1;
                        niveaux[i] = niv + 1;
                    }
                }
            }
        }/********************************************/
    }

    listeDetruire(file);
    printf("\n  %d niveau(x) de propagation, %d article(s) atteint(s)\n",
           nivMax + 1, nbAtteints);

    free(visite);
    free(niveaux);
}

/* afficher tous les articles accessibles depuis idSrc (BFS simple) */
void articlesAccessibles(grapheReseau g, int idSrc) {
    int *visite;
    int pos;
    ELEMENT courant, e;
    LISTE file;

    if (g->articles[idSrc] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idSrc);
        return;
    }

    visite = (int *) calloc(g->V, sizeof(int));
    if (visite == NULL) { printf("erreur malloc\n"); return; }

    file = listeCreer();
    inserer(file, g->articles[idSrc], 1);
    visite[idSrc] = 1;

    printf("Articles accessibles depuis %s :\n", g->articles[idSrc]->titre);

    while (!estVide(file)) {
        courant = recuperer(file, 1);
        supprimer(file, 1);
        printf("  - %s\n", courant->titre);

        /* suivre les citations directes de courant */
        for (pos = 1; pos <= listeTaille(g->adjList[courant->id]); pos++) {
            e = recuperer(g->adjList[courant->id], pos);
            if (e != NULL && !visite[e->id]) {
                inserer(file, g->articles[e->id], listeTaille(file) + 1);
                visite[e->id] = 1;
            }
        }
    }

    listeDetruire(file);
    free(visite);
}


/* ============================================================
   SECTION 5 : detection naive de fausses informations
   ============================================================ */

/*
    convertir une chaine en minuscules dans buf
    (on ne modifie pas l'original)
*/
void versMinuscules(const char *src, char *buf, int taille) {
    int i;
    for (i = 0; i < taille - 1 && src[i] != '\0'; i++)
        buf[i] = (char) tolower((unsigned char) src[i]);
    buf[i] = '\0';
}

/*
    remplacer les underscores par des espaces dans buf
*/
void underscoresEnEspaces(const char *src, char *buf, int taille) {
    int i;
    for (i = 0; i < taille - 1 && src[i] != '\0'; i++)
        buf[i] = (src[i] == '_') ? ' ' : src[i];
    buf[i] = '\0';
}

/*
    analyser un article : calculer son score de suspicion
    et mettre a jour score_fiabilite
    
    regles :
      +40 si le titre contient un fragment de BASE_FAKES
      +10 par mot de MOTS_SUSPECTS trouve dans le titre
    
    score_fiabilite = max(0, 100 - score_suspicion)
*/
int analyserArticle(ELEMENT art) {
    char tmp[100], titre_min[100];
    int i, score_susp;

    if (art == NULL) return 0;

    /* preparer le titre : underscores -> espaces -> minuscules */
    underscoresEnEspaces(art->titre, tmp, sizeof(tmp));
    versMinuscules(tmp, titre_min, sizeof(titre_min));

    score_susp = 0;

    /* verifier les phrases suspectes (+40 max une seule fois) */
    for (i = 0; i < NB_FAKES; i++) {
        if (strstr(titre_min, BASE_FAKES[i]) != NULL) {
            score_susp += 40;
            break;
        }
    }

    /* verifier les mots suspects (+10 chacun) */
    for (i = 0; i < NB_SUSPECTS; i++) {
        if (strstr(titre_min, MOTS_SUSPECTS[i]) != NULL)
            score_susp += 10;
    }

    /* mise a jour du score de fiabilite */
    art->score_fiabilite = 100 - score_susp;
    if (art->score_fiabilite < 0)   art->score_fiabilite = 0;
    if (art->score_fiabilite > 100) art->score_fiabilite = 100;

    return score_susp;
}

/* analyser tous les articles et afficher un rapport */
void analyserReseau(grapheReseau g) {
    int i, j, score_susp;
    char tmp[100], titre_min[100];
    const char *cat;
    ELEMENT art;

    printf("\n===== Rapport d'analyse fake news =====\n\n");

    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;

        art        = g->articles[i];
        score_susp = analyserArticle(art);

        if      (art->score_fiabilite < 40) cat = "SUSPECT";
        else if (art->score_fiabilite < 70) cat = "DOUTEUX";
        else                                 cat = "FIABLE ";

        printf("  [%s] %s (score : %d)\n", cat, art->titre, art->score_fiabilite);

        /* afficher ce qui a declenche la suspicion */
        if (score_susp > 0) {
            underscoresEnEspaces(art->titre, tmp, sizeof(tmp));
            versMinuscules(tmp, titre_min, sizeof(titre_min));

            for (j = 0; j < NB_FAKES; j++) {
                if (strstr(titre_min, BASE_FAKES[j]) != NULL)
                    printf("          -> phrase : \"%s\"\n", BASE_FAKES[j]);
            }
            for (j = 0; j < NB_SUSPECTS; j++) {
                if (strstr(titre_min, MOTS_SUSPECTS[j]) != NULL)
                    printf("          -> mot : \"%s\"\n", MOTS_SUSPECTS[j]);
            }
        }
    }
    printf("\n=======================================\n");
}

/* afficher les articles suspects (score < 40) qui sont cites,
   tries par degre_in decroissant (tri a bulles simple) */
void articlesSuspectsCites(grapheReseau g) {
    int i, j, n;
    ELEMENT *tab;
    int *degs;
    ELEMENT tmp_e;
    int tmp_d;

    tab  = (ELEMENT *) malloc(g->V * sizeof(ELEMENT));
    degs = (int *)     malloc(g->V * sizeof(int));
    if (tab == NULL || degs == NULL) {
        printf("erreur malloc\n");
        free(tab); free(degs);
        return;
    }

    n = 0;
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;
        if (g->articles[i]->score_fiabilite < 40 && g->degre_in[i] > 0) {
            tab[n]  = g->articles[i];
            degs[n] = g->degre_in[i];
            n++;
        }
    }

    /* tri a bulles par degre_in decroissant */
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (degs[j] < degs[j + 1]) {
                tmp_e     = tab[j];    tab[j]    = tab[j+1];  tab[j+1] = tmp_e;
                tmp_d     = degs[j];   degs[j]   = degs[j+1]; degs[j+1] = tmp_d;
            }
        }
    }

    printf("Articles suspects les plus cites :\n");
    if (n == 0)
        printf("  (aucun)\n");
    else {
        for (i = 0; i < n; i++)
            printf("  --> %s (score:%d, cite par %d article(s))\n",
                   tab[i]->titre, tab[i]->score_fiabilite, degs[i]);
    }

    free(tab);
    free(degs);
}


/* ============================================================
   SECTION 6 : analyse de robustesse (BONUS)
   ============================================================ */

/*
    fonction auxiliaire : BFS depuis idSrc
    retourne un tableau visite[] (a liberer par l'appelant)
    visite[i] = 1 si on peut atteindre i depuis idSrc
    
    ici on suit les citations INVERSES (propagation rumeur)
*/
int *bfsVisite(grapheReseau g, int idSrc) {
    int *visite;
    int i, pos;
    ELEMENT courant, e;
    LISTE file;

    visite = (int *) calloc(g->V, sizeof(int));
    if (visite == NULL) return NULL;
    if (g->articles[idSrc] == ELEMENT_VIDE) return visite;

    file = listeCreer();
    inserer(file, g->articles[idSrc], 1);
    visite[idSrc] = 1;

    while (!estVide(file)) {
        courant = recuperer(file, 1);
        supprimer(file, 1);

        for (i = 0; i < g->V; i++) {
            if (g->articles[i] == ELEMENT_VIDE || visite[i]) continue;
            for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
                e = recuperer(g->adjList[i], pos);
                if (e != NULL && e->id == courant->id) {
                    inserer(file, g->articles[i], listeTaille(file) + 1);
                    visite[i] = 1;
                    break;
                }
            }
        }
    }

    listeDetruire(file);
    return visite;
}

/*
    verifier s'il existe un chemin direct de idSrc vers idDest
    (en suivant le sens des citations : src -> ... -> dest)
*/
int cheminExiste(grapheReseau g, int idSrc, int idDest) {
    int *visite;
    int pos, res;
    ELEMENT courant, e;
    LISTE file;

    if (g->articles[idSrc] == ELEMENT_VIDE ||
        g->articles[idDest] == ELEMENT_VIDE) return 0;

    visite = (int *) calloc(g->V, sizeof(int));
    if (visite == NULL) return 0;

    file = listeCreer();
    inserer(file, g->articles[idSrc], 1);
    visite[idSrc] = 1;
    res = 0;

    while (!estVide(file) && !res) {
        courant = recuperer(file, 1);
        supprimer(file, 1);

        if (courant->id == idDest) { res = 1; break; }

        /* on suit ici les citations directes (adjList) */
        for (pos = 1; pos <= listeTaille(g->adjList[courant->id]); pos++) {
            e = recuperer(g->adjList[courant->id], pos);
            if (e != NULL && !visite[e->id]) {
                inserer(file, e, listeTaille(file) + 1);
                visite[e->id] = 1;
            }
        }
    }

    listeDetruire(file);
    free(visite);
    return res;
}

/* simuler la suppression d'un article et afficher l'impact sur le reseau */
void simulerSuppression(grapheReseau g, int idArt) {
    int i, pos, nb_citants, nb_cites, deconnectes;
    ELEMENT e;
    int *avant, *apres;

    if (g->articles[idArt] == ELEMENT_VIDE) {
        printf("article %d introuvable\n", idArt);
        return;
    }

    printf("Simulation de suppression de '%s' :\n", g->articles[idArt]->titre);

    /* compter et afficher les articles qui le citent */
    nb_citants = g->degre_in[idArt];
    printf("  - Etait cite par %d article(s) :\n", nb_citants);
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] == ELEMENT_VIDE) continue;
        for (pos = 1; pos <= listeTaille(g->adjList[i]); pos++) {
            e = recuperer(g->adjList[i], pos);
            if (e != NULL && e->id == idArt) {
                printf("      %s\n", g->articles[i]->titre);
                break;
            }
        }
    }

    /* compter et afficher les articles qu'il cite */
    nb_cites = listeTaille(g->adjList[idArt]);
    printf("  - Citait %d article(s) :\n", nb_cites);
    for (pos = 1; pos <= nb_cites; pos++) {
        e = recuperer(g->adjList[idArt], pos);
        if (e != NULL) printf("      %s\n", e->titre);
    }

    /* calculer les articles accessibles avant suppression
       on part de toutes les sources (degre_in == 0) */
    avant = (int *) calloc(g->V, sizeof(int));
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && g->degre_in[i] == 0) {
            int *v = bfsVisite(g, i);
            if (v != NULL) {
                int j;
                for (j = 0; j < g->V; j++) avant[j] |= v[j];
                free(v);
            }
        }
    }

    /* effectuer la suppression */
    supprimerArticle(g, idArt);

    /* recalculer apres suppression */
    apres = (int *) calloc(g->V, sizeof(int));
    for (i = 0; i < g->V; i++) {
        if (g->articles[i] != ELEMENT_VIDE && g->degre_in[i] == 0) {
            int *v = bfsVisite(g, i);
            if (v != NULL) {
                int j;
                for (j = 0; j < g->V; j++) apres[j] |= v[j];
                free(v);
            }
        }
    }

    /* afficher les articles devenus inaccessibles */
    printf("  - Articles deconnectes apres suppression :\n");
    deconnectes = 0;
    for (i = 0; i < g->V; i++) {
        if (i == idArt) continue;
        if (avant[i] && !apres[i] && g->articles[i] != ELEMENT_VIDE) {
            printf("      %s\n", g->articles[i]->titre);
            deconnectes++;
        }
    }
    if (!deconnectes) printf("      (aucun)\n");

    free(avant);
    free(apres);
}

/*
    neutraliser la propagation entre idSrc et idDest :
    supprimer le minimum d'articles intermediaires pour
    qu'il n'existe plus aucun chemin entre les deux.
    
    strategie : a chaque etape on supprime l'intermediaire
    le moins fiable (score_fiabilite le plus faible)
    jusqu'a ce qu'il n'y ait plus de chemin.
*/
int neutraliserPropagation(grapheReseau g, int idSrc, int idDest) {
    int nb, cible, i, scoreMin;

    if (g->articles[idSrc] == ELEMENT_VIDE ||
        g->articles[idDest] == ELEMENT_VIDE) {
        printf("article(s) introuvable(s)\n");
        return 0;
    }

    printf("Neutralisation de la propagation de A%d vers A%d :\n", idSrc, idDest);
    nb = 0;

    while (cheminExiste(g, idSrc, idDest)) {
        /* trouver l'intermediaire avec le score le plus bas */
        cible    = -1;
        scoreMin = 101;

        for (i = 0; i < g->V; i++) {
            if (i == idSrc || i == idDest) continue;
            if (g->articles[i] == ELEMENT_VIDE) continue;
            if (g->articles[i]->score_fiabilite < scoreMin) {
                scoreMin = g->articles[i]->score_fiabilite;
                cible    = i;
            }
        }

        if (cible < 0) break;

        printf("  Article supprime : %s (score:%d)\n",
               g->articles[cible]->titre, g->articles[cible]->score_fiabilite);
        supprimerArticle(g, cible);
        nb++;
    }

    if (!cheminExiste(g, idSrc, idDest))
        printf("Plus aucun chemin de A%d vers A%d.\n", idSrc, idDest);
    else
        printf("Impossible de couper le chemin sans toucher source ou destination.\n");

    printf("Nombre d'articles supprimes : %d\n", nb);
    return nb;
}
