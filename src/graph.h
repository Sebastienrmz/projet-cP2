typedef struct cellule {
    int arrivee;
    float proba;
    struct cellule *next;
} cellule_t;

typedef struct {
    cellule_t *head;
} liste;

typedef struct {
    liste *tab;
    int taille;
} liste_adjacence;


#include <stdbool.h>

typedef struct cellule {
    int arrivee;
    float proba;
    struct cellule *next;
} cellule_t;

typedef struct {
    cellule_t *head;
} liste;

typedef struct {
    liste *tab;
    int taille;
} liste_adjacence;

typedef struct tarjan_vertex {
    int identifiant;
    int numero;
    int accessible;
    bool indicateur;
} t_tarjan_vertex;

typedef struct sommet {
    int identifiant;
    struct sommet *next;
} t_sommet;

typedef struct class {
    t_sommet *sommet;
    struct class *next;
} t_class;

typedef struct {
    t_tarjan_vertex *tab;
    t_class *classes;
    liste_adjacence *graph;
} t_tarjan;

typedef struct link {
    int start;
    int end;
    struct link *next;
} t_link;
