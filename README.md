# Graphes et chaînes de Markov en C

## Essayer rapidement sous Windows

**[Télécharger la version Windows](https://github.com/Sebastienrmz/projet-cP2/releases/latest)**

Télécharger le fichier **Graphes-Markov-Windows.zip** dans la rubrique **Assets**, faire **Extraire tout**, puis double-cliquer sur **Tester.bat**. Le programme et ses ressources sont fournis ensemble : aucun chemin à configurer.

Aucun compilateur nécessaire pour cette version. Les instructions ci-dessous permettent aussi de compiler les sources.

Projet de cours sur les graphes probabilistes, les composantes fortement connexes et les matrices de transition.

Le programme charge un graphe depuis un fichier texte. Le menu permet d'afficher sa liste d'adjacence, de vérifier les sommes des probabilités, de trouver les classes avec Tarjan et de calculer des puissances de la matrice de transition.

## Lancer le projet

Avec un compilateur C17 et CMake 3.16 ou plus récent :

```sh
cmake -S . -B build
cmake --build build
```

Depuis la racine du dépôt :

```sh
./build/markov_graph data/proba
```

Sous Windows : ` .\build\markov_graph.exe data/proba`.
Avec Visual Studio, l'exécutable se trouve généralement dans `build/Debug/`.
Dans CLion, choisir la racine du dépôt comme répertoire de travail.

Les autres exemples sont `data/exemple1`, `data/exemple2` et `data/exemple3`.

## Format des données

```text
3
1 1 0.5
1 2 0.5
2 3 1
3 3 1
```

La première ligne indique le nombre de sommets. Chaque ligne suivante contient le départ, l'arrivée et la probabilité d'un arc. Les sommets sont numérotés à partir de 1. Les probabilités doivent être finies et comprises entre 0 et 1. Les arcs de probabilité nulle ne sont pas ajoutés. Les arcs répétés sont additionnés dans la matrice.

## Organisation

- `src/main.c` : menu et appels des différentes parties.
- `src/graph.h` : structures et prototypes.
- `src/graph.c` : listes d'adjacence, Tarjan, classes et calcul matriciel.
- `data/` : graphes fournis avec le projet.

Les options 3 et 5 écrivent `graph.mmd` et `hasse.mmd` dans le dossier courant. Ce sont des fichiers Mermaid à ouvrir avec un visualiseur compatible. Le second représente les liens entre classes : il ne réalise pas de réduction transitive complète du diagramme de Hasse.

## Points à connaître

Tarjan utilise un parcours en profondeur, une pile et les indices d'accessibilité pour regrouper les sommets qui communiquent. La matrice stocke les probabilités de passage d'un sommet à l'autre. Le calcul de `M^n` conserve les multiplications successives du projet initial ; `M^0` est l'identité.

La vérification de Markov conserve la tolérance de 0,01 utilisée dans les exemples arrondis. Les informations « transitoire » et « persistante » supposent que le graphe est bien une chaîne de Markov : utiliser d'abord l'option 2. Le programme ne normalise pas les données à votre place.

Les graphes sont limités à 1 000 sommets et la puissance saisie au menu à 1 000. Le parcours récursif et les matrices denses conviennent surtout aux exemples de cours. Les fonctions de convergence présentes dans `graph.c` ne sont pas proposées par le menu et ne constituent pas un calcul général de distribution stationnaire.

Travail collectif : Sébastien Ramirez et les contributeurs du dépôt, dont Raphaël Derobien.
