#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "graph.h"

/*
 * PARTIE 1 - GRAPHE / LISTE D'ADJACENCE
 */

/* Création d'une cellule (arc) */
cellule_t *create_cell(int arrivee, float proba)
{
    cellule_t *cell = malloc(sizeof(cellule_t));
    if (cell == NULL) {
        perror("Could not allocate cellule");
        exit(EXIT_FAILURE);
    }
    cell->arrivee = arrivee;
    cell->proba = proba;
    cell->next = NULL;
    return cell;
}

/* Liste vide */
liste create_empty_list(void)
{
    liste empty;
    empty.head = NULL;
    return empty;
}

/* Ajout d'une cellule en tête de liste */
void add_cell(liste *list, int arrivee, float proba)
{
    cellule_t *cell = create_cell(arrivee, proba);
    cell->next = list->head;
    list->head = cell;
}

/* Affichage d'une liste d'adjacence pour debug */
void display_list(liste *list)
{
    cellule_t *tmp = list->head;
    while (tmp != NULL) {
        printf("(%d, %.2f) @-> ", tmp->arrivee, tmp->proba);
        tmp = tmp->next;
    }
    printf("NULL\n");
}

/* Création d'un tableau de listes d'adjacence vide */
liste_adjacence create_empty_liste_adjacence(int nbvert)
{
    liste_adjacence la;
    la.taille = nbvert;
    la.tab = malloc(sizeof(liste) * nbvert);
    if (la.tab == NULL) {
        perror("Could not allocate tab");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nbvert; i++) {
        la.tab[i] = create_empty_list();
    }
    return la;
}

/* Affichage global du graphe (tableau de listes) */
void display_liste_adjacence(liste_adjacence *la)
{
    for (int i = 0; i < la->taille; i++) {
        printf("Liste pour le sommet %d: [head @] -> ", i + 1);
        display_list(&la->tab[i]);
    }
}

/* Lecture d'un graphe depuis un fichier texte
 * Format:
 *   nb_sommets
 *   depart arrivee proba
 */
/* Les exemples du cours restent petits ; cette limite evite les allocations
   enormes et une recursion Tarjan trop profonde. */
liste_adjacence readGraph(const char *filename)
{
    FILE *file = fopen(filename, "r");
    liste_adjacence graph = {NULL, 0};
    char line[512], *cursor, *end;
    long count;
    int number = 1;
    if (!file) { perror(filename); exit(EXIT_FAILURE); }
    if (!fgets(line, sizeof line, file)) goto invalid;
    errno = 0;
    count = strtol(line, &end, 10);
    if (end == line || errno || count < 1 || count > 1000) goto invalid;
    while (isspace((unsigned char)*end)) ++end;
    if (*end) goto invalid;
    graph = create_empty_liste_adjacence((int)count);
    while (fgets(line, sizeof line, file)) {
        ++number;
        if (!strchr(line, '\n') && !feof(file)) goto invalid;
        cursor = line;
        while (isspace((unsigned char)*cursor)) ++cursor;
        if (!*cursor) continue;
        errno = 0;
        long from = strtol(cursor, &end, 10);
        if (end == cursor || errno || !isspace((unsigned char)*end)) goto invalid;
        cursor = end;
        errno = 0;
        long to = strtol(cursor, &end, 10);
        if (end == cursor || errno || !isspace((unsigned char)*end)) goto invalid;
        cursor = end;
        errno = 0;
        double probability = strtod(cursor, &end);
        if (end == cursor || errno || !isfinite(probability)) goto invalid;
        while (isspace((unsigned char)*end)) ++end;
        if (*end || from < 1 || from > count || to < 1 || to > count ||
            probability < 0 || probability > 1) goto invalid;
        /* Un arc de probabilite nulle ne cree pas d'accessibilite. */
        if (probability > 0) add_cell(&graph.tab[from - 1], (int)to, (float)probability);
    }
    if (ferror(file)) goto invalid;
    fclose(file);
    return graph;
invalid:
    fprintf(stderr, "Fichier invalide : %s, ligne %d.\n", filename, number);
    free_graph(&graph);
    fclose(file);
    exit(EXIT_FAILURE);
}

/* Vérification de la propriété de Markov pour un sommet:
 * somme des probabilités sortantes ≈ 1
 */
