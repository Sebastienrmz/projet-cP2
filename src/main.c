#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char* prog) {
    fprintf(stderr,
        "Usage: %s <data.txt> [--check] [--mmd fichier.mmd]\n"
        "Exemples:\n"
        "  %s data/exemple1.txt\n"
        "  %s data/exemple1.txt --check\n"
        "  %s data/exemple1.txt --mmd data/graphe.mmd\n", prog, prog, prog, prog);
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(argv[0]); return EXIT_FAILURE; }

    const char* in_file = argv[1];
    int do_check = 0;
    const char* out_mmd = NULL;

    // Parsing très simple
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--check") == 0) {
            do_check = 1;
        } else if (strcmp(argv[i], "--mmd") == 0 && i + 1 < argc) {
            out_mmd = argv[++i];
        }
    }

    adj* g = readGraph(in_file);

    puts("=== Liste d'adjacence ===");
    adj_afficher(g);

    if (do_check) {
        puts("\n=== Verification du graphe ===");
        adj_verif_markov(g);
    }

    if (out_mmd) {
        if (adj_write_mermaid(g, out_mmd))
            printf("\nFichier Mermaid ecrit : %s\n", out_mmd);
        else
            fprintf(stderr, "Echec d'ecriture du fichier Mermaid.\n");
    }

    adj_detruire(g);
    return EXIT_SUCCESS;
}
