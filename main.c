/*
    MAIN.C
    programme principal - menu interactif
    projet ASD2 2025/2026
*/

#include <stdio.h>
#include <stdlib.h>
#include "graphe.h"
/************/
int lireEntier(int *x)
{
    while (scanf("%d", x) != 1) {
        printf("Entrée invalide, recommencez : ");
        while (getchar() != '\n');
    }
    return 1;
}/************/

void afficherMenu() {
    printf("\n===== Reseau d'Information =====\n");
    printf(" 1.  Charger un reseau depuis un fichier\n");
    printf(" 2.  Ajouter un article\n");
    printf(" 3.  Ajouter une citation\n");
    printf(" 4.  Supprimer un article\n");
    printf(" 5.  Supprimer une citation\n");
    printf(" 6.  Afficher le reseau\n");
    printf(" 7.  Articles cites par un article\n");
    printf(" 8.  Articles qui citent un article\n");
    printf(" 9.  Sources originales / articles isoles\n");
    printf(" 10. Article le plus cite\n");
    printf(" 11. Trier par date de publication\n");
    printf(" 12. Premier article citant\n");
    printf(" 13. Chaine de propagation\n");
    printf(" 14. Simuler la propagation (BFS)\n");
    printf(" 15. Articles accessibles depuis un article\n");
    printf(" 16. Analyser les articles (fake news)\n");
    printf(" 17. Articles suspects les plus cites\n");
    printf(" 18. [BONUS] Simuler la suppression d'un article\n");
    printf(" 19. [BONUS] Neutraliser une propagation\n");
    printf(" 0.  Quitter\n");
    printf("==================================\n");
    printf("Votre choix : ");
}

int main() {
    grapheReseau g = NULL;
    int choix, id, idSrc, idDest;
    char filename[100];
    ELEMENT art;

    do {
        afficherMenu();
        lireEntier(&choix);
        switch (choix) {

        case 1:
            /* charger depuis fichier */
            if (g != NULL) {
                detruireGraphe(g);
                g = NULL;
            }
            printf("Nom du fichier : ");
            scanf("%s", filename);
            g = chargerGraphe(filename);
            if (g == NULL) printf("Echec du chargement.\n");
            break;

        case 2:
            /* ajouter un article */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            art = elementCreer();
            printf("id : "); lireEntier(&art->id);
            printf("score (0-100) : "); lireEntier(&art->score_fiabilite);

            printf("jour mois annee : ");
            lireEntier(&art->jour);
            lireEntier(&art->mois);
            lireEntier(&art->annee);

            printf("heure minute : ");
            lireEntier(&art->heure);
            lireEntier(&art->minute);
            if (!ajouterArticle(g, art)) {
                elementDetruire(art);
                printf("Echec ajout article.\n");
}
            break;

        case 3:
            /* ajouter une citation */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id source : ");

            /**************/
            if (!lireEntier(&idSrc)) {
                printf("Entrée invalide.\n");
                break;
            }

            printf("id destination : ");
            if (!lireEntier(&idDest)) {
                printf("Entrée invalide.\n");
                break;
            }
            /**************/
            ajouterCitation(g, idSrc, idDest);
            break;

        case 4:
            /* supprimer un article */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article a supprimer : "); lireEntier(&id);
            supprimerArticle(g, id);
            break;

        case 5:
            /* supprimer une citation */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id source : ");      lireEntier(&idSrc);
            printf("id destination : "); lireEntier(&idDest);
            supprimerCitation(g, idSrc, idDest);
            break;

        case 6:
            /* afficher le graphe */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            afficherGraphe(g);
            break;

        case 7:
            /* articles cites par un article */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article : "); lireEntier(&id);;
            articlesCites(g, id);
            break;

        case 8:
            /* articles qui citent un article */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article : "); lireEntier(&id);;
            articlesCitants(g, id);
            break;

        case 9:
            /* sources originales et articles isoles */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            sourcesOriginales(g);
            printf("\n");
            articlesIsoles(g);
            break;

        case 10:
            /* article le plus cite */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            articlePlusCite(g);
            break;

        case 11:
            /* trier par date */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            trierParDate(g);
            break;

        case 12:
            /* premier citant */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article : "); lireEntier(&id);;
            premierCitant(g, id);
            break;

        case 13:
            /* chaine de propagation */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article source : "); lireEntier(&id);;
            chainePropagation(g, id);
            break;

        case 14:
            /* simuler propagation BFS */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }

            /**************/
            printf("id de l'article source : ");
            if (!lireEntier(&id)) {
                printf("Entrée invalide.\n");
                break;
            }
            simulerPropagation(g, id);/**************/
            break;

        case 15:
            /* articles accessibles */
            /**************/
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }

            printf("id de l'article source : ");
            if (!lireEntier(&id)) {
                printf("Entrée invalide.\n");
                break;
            }

            articlesAccessibles(g, id);
            break;/**************/
        

        case 16:
            /* analyser le reseau (fake news) */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            analyserReseau(g);
            break;

        case 17:
            /* articles suspects les plus cites */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            articlesSuspectsCites(g);
            break;

        case 18:
            /* bonus : simuler suppression */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id de l'article a supprimer : "); lireEntier(&id);;
            simulerSuppression(g, id);
            break;

        case 19:
            /* bonus : neutraliser propagation */
            if (g == NULL) { printf("Aucun graphe charge.\n"); break; }
            printf("id source : ");      lireEntier(&idSrc);
            printf("id destination : "); lireEntier(&idDest);
            neutraliserPropagation(g, idSrc, idDest);
            break;

        case 0:
            printf("Au revoir !\n");
            break;

        default:
            printf("Choix invalide.\n");
            break;
        }

    } while (choix != 0);

    if (g != NULL) detruireGraphe(g);
    return 0;
}
