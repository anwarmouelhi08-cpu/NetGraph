# NetGraph — Détection de Propagation de Fake News

## 📋 Description

**NetGraph** est un système de modélisation et d'analyse de réseaux d'articles et de citations, conçu pour détecter et tracer les chaînes de propagation de fausses informations (fake news) au sein d'un réseau d'information. Le projet implémente un **graphe orienté** représentant les relations de citation entre articles, permettant d'identifier les sources suspectes, les patterns de propagation et les articles les plus influents.

---

## 🎯 Fonctionnalités Principales

Le système offre un menu interactif complet pour gérer et analyser un réseau d'information :

### Gestion du Réseau
1. **Charger un réseau depuis fichier** — Importe des articles et citations depuis un fichier de données
2. **Ajouter un article** — Crée un nouvel article avec ses métadonnées (titre, source, score de fiabilité, date/heure)
3. **Ajouter une citation** — Établit une relation de citation entre deux articles
4. **Supprimer un article** — Supprime un article et toutes ses citations associées
5. **Supprimer une citation** — Supprime une relation de citation entre deux articles

### Interrogation du Réseau
6. **Afficher le réseau** — Affiche tous les articles et leurs citations
7. **Articles cités par un article** — Liste les articles qu'un article donné cite
8. **Articles qui citent un article** — Liste les articles qui citent un article donné
9. **Sources originales / articles isolés** — Identifie les articles sans citations entrantes (sources) et les articles sans connexions
10. **Article le plus cité** — Trouve l'article ayant le plus de citations entrantes

### Chronologie et Propagation
11. **Trier par date de publication** — Ordonne les articles par date/heure
12. **Premier article citant** — Trouve le premier article (chronologiquement) qui cite un article donné
13. **Chaîne de propagation** — Affiche le chemin d'une fausse information à travers le réseau
14. **Simuler la propagation (BFS)** — Simule par parcours en largeur (BFS) la diffusion d'une information

### Analyse et Recherche
15. **Articles accessibles depuis un article** — Trouve tous les articles atteignables à partir d'une source
16. **Analyser les articles (fake news)** — Évalue chaque article pour détecter les contenus suspects
17. **Articles suspects les plus cités** — Identifie les fake news les plus influentes dans le réseau

### Fonctionnalités Bonus
18. **[BONUS] Simuler la suppression d'un article** — Teste l'impact de la suppression d'un article sur la connexité du réseau
19. **[BONUS] Neutraliser une propagation** — Calcule les chemins de propagation entre deux articles et propose une neutralisation

---

## 📁 Structure des Fichiers

| Fichier | Description |
|---------|-------------|
| `main.c` | Programme principal contenant le menu interactif et la boucle d'interaction utilisateur |
| `graphe.c` / `graphe.h` | Implémentation du graphe orienté par liste d'adjacence ; gère les articles, citations et tous les algorithmes d'analyse |
| `LSTPRIM.c` / `LSTPRIM.h` | Implémentation du TDA **LISTE** (liste simplement chaînée) selon le cours ASD2 ; utilisée pour stocker la liste d'adjacence |
| `ELTARTICLE.c` / `ELTARTICLE.h` | Implémentation du TDA **ELEMENT** représentant un article avec ses métadonnées (id, titre, source, score de fiabilité, date/heure) |
| `fakesdb.h` | Base de données pour la détection naive de fake news : contient des mots-clés suspects et des expressions de fake news courantes |
| `interface.html` | Interface graphique HTML de visualisation du réseau (optionnel, à consulter dans un navigateur) |
| `data.txt` | Fichier de données de test contenant des articles et des citations au format spécifié |
| `LICENSE` | Licence du projet |

---

## 🔧 Compilation et Exécution

### Sous Windows

#### Compilation
Compilez tous les fichiers `.c` avec GCC :

```bash
gcc.exe -o NetGraph.exe main.c graphe.c LSTPRIM.c ELTARTICLE.c -Wall -Wextra
```

**Options** :
- `-o NetGraph.exe` : Produit l'exécutable `NetGraph.exe`
- `-Wall -Wextra` : Affiche les avertissements (optionnel mais recommandé)

#### Exécution
```bash
NetGraph.exe
```

Un menu interactif s'affiche, permettant de charger un réseau et d'effectuer les opérations listées ci-dessus.

### Exemple d'Utilisation

```bash
# Compilation
gcc.exe -o NetGraph.exe main.c graphe.c LSTPRIM.c ELTARTICLE.c

# Exécution
NetGraph.exe

# Menu :
===== Reseau d'Information =====
 1.  Charger un reseau depuis un fichier
 2.  Ajouter un article
 ...
 0.  Quitter
==================================
Votre choix : 1
Nom du fichier : data.txt
graphe charge : 5 articles
```

---

## 📊 Structure de Données

### Graphe Orienté par Liste d'Adjacence

