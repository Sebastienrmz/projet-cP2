#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char* prog) {
    fprintf(stderr, "Usage: %s <data.txt> [--check]\n", prog);
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(argv[0]); return EXIT_FAILURE; }

    const char* fichier = argv[1];
    int check = (argc >= 3 && strcmp(argv[2], "--check") == 0);

    adj* g = readGraph(fichier);

    puts("=== Liste d'adjacence ===");
    adj_afficher(g);

    if (check) {
        puts("\n=== Vérification Markov ===");
        adj_verif_markov(g);
    }

    adj_detruire(g);
    return EXIT_SUCCESS;
}