int valid_sommet(liste *list, int sommet)
{
    cellule_t *tmp = list->head;
    float somme = 0.0f;

    while (tmp != NULL) {
        somme += tmp->proba;
        tmp = tmp->next;
    }
    if (somme > 1.01f || somme < 0.99f) {
        printf("La somme des probabilites du sommet %d est %.2f\n", sommet, somme);
        return 0;
    }
    return 1;
}

/* Vérification globale de la propriété de Markov */
void valid_graph(liste_adjacence *list)
{
    int valid = 1;
    for (int i = 0; i < list->taille; i++) {
        if (!valid_sommet(&list->tab[i], i + 1))
            valid = 0;
    }
    if (!valid)
        printf("Le graphe n est pas un graphe de Markov\n");
    else
        printf("Le graphe est un graphe de Markov\n");
}

/* Conversion 1 -> A, 2 -> B, ..., 26 -> Z, 27 -> AA, etc. */
static char *getId(int num)
{
    static char buffer[10];
    char temp[10];
    int index = 0;

    if (num <= 0) {
        buffer[0] = '\0';
        return buffer;
    }

    int n = num - 1; // passer en 0-based

    while (n >= 0) {
        temp[index++] = 'A' + (n % 26);
        n = (n / 26) - 1;
    }

    // on a rempli temp à l’envers → on recopie à l'endroit
    for (int i = 0; i < index; i++)
        buffer[i] = temp[index - 1 - i];

    buffer[index] = '\0';
    return buffer;
}

/* Génération des arcs en Mermaid pour un sommet donné */
void graph_cell(liste *list, int id, FILE *file)
{
    cellule_t *tmp = list->head;
    char from[10];
    char to[10];

    strcpy(from, getId(id));

    while (tmp != NULL) {
        strcpy(to, getId(tmp->arrivee));
        fprintf(file, "%s -->|%.2f| %s\n", from, tmp->proba, to);
        tmp = tmp->next;
    }
}

/* Génération complète du fichier Mermaid du graphe */
void draw_graph(liste_adjacence *list, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Could not open file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "  layout: elk\n");
    fprintf(file, "  theme: neo\n");
    fprintf(file, "  look: neo\n");
    fprintf(file, "---\n\n");

    fprintf(file, "flowchart LR\n");

    // Déclaration des sommets
    for (int i = 0; i < list->taille; i++) {
        fprintf(file, "%s((%d))\n", getId(i + 1), i + 1);
    }
    fprintf(file, "\n");

    // Déclaration des arcs
    for (int i = 0; i < list->taille; i++) {
        graph_cell(&list->tab[i], i + 1, file);
    }

    fclose(file);
    printf("graph.mmd genere\n");
}

/*
 * PARTIE 2 - TARJAN / CLASSES / DIAGRAMME DE HASSE
 */

/* Initialisation des sommets pour Tarjan */
t_tarjan_vertex *transform_graph(liste_adjacence *list)
{
    t_tarjan_vertex *vertex = malloc(sizeof(t_tarjan_vertex) * list->taille);
    if (vertex == NULL) {
        perror("Could not allocate vertex");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < list->taille; i++) {
        vertex[i].identifiant = i;
        vertex[i].numero = -1;
        vertex[i].accessible = -1;
        vertex[i].indicateur = false;
    }
    return vertex;
}

/* Ajout d'une classe (liste chainée de sommets) */
void add_class(t_class **head, t_sommet *list)
{
    t_class *new = malloc(sizeof(t_class));
    if (new == NULL) {
        perror("Could not allocate class");
        exit(EXIT_FAILURE);
    }
    new->sommet = list;
    new->next = *head;
    *head = new;
}

/* Ajout d'un sommet dans une pile/list chainée */
void add_sommet(t_sommet **head, int identifiant)
{
    t_sommet *new = malloc(sizeof(t_sommet));
    if (new == NULL) {
        perror("Could not allocate sommet");
        exit(EXIT_FAILURE);
    }
    new->identifiant = identifiant;
    new->next = *head;
    *head = new;
}

/* Retrait d'un sommet (utilisé comme pop de pile) */
int delete_sommet(t_sommet **head)
{
    t_sommet *tmp = *head;
    int identifiant = tmp->identifiant;
    *head = tmp->next;
    free(tmp);
    return identifiant;
}

