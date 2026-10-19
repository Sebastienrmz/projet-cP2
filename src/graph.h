#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stdio.h>

/* cellule d'une liste d'adjacence, représente un arc avec son sommet d'arrivée et sa probabilité */
typedef struct cellule {
    int arrivee;
    float proba;
    struct cellule *next;
} cellule_t;

/* liste chainée utilisée pour stocker les cellules */
typedef struct {
    cellule_t *head;
} liste;

/* structure du graphe en tableau de listes d'adjacence */
typedef struct {
    liste *tab;
    int taille;
} liste_adjacence;

/* structure utilisée par Tarjan pour suivre l'état d'un sommet */
typedef struct tarjan_vertex {
    int identifiant;
    int numero;
    int accessible;
    bool indicateur;  /* vrai si le sommet est sur la pile */
} t_tarjan_vertex;

/* liste chainee contenant les sommets d'une classe */
typedef struct sommet {
    int identifiant;
    struct sommet *next;
} t_sommet;

/* structure représentant une classe de sommets fortement connexes */
typedef struct class {
    t_sommet *sommet;
    struct class *next;
} t_class;

/* contient les données de travail pour Tarjan */
typedef struct {
    t_tarjan_vertex *tab;
    t_class *classes;
    liste_adjacence *graph;
} t_tarjan;

/* lien entre classes pour construire le diagramme de Hasse */
typedef struct link {
    int start;
    int end;
    struct link *next;
} t_link;

/* structure utilisée pour extraire une sous-matrice selon une composante */
typedef struct partition {
    t_sommet *head;
} t_partition;

/* structure pour les matrices utilisées dans la partie 3 */
typedef struct {
    double **matrix;
    int row;
    int column;
} t_matrix;


/* prototypes partie 1 */
cellule_t *create_cell(int arrivee, float proba);
liste create_empty_list(void);
void add_cell(liste *list, int arrivee, float proba);
void display_list(liste *list);
liste_adjacence create_empty_liste_adjacence(int nbvert);
void display_liste_adjacence(liste_adjacence *la);
liste_adjacence readGraph(const char *filename);
void valid_graph(liste_adjacence *list);
void graph_cell(liste *list, int id, FILE *file);
void draw_graph(liste_adjacence *list, char *filename);

/* prototypes partie 2 */
t_tarjan_vertex *transform_graph(liste_adjacence *list);
void add_class(t_class **head, t_sommet *list);
void add_sommet(t_sommet **head, int identifiant);
int delete_sommet(t_sommet **head);
void parcours(int v, t_tarjan *tarjan, t_sommet **sommet, int *num);
t_class *tarjan_function(liste_adjacence *graph);
void display_classes(t_class *classes);
int *create_tab_class(liste_adjacence *graph, t_class *classes);
void add_link(t_link **link, int start, int end);
t_link *get_class_link(int *tab_class, liste_adjacence *graph);
void draw_label(t_class *cls, int index, FILE *file);
void draw_classes_link(t_link *link, t_class *classes, char *filename);
bool can_exist(int class_index, t_link *link);
bool is_lonely(t_sommet *list);
bool is_irreductible(t_class *classes);
void display_sommet(t_sommet *sommet, bool and);
void display_information(t_class *classes, t_link *link);

/* prototypes partie 3 */
t_matrix create_empty_matrix(int row, int column);
t_matrix create_matrix(liste_adjacence *graph);
t_matrix copy_matrix(t_matrix *copy);
double calcul_cell(t_matrix *first, t_matrix *second, int row, int column);
t_matrix multiply_matrix(t_matrix *first, t_matrix *second);
t_matrix diff_matrix(t_matrix *first, t_matrix *second);
void display_matrix(t_matrix *matrix);
int len_sommet(t_sommet *sommet);
t_matrix subMatrix(t_matrix matrix, t_partition part);
t_matrix identity_matrix(int n);
t_matrix power_matrix(t_matrix *M, int n);

void free_graph(liste_adjacence *graph);
void free_classes(t_class *classes);
void free_links(t_link *links);
void free_matrix(t_matrix *matrix);

#endif
