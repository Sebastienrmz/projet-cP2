
#ifndef PROJET_CP2_GRAPH_H
#define PROJET_CP2_GRAPH_H

#endif //PROJET_CP2_GRAPH_H

#ifndef TI301_GRAPH_H
#define TI301_GRAPH_H

#include <stddef.h>
#include <stdbool.h>

/* -------- structures du cours --------
 * cellule : une arête sortante (destination + poids + lien suivant)
 * liste   : liste simplement chaînée de cellules
 * adj     : tableau dynamique de listes (une par sommet)
 */

typedef struct cellule {
    int to;                 // sommet d'arrivée
    float poids;            // poids/probabilité (0 < p <= 1)
    struct cellule* next;   // cellule suivante
} cellule;

typedef struct {
    cellule* head;          // tête de liste (NULL si vide)
} liste;

typedef struct {
    size_t n;               // nombre de sommets
    liste* L;               // L[i] = liste des arêtes sortantes du sommet i+1
} adj;

/* ---- création / destruction ---- */
adj*   adj_creer(size_t n);
void   adj_detruire(adj* g);

/* ---- opérations sur les listes ---- */
void   liste_ajouter_tete(liste* li, int dest, float p);

/* ---- I/O (format du sujet) ---- */
adj*   readGraph(const char* filename);

/* ---- affichage ---- */
void   adj_afficher(const adj* g);

/* ---- Étape 2 (optionnel) : vérif Markov ---- */
bool   adj_verif_markov(const adj* g);

#endif