/* DFS de Tarjan */
void parcours(int v, t_tarjan *tarjan, t_sommet **sommet, int *num)
{
    cellule_t *tmp_cell = tarjan->graph->tab[v].head;
    t_sommet *tmp_sommet = NULL;
    int w;

    tarjan->tab[v].numero = *num;
    tarjan->tab[v].accessible = *num;
    (*num)++;
    add_sommet(sommet, v);
    tarjan->tab[v].indicateur = true;

    while (tmp_cell != NULL) {
        w = tmp_cell->arrivee - 1;
        if (tarjan->tab[w].numero == -1) {
            parcours(w, tarjan, sommet, num);
            if (tarjan->tab[v].accessible > tarjan->tab[w].accessible)
                tarjan->tab[v].accessible = tarjan->tab[w].accessible;
        } else if (tarjan->tab[w].indicateur) {
            if (tarjan->tab[v].accessible > tarjan->tab[w].numero)
                tarjan->tab[v].accessible = tarjan->tab[w].numero;
        }
        tmp_cell = tmp_cell->next;
    }

    if (tarjan->tab[v].accessible == tarjan->tab[v].numero) {
        do {
            w = delete_sommet(sommet);
            tarjan->tab[w].indicateur = false;
            add_sommet(&tmp_sommet, w);
        } while (w != v);
        add_class(&tarjan->classes, tmp_sommet);
    }
}

/* Fonction principale Tarjan : renvoie la liste des classes fortement connexes */
t_class *tarjan_function(liste_adjacence *graph)
{
    int num = 0;
    t_tarjan tarjan;
    t_sommet *sommet = NULL;

    tarjan.tab = transform_graph(graph);
    tarjan.classes = NULL;
    tarjan.graph = graph;

    for (int i = 0; i < graph->taille; i++) {
        if (tarjan.tab[i].numero == -1) {
            parcours(i, &tarjan, &sommet, &num);
        }
    }

    free(tarjan.tab);
    return tarjan.classes;
}

/* Affichage des classes */
void display_classes(t_class *classes)
{
    t_class *tmp_class = classes;
    t_sommet *tmp_sommet = NULL;
    int i = 1;
    int tmp = 0;

    while (tmp_class != NULL) {
        tmp = 0;
        tmp_sommet = tmp_class->sommet;
        printf("Composante C%d: {", i);
        while (tmp_sommet != NULL) {
            if (tmp != 0)
                printf(",");
            printf("%d", tmp_sommet->identifiant + 1);
            tmp++;
            tmp_sommet = tmp_sommet->next;
        }
        printf("}\n");
        i++;
        tmp_class = tmp_class->next;
    }
}

/* Construction d'un tableau: sommet -> indice de classe */
int *create_tab_class(liste_adjacence *graph, t_class *classes)
{
    int *tab_class = malloc(sizeof(int) * graph->taille);
    t_class *tmp_class = classes;
    t_sommet *tmp_sommet = NULL;
    int i = 0;

    if (tab_class == NULL) {
        perror("Could not allocate tab_class");
        exit(EXIT_FAILURE);
    }

    while (tmp_class != NULL) {
        tmp_sommet = tmp_class->sommet;
        while (tmp_sommet != NULL) {
            tab_class[tmp_sommet->identifiant] = i;
            tmp_sommet = tmp_sommet->next;
        }
        i++;
        tmp_class = tmp_class->next;
    }
    return tab_class;
}

/* Ajout d'un lien entre classes (évite les doublons) */
void add_link(t_link **link, int start, int end)
{
    t_link *tmp = *link;
    while (tmp != NULL) {
        if (tmp->start == start && tmp->end == end)
            return;
        tmp = tmp->next;
    }

    t_link *new_link = malloc(sizeof(t_link));
    if (new_link == NULL) {
        perror("Could not allocate link");
        exit(EXIT_FAILURE);
    }
    new_link->start = start;
    new_link->end = end;
    new_link->next = *link;
    *link = new_link;
}

/* Construction des liens entre classes (diagramme de Hasse non réduit) */
t_link *get_class_link(int *tab_class, liste_adjacence *graph)
{
    t_link *link = NULL;
    cellule_t *tmp = NULL;

    for (int i = 0; i < graph->taille; i++) {
        tmp = graph->tab[i].head;
        while (tmp != NULL) {
            int c_from = tab_class[i];
            int c_to   = tab_class[tmp->arrivee - 1];
            if (c_from != c_to) {
                add_link(&link, c_from, c_to);
            }
            tmp = tmp->next;
        }
    }
    return link;
}

