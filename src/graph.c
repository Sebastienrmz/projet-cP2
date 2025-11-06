#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* ========== helpers internes ========== */
static cellule* cellule_creer(int dest, float p, cellule* next) {
    cellule* c = (cellule*)malloc(sizeof(cellule));
    if (!c) { perror("malloc cellule"); exit(EXIT_FAILURE); }
    c->to = dest; c->poids = p; c->next = next;
    return c;
}

/* ========== création / destruction ========== */
adj* adj_creer(size_t n) {
    adj* g = (adj*)malloc(sizeof(adj));
    if (!g) { perror("malloc adj"); exit(EXIT_FAILURE); }
    g->n = n;
    g->L = (liste*)calloc(n, sizeof(liste));      // listes vides
    if (!g->L) { perror("calloc listes"); free(g); exit(EXIT_FAILURE); }
    return g;
}

void adj_detruire(adj* g) {
    if (!g) return;
    for (size_t i = 0; i < g->n; ++i) {
        cellule* cur = g->L[i].head;
        while (cur) { cellule* nxt = cur->next; free(cur); cur = nxt; }
    }
    free(g->L);
    free(g);
}

/* ========== listes ========== */
void liste_ajouter_tete(liste* li, int dest, float p) {
    li->head = cellule_creer(dest, p, li->head);  // insertion en tête (simple)
}

/* ========== lecture fichier (format du sujet) ========== */
/* Ligne 1 : N
 * Lignes suivantes : depart arrivee poids
 * (cf. diapo "Lecture des fichiers de données")                                         */
adj* readGraph(const char* filename) {                                                /* :contentReference[oaicite:1]{index=1} */
    FILE* f = fopen(filename, "rt");
    if (!f) { perror(filename); exit(EXIT_FAILURE); }

    int N = 0;
    if (fscanf(f, "%d", &N) != 1 || N <= 0) {
        fprintf(stderr, "Nombre de sommets invalide dans %s\n", filename);
        fclose(f); exit(EXIT_FAILURE);
    }

    adj* g = adj_creer((size_t)N);

    int u, v; float p;
    while (fscanf(f, "%d %d %f", &u, &v, &p) == 3) {
        if (u < 1 || v < 1 || u > N || v > N) {
            fprintf(stderr, "Arête ignorée (%d -> %d) hors bornes [1,%d]\n", u, v, N);
            continue;
        }
        liste_ajouter_tete(&g->L[u - 1], v, p);
    }

    fclose(f);
    return g;
}

/* ========== affichage ========== */
/* Format d’affichage “liste d’adjacence” comme dans le cours                         */
void adj_afficher(const adj* g) {
    if (!g) return;
    for (size_t i = 0; i < g->n; ++i) {
        printf("Liste pour le sommet %zu: [head] ", i + 1);
        for (cellule* c = g->L[i].head; c; c = c->next) {
            printf("-> (%d, %.2f) ", c->to, c->poids);
        }
        printf("\n");
    }
}

/* ========== Étape 2 (optionnel) : vérification Markov ========== */
/* somme des probabilités sortantes de chaque sommet ≈ 1 (tolérance simple)            */
bool adj_verif_markov(const adj* g) {
    if (!g) return false;
    const float MIN = 0.99f, MAX = 1.00f + 1e-6f;
    bool ok = true;
    for (size_t i = 0; i < g->n; ++i) {
        float s = 0.0f;
        for (cellule* c = g->L[i].head; c; c = c->next) s += c->poids;
        if (s < MIN || s > MAX) {
            printf("Sommet %zu invalide : somme = %.2f\n", i + 1, s);
            ok = false;
        }
    }
    if (ok) printf("Le graphe est un graphe de Markov.\n");
    return ok;
}
