#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

void menu()
{
    printf("\nMenu :\n");
    printf(" 1 - Afficher la liste d'adjacence\n");
    printf(" 2 - Verifier si le graphe est de Markov\n");
    printf(" 3 - Generer graph.mmd\n");
    printf(" 4 - Tarjan et affichage des classes\n");
    printf(" 5 - Generer hasse.mmd\n");
    printf(" 6 - Informations sur les classes\n");
    printf(" 7 - Afficher la matrice de transition M\n");
    printf(" 8 - Calculer M^3\n");
    printf(" 9 - Sous-matrices par classe\n");
    printf(" 0 - Quitter\n");
    printf("\nChoix: ");
}

int main(int argc, char **argv)
{
    const char *graph_file = "../data/exemple1";

    if (argc >= 2)
        graph_file = argv[1];

    printf("\nProjet Markov\n");
    printf("Fichier charge : %s\n\n", graph_file);

    liste_adjacence graph = readGraph(graph_file);

    int choix;
    t_class *classes = NULL;
    int *tab_class = NULL;
    t_link *links = NULL;
    t_matrix M;

    do {
        menu();
        scanf("%d", &choix);
        printf("\n");

        switch (choix)
        {
            case 1:
                printf("Liste d'adjacence :\n");
                display_liste_adjacence(&graph);
                break;

            case 2:
                printf("Verification de Markov :\n");
                valid_graph(&graph);
                break;

            case 3:
                printf("Generation du fichier graph.mmd\n");
                draw_graph(&graph, "graph.mmd");
                printf("Fichier cree.\n");
                break;

            case 4:
                printf("Calcul des classes (Tarjan) :\n");
                classes = tarjan_function(&graph);
                display_classes(classes);
                break;

            case 5:
                if (!classes) classes = tarjan_function(&graph);
                tab_class = create_tab_class(&graph, classes);
                links = get_class_link(tab_class, &graph);
                draw_classes_link(links, classes, "hasse.mmd");
                printf("Generation de hasse.mmd terminee\n");
                break;

            case 6:
                if (!classes) classes = tarjan_function(&graph);
                if (!links)
                    links = get_class_link(create_tab_class(&graph, classes), &graph);
                printf("Informations sur les classes :\n");
                display_information(classes, links);
                break;

            case 7:
                printf("Matrice de transition M :\n");
                M = create_matrix(&graph);
                display_matrix(&M);
                break;

            case 8: {
                printf("Calcul de M^3 :\n");
                M = create_matrix(&graph);
                t_matrix M2 = multiply_matrix(&M, &M);
                t_matrix M3 = multiply_matrix(&M2, &M);
                display_matrix(&M3);
                break;
            }

            case 9: {
                if (!classes) classes = tarjan_function(&graph);
                printf("Sous-matrices :\n");

                t_class *tmp = classes;
                int i = 1;
                while (tmp != NULL) {
                    printf("Classe C%d :\n", i);
                    M = create_matrix(&graph);

                    t_partition part;
                    part.head = tmp->sommet;
                    t_matrix sub = subMatrix(M, part);
                    display_matrix(&sub);
                    printf("\n");

                    tmp = tmp->next;
                    i++;
                }
                break;
            }

            case 0:
                printf("Fin du programme.\n");
                break;

            default:
                printf("Choix invalide.\n");
        }

    } while (choix != 0);

    return 0;
}