/* Génération du label Mermaid pour une classe */
void draw_label(t_class *cls, int index, FILE *file)
{
    t_sommet *tmp = cls->sommet;
    int i = 0;

    fprintf(file, " C%d[\"{", index);
    while (tmp != NULL) {
        if (i != 0)
            fprintf(file, ", ");
        fprintf(file, "%d", tmp->identifiant + 1);
        i++;
        tmp = tmp->next;
    }
    fprintf(file, "}\"];\n");
}

/* Génération du diagramme de Hasse (Mermaid) */
void draw_classes_link(t_link *link, t_class *classes, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Could not open file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n  layout: elk\n  theme: neo\n  look: neo\n");
    fprintf(file, "---\n\ngraph TB\n");

    t_class *tmp_class = classes;
    int id = 1;
    while (tmp_class != NULL) {
        draw_label(tmp_class, id, file);
        tmp_class = tmp_class->next;
        id++;
    }

    t_link *tmp_link = link;
    while (tmp_link != NULL) {
        fprintf(file, "C%d --> C%d\n", tmp_link->start + 1, tmp_link->end + 1);
        tmp_link = tmp_link->next;
    }

    fclose(file);
    printf("hasse.mmd genere\n");
}

/* Fonctions d'information sur les classes */

bool can_exist(int class_index, t_link *link)
{
    t_link *tmp = link;
    while (tmp != NULL) {
        if (tmp->start == class_index)
            return true;
        tmp = tmp->next;
    }
    return false;
}

bool is_lonely(t_sommet *list)
{
    return (list->next == NULL);
}

bool is_irreductible(t_class *classes)
{
    return (classes != NULL && classes->next == NULL);
}

void display_sommet(t_sommet *sommet, bool and)
{
    t_sommet *tmp = sommet;
    int i = 0;

    while (tmp != NULL) {
        if (i != 0) {
            if (and && tmp->next == NULL)
                printf(" et ");
            else
                printf(", ");
        }
        printf("%d", tmp->identifiant + 1);
        i++;
        tmp = tmp->next;
    }
}

void display_information(t_class *classes, t_link *link)
{
    t_class *tmp_class = classes;
    int i = 0;

    while (tmp_class != NULL) {
        printf("La classe {");
        if (can_exist(i, link)) {
            display_sommet(tmp_class->sommet, false);
            printf("} est transitoire - ");
            if (is_lonely(tmp_class->sommet))
                printf("l etat %d est transitoire;\n", tmp_class->sommet->identifiant + 1);
            else {
                printf("les etats ");
                display_sommet(tmp_class->sommet, true);
                printf(" sont transitoires;\n");
            }
        } else {
            display_sommet(tmp_class->sommet, false);
            printf("} est persistante - ");
            if (is_lonely(tmp_class->sommet))
                printf("l etat %d est persistant - L etat %d est absorbant;\n",
                       tmp_class->sommet->identifiant + 1,
                       tmp_class->sommet->identifiant + 1);
            else {
                printf("les etats ");
                display_sommet(tmp_class->sommet, true);
                printf(" sont persistants;\n");
            }
        }
        i++;
        tmp_class = tmp_class->next;
    }
    if (is_irreductible(classes))
        printf("Le graphe de Markov est irreductible.\n");
    else
        printf("Le graphe de Markov n est pas irreductible.\n");
}

/*
 * PARTIE 3 - MATRICES / SOUS-MATRICES / CONVERGENCE
 */