Le réseau est implémenté comme un **graphe orienté** où :
- **Chaque nœud** représente un **article**
- **Une arête A → B** signifie **"l'article A cite l'article B"**

**Représentation en mémoire** (structure `grapheReseauStruct`) :
- `V` : Nombre de nœuds (articles)
- `articles[]` : Tableau des articles (indexé par id)
- `adjList[]` : Tableau de listes d'adjacence (adjList[i] = liste des articles cités par l'article i)
- `degre_in[]` : Tableau des degrés entrants (nombre d'articles qui citent chaque article)

**Avantages** :
- Efficace pour les opérations de parcours et traversée
- Adapté aux réseaux peu denses
- Permet des algorithmes de pathfinding et de propagation (BFS)

### TDA LISTE — Liste Simplement Chaînée

La liste d'adjacence utilise une **liste simplement chaînée** classique :
- **Insertion/Suppression** : O(n)
- **Accès** : O(n)
- **Parcours** : O(n)

Cette structure est utilisée pour stocker les voisins de chaque article dans le graphe.

### TDA ELEMENT — Article d'Information

Chaque article est un **ELEMENT** (structure `articleStruct`) contenant :
- `id` : Identifiant unique de l'article
- `titre` : Titre de l'article (jusqu'à 99 caractères)
- `source` : Source de publication (jusqu'à 49 caractères)
- `score_fiabilite` : Score de 0 à 100 indiquant le degré de confiance
- `jour`, `mois`, `annee` : Date de publication
- `heure`, `minute` : Heure de publication

---

## 📄 Format du Fichier de Données

Les fichiers de configuration (comme `data.txt`) suivent un format texte simple :

```
# Format : A id "titre" source score jour mois annee heure minute
#          C id_src id_dest
#          # commentaire

A 0 "La_verite_sur_le_vaccin_alerte_secret_choc" page_TunisieActu 8 15 3 2025 9 0
A 1 "Etude_exclusif_urgent_OMS_choc_complot" Kapitalis 45 15 3 2025 8 0
A 2 "Partagez_avant_suppression_urgent_choc_alerte" TunisieNumerique 5 15 3 2025 11 0

C 0 1
C 0 3
C 1 3
```

**Syntaxe** :
- **Lignes article** : `A id "titre" source score jour mois annee heure minute`
  - `A` : Préfixe obligatoire
  - `id` : Entier unique
  - `titre` : Chaîne entre guillemets (max 99 caractères)
  - `source` : Chaîne sans espaces (max 49 caractères)
  - `score` : Entier 0-100
  - `jour` : 1-31, `mois` : 1-12, `annee` : 1900-2100
  - `heure` : 0-23, `minute` : 0-59

- **Lignes citation** : `C id_src id_dest`
  - `C` : Préfixe obligatoire
  - `id_src` : ID de l'article citant
  - `id_dest` : ID de l'article cité

- **Commentaires** : `# texte`

---

## 🌐 Interface HTML

Un fichier `interface.html` est fourni pour visualiser le réseau de façon graphique. 

**Utilisation** :
1. Ouvrez le fichier `interface.html` dans un navigateur web (Firefox, Chrome, Edge, Safari)
2. L'interface affiche le réseau avec un design moderne et des couleurs thématisées

**Remarque** : Cette interface est actuellement statique et peut nécessiter des améliorations pour l'intégration dynamique avec le programme C.

---

## 🔍 Détection de Fake News

Le système utilise une **approche naïve de détection** basée sur des mots-clés suspects (définis dans `fakesdb.h`) :

### Expressions de Fake News
- "ils vous cachent"
- "partagez avant suppression"
- "ce qu on vous cache"
- "la verite sur"
- "les medias ne disent pas"
- "wake up"

### Mots Suspects
- alerte, urgent, exclusif
- censure, complot, secret
- interdit, choc

Un article est considéré comme suspect si :
1. Son titre contient une ou plusieurs expressions de fake news
2. Son titre contient plusieurs mots suspects
3. Son score de fiabilité est très bas (< 30)

**Limitation** : Cette détection est basique et peut générer des faux positifs. Une approche plus sophistiquée (ML, analyse de crédibilité, etc.) serait nécessaire pour un système de production.

---

## 📝 Exemple de Workflow

```
1. Compilation du projet
2. Exécution : NetGraph.exe
3. Choix 1 : Charger data.txt
4. Choix 16 : Analyser les articles (fake news)
   → Identifie les articles suspects
5. Choix 14 : Simuler la propagation (BFS) depuis article 0
   → Trace le chemin de diffusion
6. Choix 17 : Articles suspects les plus cités
   → Trouve les fake news les plus influentes
7. Choix 19 : Neutraliser une propagation entre deux articles
   → Calcule les chemins à couper
```

---

## 👤 Auteur(s)

Projet réalisé dans le cadre du cours **ASD2 (Algorithmique et Structures de Données 2)** — Année académique 2025/2026.

---

## 📄 Licence

Voir le fichier [LICENSE](LICENSE) pour les détails.