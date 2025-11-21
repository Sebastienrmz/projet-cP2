#include <stdio.h>
#include <stdlib.h>
#include "include.h"
//étape1
cellule_t *create_cell(int arrivee, float proba)
{
    cellule_t *cell = malloc(sizeof(cellule_t));

    if (cell == NULL) {
        perror("Could not alocate cellule");
        exit(EXIT_FAILURE);
    }
    cell->arrivee = arrivee;
    cell->proba = proba;
    cell->next = NULL;
    return cell;
}

liste create_empty_list()
{
    liste empty;

    empty.head = NULL;
    return empty;
}

void add_cell(liste *list, int arrivee, float proba)
{
    cellule_t *cell = create_cell(arrivee, proba);

    cell->next = list->head;
    list->head = cell;
}

void display_list(liste *list)
{
    cellule_t *tmp = list->head;

    while (tmp != NULL) {
        printf("(%d, %.2f) @-> ", tmp->arrivee, tmp->proba);
        tmp = tmp->next;
    }
    printf("NULL\n");
}

liste_adjacence create_empty_liste_adjacence(int nbvert)
{
    liste_adjacence la;

    la.taille = nbvert;
    la.tab = malloc(sizeof(liste) * nbvert);
    if (la.tab == NULL) {
        perror("Could not alocate tab");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nbvert; i++) {
        la.tab[i] = create_empty_list();
    }
    return la;
}

void display_liste_adjacence(liste_adjacence *la)
{
    for (int i = 0; i < la->taille; i++) {
        printf("Liste pour le somet %d: [head @] -> ", i + 1);
        display_list(&la->tab[i]);
    }
}

liste_adjacence readGraph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    int nbvert, depart, arrivee;
    float proba;
    liste_adjacence la;

    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", &nbvert) != 1)
    {
        perror("Could not read number of vertices");
        exit(EXIT_FAILURE);
    }

    la = create_empty_liste_adjacence(nbvert);

    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3)
    {
        add_cell(&la.tab[depart - 1], arrivee, proba);
    }

    fclose(file);
    return la;
}
//étape2
int valid_sommet(liste *list, int sommet) {
    cellule_t *tmp = list->head;
    float somme = 0;

    while (tmp != NULL) {
        somme += tmp->proba;
        tmp = tmp->next;
    }
    if (somme > 1 || somme < 0.99) {
        printf("la somme des probabilités du sommet %d est %.2f\n", sommet, somme);
        return 0;
    }
    return 1;
}

void valid_graph(liste_adjacence *list) {
    int valid = 1;

    for (int i = 0; i < list->taille; i++) {
        if (!valid_sommet(&list->tab[i], i + 1))
            valid = 0;
    }
    if (!valid)
        printf("Le graphe n'est pas un graphe de Markov\n");
    else
        printf("Le graphe est un graphe de Markov\n");
}
//étape3

void graph_cell(liste *list, int id, FILE *file)
{
    cellule_t *tmp = list->head;

    while (tmp != NULL) {
        fprintf(file, "%s -->|%.2f|%s\n", getId(id), tmp->proba, getId(tmp->arrivee));
        tmp = tmp->next;
    }
}

void draw_graph(liste_adjacence *list, char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "---\nconfig:\n layout: elk\n theme: neo\n look: neo\n---\n\nflowchart LR\n");
    for (int i = 0; i < list->taille; i++) {
        fprintf(file, "%s((%d))\n", getId(i + 1), i + 1);
    }
    fprintf(file, "\n");
    for (int i = 0; i < list->taille; i++) {
        graph_cell(&list->tab[i], i + 1, file);
    }
    fclose(file);
}

//####33333333####3333####33333333#333##

//Partie 2
//étape 1
#include <stdio.h>
#include <stdlib.h>
#include "include.h"

