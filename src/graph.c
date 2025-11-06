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
    li->head = cellule_creer(dest, p, li->head);  // insertion en tête
}

/* ========== lecture fichier (format du sujet) ========== */
adj* readGraph(const char* filename) {
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

/* ========== Étape 2 : vérification Markov (UNE SEULE DÉFINITION) ========== */
bool adj_verif_markov(const adj* g) {
    if (!g) return false;

    const float MIN = 0.99f;
    const float MAX = 1.00f + 1e-6f;   // petite tolérance numérique
    bool ok = true;

    for (size_t i = 0; i < g->n; ++i) {
        float somme = 0.0f;
        for (cellule* c = g->L[i].head; c; c = c->next) {
            somme += c->poids;
        }
        if (somme < MIN || somme > MAX) {
            printf("la somme des probabilites du sommet %zu est %.2f\n", i + 1, somme);
            ok = false;
        }
    }

    if (ok) {
        printf("Le graphe est un graphe de Markov\n");
    } else {
        printf("Le graphe n'est pas un graphe de Markov\n");
    }
    return ok;
}

/* ========== Étape 3 : utilitaire getId ========== */
void getId(int num, char* buffer, size_t buflen) {
    if (!buffer || buflen == 0) return;
    if (num <= 0) { buffer[0] = '?'; buffer[1] = '\0'; return; }

    char tmp[32];
    int k = 0;
    while (num > 0 && k < (int)sizeof(tmp) - 1) {
        int r = (num - 1) % 26;
        tmp[k++] = (char)('A' + r);
        num = (num - 1) / 26;
    }
    tmp[k] = '\0';

    int n = k;
    for (int i = 0; i < n && (size_t)i < buflen - 1; ++i) {
        buffer[i] = tmp[n - 1 - i];
    }
    buffer[(size_t)n < buflen ? n : buflen - 1] = '\0';
}

/* ========== Étape 3 : écriture Mermaid ========== */
int adj_write_mermaid(const adj* g, const char* out_path) {
    if (!g || !out_path) return 0;

    FILE* f = fopen(out_path, "wt");
    if (!f) { perror(out_path); return 0; }

    fprintf(f,
        "---\n"
        "config:\n"
        "  layout: elk\n"
        "  theme: neo\n"
        "  look: neo\n"
        "---\n\n"
        "flowchart LR\n");

    // nœuds
    for (size_t i = 0; i < g->n; ++i) {
        char id[8];
        getId((int)(i + 1), id, sizeof id);
        fprintf(f, "%s((%zu))\n", id, i + 1);
    }
    fprintf(f, "\n");

    // arêtes
    for (size_t u = 0; u < g->n; ++u) {
        char idu[8];
        getId((int)(u + 1), idu, sizeof idu);
        for (cellule* c = g->L[u].head; c; c = c->next) {
            char idv[8];
            getId(c->to, idv, sizeof idv);
            fprintf(f, "%s -->|%.2f|%s\n", idu, c->poids, idv);
        }
    }

    fclose(f);
    return 1;
}
