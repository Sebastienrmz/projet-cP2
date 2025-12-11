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
    printf("10 - Calculer M^n\n");
    printf(" 0 - Quitter\n");
    printf("\nChoix: ");
}

int main(int argc, char **argv)
{
    // fichier par defaut
    const char *graph_file = "../data/proba";

    // si on donne un fichier en argument
    if (argc >= 2)
        graph_file = argv[1];

    printf("\nProjet Markov\n");
    printf("Fichier charge : %s\n\n", graph_file);

    // lecture du graphe
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

                // free
                for (int i = 0; i < M.row; i++)
                    free(M.matrix[i]);
                free(M.matrix);
                break;

            case 8: {
                printf("Calcul de M^3 :\n");
                M = create_matrix(&graph);
                t_matrix M2 = multiply_matrix(&M, &M);
                t_matrix M3 = multiply_matrix(&M2, &M);
                display_matrix(&M3);

                // free
                for (int i = 0; i < M.row; i++)
                    free(M.matrix[i]);
                free(M.matrix);

                for (int i = 0; i < M2.row; i++)
                    free(M2.matrix[i]);
                free(M2.matrix);

                for (int i = 0; i < M3.row; i++)
                    free(M3.matrix[i]);
                free(M3.matrix);
                break;
            }

            case 9: {
                if (!classes) classes = tarjan_function(&graph);
                printf("Sous-matrices par classe:\n");

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

                    // free sub
                    for (int r = 0; r < sub.row; r++)
                        free(sub.matrix[r]);
                    free(sub.matrix);

                    // free M
                    for (int r = 0; r < M.row; r++)
                        free(M.matrix[r]);
                    free(M.matrix);

                    tmp = tmp->next;
                    i++;
                }
                break;
            }

            case 10: {   // <<< NOUVELLE OPTION M^n
                int n;
                printf("Entrez la puissance n : ");
                scanf("%d", &n);

                // construire la matrice M
                M = create_matrix(&graph);

                // calcul de M^n
                t_matrix Mn = power_matrix(&M, n);

                printf("\nMatrice M^%d :\n", n);
                display_matrix(&Mn);
                printf("\n");

                // free
                for (int i = 0; i < M.row; i++)
                    free(M.matrix[i]);
                free(M.matrix);

                for (int i = 0; i < Mn.row; i++)
                    free(Mn.matrix[i]);
                free(Mn.matrix);

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