t_tarjan_vertex *transform_graph(liste_adjacence *list)
{
    t_tarjan_vertex *vertex = malloc(sizeof(t_tarjan_vertex) * list->taille);

    if (vertex == NULL) {
        perror("Could not alocate vertex");
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

void add_sommet(t_sommet **head, int identifiant)
{
    t_sommet *new = malloc(sizeof(t_sommet));

    if (new == NULL) {
        perror("Could not alocate sommet");
        exit(EXIT_FAILURE);
    }

    new->identifiant = identifiant;
    new->next = *head;
    *head = new;
}

int delete_sommet(t_sommet **head)
{
    t_sommet *tmp = *head;
    int identifiant = tmp->identifiant;

    *head = tmp->next;
    return identifiant;
}

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
        }
        else if (tarjan->tab[w].indicateur) {
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

t_class *tarjan_function(liste_adjacence *graph) {
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

    return tarjan.classes;
}

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

//étape 2

int *create_tab_class(liste_adjacence *graph, t_class *classes)
{
    int *tab_class = malloc(sizeof(int) * graph->taille);
    t_class *tmp_class = classes;
    t_sommet *tmp_sommet = NULL;
    int i = 0;

    if (tab_class == NULL)  {
        perror("Could not alocate tab_class");
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

void add_link(t_link **link, int start, int end)
{
    t_link *new = NULL;
    t_link *tmp = *link;

    while (tmp != NULL) {
        if (tmp->start == start && tmp->end == end)
            return;
        tmp = tmp->next;
    }
    new = malloc(sizeof(t_link));
    if (new == NULL) {
        perror("Could not alocate link");
        exit(EXIT_FAILURE);
    }
    new->start = start;
    new->end = end;
    new->next = *link;
    *link = new;
}

t_link *get_class_link(int *tab_class, liste_adjacence *graph)
{
    t_link *link = NULL;
    cellule_t *tmp = NULL;

    for (int i = 0; i < graph->taille; i++) {
        tmp = graph->tab[i].head;
        while (tmp != NULL) {
            if (tab_class[i] != tab_class[tmp->arrivee - 1])
                add_link(&link, tab_class[i], tab_class[tmp->arrivee - 1]);
            tmp = tmp->next;
        }
    }
    return link;
}

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

void draw_classes_link(t_link *link, t_class *classes, char *filename)
{
    FILE *file = fopen(filename, "w");
    t_link *tmp_link = link;
    t_class *tmp_class = classes;
    int i = 1;

    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "---\nconfig:\n layout: elk\n theme: neo\n look: neo\n---\n\ngraph TB\n");
    while (tmp_class != NULL) {
        draw_label(tmp_class, i, file);
        tmp_class = tmp_class->next;
        i++;
    }
    while (tmp_link != NULL) {
        fprintf(file, " C%d --> C%d\n", tmp_link->start + 1, tmp_link->end + 1);
        tmp_link = tmp_link->next;
    }

    fclose(file);
}

//étape 3

bool can_exist(int class, t_link *link)
{
    t_link *tmp = link;

    while (tmp != NULL) {
        if (tmp->start == class)
            return true;
        tmp = tmp->next;
    }
    return false;
}

bool is_lonely(t_sommet *list)
{
    if (list->next == NULL)
        return true;
    return false;
}

bool is_irreductible(t_class *classes)
{
    if (classes->next == NULL)
        return true;
    return false;
}

void display_sommet(t_sommet *sommet, bool and)
{
    t_sommet *tmp = sommet;
    int i = 0;

    while (tmp != NULL) {
        if (i != 0 || (and && tmp->next == NULL))
            if (and && tmp->next == NULL)
                printf(" et ");
            else
                printf(", ");
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
                printf("l'état %d est transitoire;\n", tmp_class->sommet->identifiant + 1);
            else {
                printf("les états ");
                display_sommet(tmp_class->sommet, true);
                printf(" sont transitoires;\n");
            }
        } else {
            display_sommet(tmp_class->sommet, false);
            printf("} est persistante - ");
            if (is_lonely(tmp_class->sommet))
                printf("l'état %d est persistant - L'état %d est absorbant;\n", tmp_class->sommet->identifiant + 1, tmp_class->sommet->identifiant + 1);
            else {
                printf("les états ");
                display_sommet(tmp_class->sommet, true);
                printf(" sont persistants;\n");
            }
        }
        i++;
        tmp_class = tmp_class->next;
    }
    if (is_irreductible(classes))
        printf("Le graphe de Markov est irréductible.\n");
    else
        printf("Le graphe de Markov n’est pas irréductible.\n");
}



//###############333#######3#33333######333#3#33##

//Partie 3
//étape1

#include "include.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

t_matrix create_empty_matrix(int row, int column)
{
    t_matrix matrix;

    matrix.matrix = malloc(sizeof(double *) * row);
    if (matrix.matrix == NULL) {
        perror("Could not alocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < row; i++) {
        matrix.matrix[i] = malloc(sizeof(double) * column);

        if (matrix.matrix[i] == NULL) {
            perror("Could not alocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < column; j++)
            matrix.matrix[i][j] = 0;
    }
    matrix.row = row;
    matrix.column = column;
    return matrix;
}

t_matrix create_matrix(liste_adjacence *graph)
{
    t_matrix matrix;

    matrix.matrix = malloc(sizeof(double *) * graph->taille);
    cellule_t *tmp = NULL;

    if (matrix.matrix == NULL) {
        perror("Could not alocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < graph->taille; i++) {
        tmp = graph->tab[i].head;
        matrix.matrix[i] = malloc(sizeof(double) * graph->taille);

        if (matrix.matrix[i] == NULL) {
            perror("Could not alocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < graph->taille; j++)
            matrix.matrix[i][j] = 0;
        while (tmp != NULL) {
            matrix.matrix[i][tmp->arrivee - 1] = tmp->proba;
            tmp = tmp->next;
        }
    }
    matrix.row = graph->taille;
    matrix.column = graph->taille;
    return matrix;
}

t_matrix copy_matrix(t_matrix *copy)
{
    t_matrix matrix;

    matrix.matrix = malloc(sizeof(double *) * copy->row);
    if (matrix.matrix == NULL) {
        perror("Could not alocate matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < copy->row; i++) {
        matrix.matrix[i] = malloc(sizeof(double) * copy->column);

        if (matrix.matrix[i] == NULL) {
            perror("Could not alocate matrix row");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < copy->column; j++)
            matrix.matrix[i][j] = copy->matrix[i][j];
    }
    matrix.row = copy->row;
    matrix.column = copy->column;
    return matrix;
}

float calcul_cell(t_matrix *first, t_matrix *second, int row, int column)
{
    float result = 0;

    for (int i = 0; i < first->column; i++)
        result += first->matrix[row][i] * second->matrix[i][column];
    return result;
}

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

t_matrix diff_matrix(t_matrix *first, t_matrix *second)
{
    t_matrix result;

    if (first->row != second->row && first->column != second->column) {
        perror("The number of columns or rows of the first matrix does not correspond to the number of columns or rows of the second matrix");
        exit(EXIT_FAILURE);
    }
    result = create_empty_matrix(first->row, first->column);
    for (int i = 0; i < first->row; i++) {
        for (int j = 0; j < first->column; j++) {
            result.matrix[i][j] = fabsf(first->matrix[i][j] - second->matrix[i][j]);
        }
    }
    return result;
}

void display_matrix(t_matrix *matrix)
{
    for (int i = 0; i < matrix->row; i++) {
        for (int j = 0; j < matrix->column; j++)
            printf("%.2f ", matrix->matrix[i][j]);
        printf("\n");
    }
}


//// exemple de main :
/*
int main() {
    liste_adjacence la = readGraph("example1.txt");
    t_matrix matrix = create_matrix(&la);
    t_matrix result = copy_matrix(&matrix);
    t_matrix initiate = create_empty_matrix(1, 5);

    initiate.matrix[0][0] = 1;
    display_matrix(&initiate);
    printf("////////////////////////\n");
    display_matrix(&matrix);
    printf("////////////////////////\n");
    display_matrix(&result);
    printf("////////////////////////\n");
    for (int i = 0; i < 2; i++) // Pour M puissance 3
        result = multiply_matrix(&result, &matrix);
    display_matrix(&result);
    printf("////////////////////////\n");
    result = multiply_matrix(&initiate, &result);
    display_matrix(&result);
    printf("////////////////////////\n");
    return 0;
}
*/



//étape2

#include "include.h"
#include <stdio.h>
#include <stdlib.h>

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
            result.matrix[i][j] = matrix.matrix[first->identifiant][second->identifiant];
            j++;
            second = second->next;
        }
        i++;
        first = first->next;
    }
    return result;
}

//// exemple de main :
/*
int main() {
    liste_adjacence la = readGraph("example1.txt");
    t_matrix matrix = create_matrix(&la);
    t_class *class = tarjan_function(&la);
    t_matrix result;
    t_partition part;

    display_matrix(&matrix);
    printf("////////////////////////\n");
    display_classes(class);
    printf("////////////////////////\n");
    printf("////////////////////////\n");
    while (class != NULL) {
        part.head = class->sommet;
        result = subMatrix(matrix, part);
        display_matrix(&result);
        printf("////////////////////////\n");
        class = class->next;
    }
    return 0;
}
*/