/* Matrice vide (row x column remplie de 0) */
t_matrix create_empty_matrix(int row, int column)
{
    t_matrix matrix;
    matrix.matrix = malloc(sizeof(double *) * row);
    if (matrix.matrix == NULL) {
        perror("Could not allocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < row; i++) {
        matrix.matrix[i] = malloc(sizeof(double) * column);
        if (matrix.matrix[i] == NULL) {
            perror("Could not allocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < column; j++)
            matrix.matrix[i][j] = 0.0;
    }
    matrix.row = row;
    matrix.column = column;
    return matrix;
}

/* Matrice identité de taille n x n */
t_matrix identity_matrix(int n)
{
    t_matrix I = create_empty_matrix(n, n);
    for (int i = 0; i < n; i++)
        I.matrix[i][i] = 1.0;
    return I;
}

/* Création de la matrice de transition à partir du graphe */
t_matrix create_matrix(liste_adjacence *graph)
{
    t_matrix matrix;
    cellule_t *tmp = NULL;

    matrix.matrix = malloc(sizeof(double *) * graph->taille);
    if (matrix.matrix == NULL) {
        perror("Could not allocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < graph->taille; i++) {
        tmp = graph->tab[i].head;
        matrix.matrix[i] = malloc(sizeof(double) * graph->taille);
        if (matrix.matrix[i] == NULL) {
            perror("Could not allocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < graph->taille; j++)
            matrix.matrix[i][j] = 0.0;
        while (tmp != NULL) {
            matrix.matrix[i][tmp->arrivee - 1] += tmp->proba;
            tmp = tmp->next;
        }
    }
    matrix.row = graph->taille;
    matrix.column = graph->taille;
    return matrix;
}

/* Copie d'une matrice */
t_matrix copy_matrix(t_matrix *copy)
{
    t_matrix matrix;
    matrix.matrix = malloc(sizeof(double *) * copy->row);
    if (matrix.matrix == NULL) {
        perror("Could not allocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < copy->row; i++) {
        matrix.matrix[i] = malloc(sizeof(double) * copy->column);
        if (matrix.matrix[i] == NULL) {
            perror("Could not allocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < copy->column; j++)
            matrix.matrix[i][j] = copy->matrix[i][j];
    }
    matrix.row = copy->row;
    matrix.column = copy->column;
    return matrix;
}

/* Calcul d'une case (row,column) de first*second */
double calcul_cell(t_matrix *first, t_matrix *second, int row, int column)
{
    double result = 0.0;
    for (int i = 0; i < first->column; i++)
        result += first->matrix[row][i] * second->matrix[i][column];
    return result;
}

/* Produit matriciel */
t_matrix multiply_matrix(t_matrix *first, t_matrix *second)
{
    t_matrix result;
    if (first->column != second->row) {
        perror("The number of columns of the first matrix does not correspond to the number of rows of the second matrix");
        exit(EXIT_FAILURE);
    }
    result = create_empty_matrix(first->row, second->column);
    for (int i = 0; i < first->row; i++) {
        for (int j = 0; j < second->column; j++) {
            result.matrix[i][j] = calcul_cell(first, second, i, j);
        }
    }
    return result;
}
/* Puissance entière d'une matrice : M^n */
t_matrix power_matrix(t_matrix *M, int n)
{
    if (n < 0 || M->row != M->column) {
        fprintf(stderr, "power_matrix: n must be >= 0\n");
        exit(EXIT_FAILURE);
    }

    /* Cas n = 0 : retourne l'identité */
    if (n == 0) {
        return identity_matrix(M->row);
    }

    /* Cas n = 1 : copie de M */
    if (n == 1) {
        return copy_matrix(M);
    }

    /* Cas n >= 2 : on part de M et on multiplie n-1 fois */
    t_matrix result = copy_matrix(M);   // result = M

    for (int k = 1; k < n; k++) {
        t_matrix next = multiply_matrix(&result, M);

        // libère l'ancienne matrice result
        for (int i = 0; i < result.row; i++)
            free(result.matrix[i]);
        free(result.matrix);

        result = next;                  // result = result * M
    }

    return result;  // contient M^n
}

/* Matrice des valeurs absolues des différences : |first - second| */
t_matrix diff_matrix(t_matrix *first, t_matrix *second)
{
    t_matrix result;
    if (first->row != second->row || first->column != second->column) {
        perror("The number of columns or rows of the first matrix does not correspond to the number of columns or rows of the second matrix");
        exit(EXIT_FAILURE);
    }
    result = create_empty_matrix(first->row, first->column);
    for (int i = 0; i < first->row; i++) {
        for (int j = 0; j < first->column; j++) {
            result.matrix[i][j] = fabs(first->matrix[i][j] - second->matrix[i][j]);
        }
    }
    return result;
}

/* Norme scalaire diff(M,N) = somme des |m_ij - n_ij| */
double norm_diff_matrix(t_matrix *first, t_matrix *second)
{
    if (first->row != second->row || first->column != second->column) {
        perror("Matrix sizes do not match in norm_diff_matrix");
        exit(EXIT_FAILURE);
    }

    double sum = 0.0;
    for (int i = 0; i < first->row; i++) {
        for (int j = 0; j < first->column; j++) {
            sum += fabs(first->matrix[i][j] - second->matrix[i][j]);
        }
    }
    return sum;
}

/* Affichage d'une matrice (pour debug) */
void display_matrix(t_matrix *matrix)
{
    for (int i = 0; i < matrix->row; i++) {
        for (int j = 0; j < matrix->column; j++)
            printf("%.2f ", matrix->matrix[i][j]);
        printf("\n");
    }
}

/* Longueur d'une liste de sommets */
int len_sommet(t_sommet *sommet)
{
    t_sommet *tmp = sommet;
    int i = 0;
    while (tmp != NULL) {
        i++;
        tmp = tmp->next;
    }
    return i;
}

/* Sous-matrice associée à une composante (classe) */
t_matrix subMatrix(t_matrix matrix, t_partition part)
{
    int len = len_sommet(part.head);
    t_matrix result = create_empty_matrix(len, len);
    t_sommet *first = part.head;
    t_sommet *second = part.head;
    int i = 0;
    int j = 0;

    while (first != NULL) {
        j = 0;
        second = part.head;
        while (second != NULL) {
            result.matrix[i][j] =
                matrix.matrix[first->identifiant][second->identifiant];
            j++;
            second = second->next;
        }
        i++;
        first = first->next;
    }
    return result;
}

/* Puissance de M jusqu'à convergence selon un epsilon */
t_matrix power_until_convergence(t_matrix *M, double epsilon, int max_iter)
{
    t_matrix current = copy_matrix(M);

    for (int k = 0; k < max_iter; k++) {
        t_matrix next = multiply_matrix(&current, M);
        double diff = norm_diff_matrix(&next, &current);

        // Libération de l'ancienne matrice
        for (int i = 0; i < current.row; i++)
            free(current.matrix[i]);
        free(current.matrix);

        current = next;

        if (diff < epsilon)
            break;
    }
    return current;
}

/* Calcul (approché) des distributions stationnaires pour chaque classe */
void stationary_distribution_for_classes(liste_adjacence *graph, t_class *classes)
{
    if (classes == NULL)
        return;

    t_matrix M = create_matrix(graph);
    t_class *cls = classes;
    int index = 1;

    while (cls != NULL) {
        t_partition part;
        part.head = cls->sommet;

        t_matrix sub = subMatrix(M, part);
        t_matrix limit = power_until_convergence(&sub, 0.0001, 1000);

        printf("Distribution stationnaire approx pour la classe C%d :\n[ ", index);
        for (int j = 0; j < limit.column; j++) {
            printf("%.4f ", limit.matrix[0][j]);
        }
        printf("]\n\n");

        // libération de sub et limit
        for (int i = 0; i < sub.row; i++)
            free(sub.matrix[i]);
        free(sub.matrix);

        for (int i = 0; i < limit.row; i++)
            free(limit.matrix[i]);
        free(limit.matrix);

        cls = cls->next;
        index++;
    }

    // libération de M
    for (int i = 0; i < M.row; i++)
        free(M.matrix[i]);
    free(M.matrix);
}

void free_graph(liste_adjacence *graph)
{
    for (int i = 0; i < graph->taille; ++i) {
        cellule_t *cell = graph->tab[i].head;
        while (cell) { cellule_t *next = cell->next; free(cell); cell = next; }
    }
    free(graph->tab);
    graph->tab = NULL;
    graph->taille = 0;
}

void free_classes(t_class *classes)
{
    while (classes) {
        t_class *next = classes->next;
        while (classes->sommet) delete_sommet(&classes->sommet);
        free(classes);
        classes = next;
    }
}

void free_links(t_link *links)
{
    while (links) { t_link *next = links->next; free(links); links = next; }
}

void free_matrix(t_matrix *matrix)
{
    for (int i = 0; i < matrix->row; ++i) free(matrix->matrix[i]);
    free(matrix->matrix);
    matrix->matrix = NULL;
    matrix->row = matrix->column = 0;
}
