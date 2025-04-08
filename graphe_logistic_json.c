#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "cJSON.h"
#include <limits.h>
#include <float.h> // Pour FLT_MAX

#define INF 1e9 // Représente l'infini

// Structure pour les attributs d’une arête
typedef struct EdgeAttr {
    float distance;       // en kilomètres
    float baseTime;       // en minutes (temps nominal)
    float cost;           // coût monétaire
    int roadType;         // type de route (0: asphalte, 1: latérite, etc.)
    float reliability;    // indice de fiabilité [0,1]
    int restrictions;     // restrictions codées en bits
} EdgeAttr;

// Structure pour un nœud de la liste d’adjacence
typedef struct AdjListNode {
    int dest;                     // identifiant du nœud destination
    EdgeAttr attr;                // attributs de l’arête
    struct AdjListNode* next;     // pointeur vers le prochain nœud
} AdjListNode;

// Structure pour la liste d’adjacence
typedef struct AdjList {
    AdjListNode* head;            // tête de la liste
} AdjList;

// Structure pour le graphe
typedef struct Graph {
    int V;                        // nombre de sommets
    AdjList* array;               // tableau des listes d’adjacence
} Graph;

// Structure pour un nœud du réseau
typedef struct Node {
    int id;
    char name[50];
    char type[20];
    float coordinates[2];
    int capacity;
} Node;

// Fonction pour créer un nœud de la liste d’adjacence
AdjListNode* create_adj_list_node(int dest, EdgeAttr attr) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->attr = attr;
    newNode->next = NULL;
    return newNode;
}

// Fonction pour créer un graphe avec V sommets
Graph* create_graph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;

    // Créer un tableau de listes d’adjacence
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));

    // Initialiser chaque liste d’adjacence comme vide
    for (int i = 0; i < V; i++) {
        graph->array[i].head = NULL;
    }

    return graph;
}

// Fonction pour ajouter une arête au graphe
void add_edge(Graph* graph, int src, int dest, EdgeAttr attr) {
    // Ajouter une arête de src à dest
    AdjListNode* newNode = create_adj_list_node(dest, attr);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Ajouter une arête de dest à src (si le graphe est non orienté)
    newNode = create_adj_list_node(src, attr);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// Fonction pour supprimer une arête du graphe
void remove_edge(Graph* graph, int src, int dest) {
    // Supprimer l'arête de src à dest
    AdjListNode* current = graph->array[src].head;
    AdjListNode* prev = NULL;

    while (current != NULL && current->dest != dest) {
        prev = current;
        current = current->next;
    }

    if (current != NULL) {
        if (prev == NULL) {
            // Le nœud à supprimer est le premier de la liste
            graph->array[src].head = current->next;
        } else {
            // Le nœud à supprimer est au milieu ou à la fin
            prev->next = current->next;
        }
        free(current);
    }

    // Supprimer l'arête de dest à src (si le graphe est non orienté)
    current = graph->array[dest].head;
    prev = NULL;

    while (current != NULL && current->dest != src) {
        prev = current;
        current = current->next;
    }

    if (current != NULL) {
        if (prev == NULL) {
            graph->array[dest].head = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
    }
}

// Fonction pour supprimer un nœud du graphe
void remove_node(Graph* graph, int node) {
    // Supprimer toutes les arêtes pointant vers ce nœud
    for (int v = 0; v < graph->V; v++) {
        if (v != node) {
            remove_edge(graph, v, node);
        }
    }

    // Supprimer toutes les arêtes sortantes de ce nœud
    AdjListNode* current = graph->array[node].head;
    while (current != NULL) {
        AdjListNode* temp = current;
        current = current->next;
        free(temp);
    }
    graph->array[node].head = NULL;

    // Optionnel : Marquer le nœud comme supprimé (si nécessaire)
    // Par exemple, on peut utiliser un tableau booléen pour indiquer si un nœud est actif ou non.
}

// Fonction pour ajuster les attributs d'une arête en fonction de l'heure
void adjust_edge_attributes(Graph* graph, int src, int dest, int hour) {
    AdjListNode* current = graph->array[src].head;

    // Parcourir la liste d'adjacence pour trouver l'arête correspondante
    while (current != NULL) {
        if (current->dest == dest) {
            // Exemple : augmenter le temps et le coût pendant les heures de pointe (7h-9h et 17h-19h)
            if ((hour >= 7 && hour <= 9) || (hour >= 17 && hour <= 19)) {
                current->attr.baseTime *= 1.5;  // Augmenter le temps de 50%
                current->attr.cost *= 1.2;     // Augmenter le coût de 20%
            } else {
                // Remettre les valeurs à leur état nominal en dehors des heures de pointe
                current->attr.baseTime /= 1.5;
                current->attr.cost /= 1.2;
            }
            break;
        }
        current = current->next;
    }
}

// Fonction pour ajuster toutes les arêtes du graphe en fonction de l'heure
void adjust_graph_attributes(Graph* graph, int hour) {
    for (int v = 0; v < graph->V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            // Ajuster les attributs de chaque arête
            adjust_edge_attributes(graph, v, current->dest, hour);
            current = current->next;
        }
    }
}

// Fonction pour afficher le graphe
void print_graph(Graph* graph) {
    for (int v = 0; v < graph->V; v++) {
        AdjListNode* pCrawl = graph->array[v].head;
        printf("Liste d’adjacence du sommet %d\n", v);
        while (pCrawl) {
            printf(" -> %d (distance: %.2f, coût: %.2f, fiabilité: %.2f)\n",
                   pCrawl->dest, pCrawl->attr.distance, pCrawl->attr.cost, pCrawl->attr.reliability);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

// Fonction pour libérer la mémoire du graphe
void free_graph(Graph* graph) {
    for (int v = 0; v < graph->V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current) {
            AdjListNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->array);
    free(graph);
}

// Fonction pour charger un graphe à partir d'un fichier JSON
Graph* load_graph_from_json(const char* filename) {
    // Lire le fichier JSON
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier JSON.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    // Parser le fichier JSON
    cJSON* root = cJSON_Parse(data);
    free(data);

    if (root == NULL) {
        fprintf(stderr, "Erreur : Impossible de parser le fichier JSON.\n");
        return NULL;
    }

    // Récupérer les nœuds
    cJSON* nodes_json = cJSON_GetObjectItem(root, "nodes");
    if (!cJSON_IsArray(nodes_json)) {
        fprintf(stderr, "Erreur : Le fichier JSON ne contient pas un tableau 'nodes'.\n");
        cJSON_Delete(root);
        return NULL;
    }

    int nodeCount = cJSON_GetArraySize(nodes_json);
    Graph* graph = create_graph(nodeCount);

    // Parcourir les nœuds
    cJSON* node_json;
    cJSON_ArrayForEach(node_json, nodes_json) {
        int id = cJSON_GetObjectItem(node_json, "id")->valueint;
        const char* name = cJSON_GetObjectItem(node_json, "name")->valuestring;
        const char* type = cJSON_GetObjectItem(node_json, "type")->valuestring;
        cJSON* coordinates = cJSON_GetObjectItem(node_json, "coordinates");
        int capacity = cJSON_GetObjectItem(node_json, "capacity")->valueint;

        // Ajouter le nœud au graphe (vous pouvez adapter cette partie selon votre structure)
        printf("Nœud %d : %s (%s), capacité : %d, coordonnées : [%f, %f]\n",
               id, name, type, capacity,
               cJSON_GetArrayItem(coordinates, 0)->valuedouble,
               cJSON_GetArrayItem(coordinates, 1)->valuedouble);
    }

    // Récupérer les arêtes
    cJSON* edges_json = cJSON_GetObjectItem(root, "edges");
    if (!cJSON_IsArray(edges_json)) {
        fprintf(stderr, "Erreur : Le fichier JSON ne contient pas un tableau 'edges'.\n");
        cJSON_Delete(root);
        return NULL;
    }

    // Parcourir les arêtes
    cJSON* edge_json;
    cJSON_ArrayForEach(edge_json, edges_json) {
        int source = cJSON_GetObjectItem(edge_json, "source")->valueint;
        int destination = cJSON_GetObjectItem(edge_json, "destination")->valueint;
        EdgeAttr attr;
        attr.distance = cJSON_GetObjectItem(edge_json, "distance")->valuedouble;
        attr.baseTime = cJSON_GetObjectItem(edge_json, "baseTime")->valuedouble;
        attr.cost = cJSON_GetObjectItem(edge_json, "cost")->valuedouble;
        attr.roadType = cJSON_GetObjectItem(edge_json, "roadType")->valueint;
        attr.reliability = cJSON_GetObjectItem(edge_json, "reliability")->valuedouble;
        attr.restrictions = cJSON_GetObjectItem(edge_json, "restrictions")->valueint;

        // Gestion des variations temporelles
        cJSON* timeVariation = cJSON_GetObjectItem(edge_json, "timeVariation");
        if (timeVariation) {
            double morning = cJSON_GetObjectItem(timeVariation, "morning")->valuedouble;
            double afternoon = cJSON_GetObjectItem(timeVariation, "afternoon")->valuedouble;
            double night = cJSON_GetObjectItem(timeVariation, "night")->valuedouble;

            printf("Arête de %d à %d : distance = %.2f, coût = %.2f, variations temporelles (matin = %.2f, après-midi = %.2f, nuit = %.2f)\n",
                   source, destination, attr.distance, attr.cost, morning, afternoon, night);
        }

        // Ajouter l'arête au graphe
        add_edge(graph, source, destination, attr);
    }

    cJSON_Delete(root);
    return graph;
}

// Fonction pour sauvegarder un graphe dans un fichier JSON
void save_graph_to_json(Graph* graph, const char* filename) {
    // Créer l'objet JSON racine
    cJSON* root = cJSON_CreateObject();

    // Ajouter les nœuds au JSON
    cJSON* nodes_json = cJSON_CreateArray();
    for (int v = 0; v < graph->V; v++) {
        cJSON* node_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(node_json, "id", v);

        // Ajouter d'autres informations sur le nœud si nécessaire
        // Exemple : nom, type, coordonnées, capacité
        // cJSON_AddStringToObject(node_json, "name", "NodeName");
        // cJSON_AddStringToObject(node_json, "type", "NodeType");
        // cJSON_AddItemToObject(node_json, "coordinates", cJSON_CreateDoubleArray(coordinates, 2));
        // cJSON_AddNumberToObject(node_json, "capacity", capacity);

        cJSON_AddItemToArray(nodes_json, node_json);
    }
    cJSON_AddItemToObject(root, "nodes", nodes_json);

    // Ajouter les arêtes au JSON
    cJSON* edges_json = cJSON_CreateArray();
    for (int v = 0; v < graph->V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            // Ajouter une arête uniquement dans une direction pour éviter les doublons
            if (v < current->dest) {
                cJSON* edge_json = cJSON_CreateObject();
                cJSON_AddNumberToObject(edge_json, "source", v);
                cJSON_AddNumberToObject(edge_json, "destination", current->dest);
                cJSON_AddNumberToObject(edge_json, "distance", current->attr.distance);
                cJSON_AddNumberToObject(edge_json, "baseTime", current->attr.baseTime);
                cJSON_AddNumberToObject(edge_json, "cost", current->attr.cost);
                cJSON_AddNumberToObject(edge_json, "roadType", current->attr.roadType);
                cJSON_AddNumberToObject(edge_json, "reliability", current->attr.reliability);
                cJSON_AddNumberToObject(edge_json, "restrictions", current->attr.restrictions);

                // Ajouter des variations temporelles si elles existent
                cJSON* timeVariation = cJSON_CreateObject();
                cJSON_AddNumberToObject(timeVariation, "morning", 1.2);  // Exemple de valeur
                cJSON_AddNumberToObject(timeVariation, "afternoon", 1.5);
                cJSON_AddNumberToObject(timeVariation, "night", 0.9);
                cJSON_AddItemToObject(edge_json, "timeVariation", timeVariation);

                cJSON_AddItemToArray(edges_json, edge_json);
            }
            current = current->next;
        }
    }
    cJSON_AddItemToObject(root, "edges", edges_json);

    // Convertir l'objet JSON en chaîne de caractères
    char* json_string = cJSON_Print(root);

    // Écrire la chaîne JSON dans un fichier
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible de créer le fichier JSON.\n");
        cJSON_Delete(root);
        free(json_string);
        return;
    }
    fprintf(file, "%s", json_string);
    fclose(file);

    // Libérer la mémoire
    cJSON_Delete(root);
    free(json_string);

    printf("Graphe sauvegardé dans le fichier '%s'.\n", filename);
}

// Fonction auxiliaire pour détecter les cycles
bool detect_cycle_util(Graph* graph, int v, bool* visited, int parent) {
    visited[v] = true;

    AdjListNode* current = graph->array[v].head;
    while (current != NULL) {
        int adj = current->dest;

        // Si le sommet adjacent n'a pas été visité, continuer la DFS
        if (!visited[adj]) {
            if (detect_cycle_util(graph, adj, visited, v)) {
                return true;
            }
        }
        // Si le sommet adjacent est visité et n'est pas le parent, un cycle est détecté
        else if (adj != parent) {
            return true;
        }

        current = current->next;
    }

    return false;
}

// Fonction principale pour détecter les cycles
bool detect_cycle(Graph* graph) {
    bool* visited = (bool*)calloc(graph->V, sizeof(bool));

    for (int v = 0; v < graph->V; v++) {
        if (!visited[v]) {
            if (detect_cycle_util(graph, v, visited, -1)) {
                free(visited);
                return true;
            }
        }
    }

    free(visited);
    return false;
}

// Fonction auxiliaire pour effectuer une DFS
void dfs_util(Graph* graph, int v, bool* visited) {
    visited[v] = true;

    AdjListNode* current = graph->array[v].head;
    while (current != NULL) {
        int adj = current->dest;
        if (!visited[adj]) {
            dfs_util(graph, adj, visited);
        }
        current = current->next;
    }
}

// Fonction principale pour compter les composantes connexes
int count_connected_components(Graph* graph) {
    bool* visited = (bool*)calloc(graph->V, sizeof(bool));
    int count = 0;

    for (int v = 0; v < graph->V; v++) {
        if (!visited[v]) {
            count++;
            dfs_util(graph, v, visited);
        }
    }

    free(visited);
    return count;
}

// Fonction auxiliaire pour détecter les points d'articulation
void articulation_points_util(Graph* graph, int u, bool* visited, int* disc, int* low, int* parent, bool* ap, int* time) {
    int children = 0;
    visited[u] = true;

    disc[u] = low[u] = ++(*time);

    AdjListNode* current = graph->array[u].head;
    while (current != NULL) {
        int v = current->dest;

        if (!visited[v]) {
            children++;
            parent[v] = u;
            articulation_points_util(graph, v, visited, disc, low, parent, ap, time);

            // Mise à jour de la valeur low
            low[u] = (low[u] < low[v]) ? low[u] : low[v];

            // Vérifier si u est un point d'articulation
            if (parent[u] == -1 && children > 1) {
                ap[u] = true;
            }
            if (parent[u] != -1 && low[v] >= disc[u]) {
                ap[u] = true;
            }
        } else if (v != parent[u]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }

        current = current->next;
    }
}

// Fonction principale pour détecter les points d'articulation
void find_articulation_points(Graph* graph) {
    bool* visited = (bool*)calloc(graph->V, sizeof(bool));
    int* disc = (int*)calloc(graph->V, sizeof(int));
    int* low = (int*)calloc(graph->V, sizeof(int));
    int* parent = (int*)calloc(graph->V, sizeof(int));
    bool* ap = (bool*)calloc(graph->V, sizeof(bool));
    int time = 0;

    for (int i = 0; i < graph->V; i++) {
        parent[i] = -1;
        visited[i] = false;
        ap[i] = false;
    }

    for (int i = 0; i < graph->V; i++) {
        if (!visited[i]) {
            articulation_points_util(graph, i, visited, disc, low, parent, ap, &time);
        }
    }

    printf("Points d'articulation :\n");
    for (int i = 0; i < graph->V; i++) {
        if (ap[i]) {
            printf("%d ", i);
        }
    }
    printf("\n");

    free(visited);
    free(disc);
    free(low);
    free(parent);
    free(ap);
}

// Fonction pour calculer le degré moyen des sommets
float calculate_average_degree(Graph* graph) {
    int total_degree = 0;

    for (int v = 0; v < graph->V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            total_degree++;
            current = current->next;
        }
    }

    // Diviser par le nombre de sommets
    return (float)total_degree / graph->V;
}

// Fonction pour calculer la densité du graphe
float calculate_density(Graph* graph) {
    int edge_count = 0;

    for (int v = 0; v < graph->V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            edge_count++;
            current = current->next;
        }
    }

    // Diviser par 2 car chaque arête est comptée deux fois dans un graphe non orienté
    edge_count /= 2;

    int V = graph->V;
    return (float)(2 * edge_count) / (V * (V - 1));
}

// Fonction pour afficher les statistiques de connectivité
void print_connectivity_statistics(Graph* graph) {
    int connected_components = count_connected_components(graph);
    float average_degree = calculate_average_degree(graph);
    float density = calculate_density(graph);

    printf("Statistiques sur la connectivité du graphe :\n");
    printf("Nombre de composantes connexes : %d\n", connected_components);
    printf("Degré moyen des sommets : %.2f\n", average_degree);
    printf("Densité du graphe : %.4f\n", density);
}

// Fonction pour appliquer l'algorithme de Floyd-Warshall
void floyd_warshall(Graph* graph, float** dist) {
    int V = graph->V;

    // Initialiser la matrice des distances
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else {
                dist[i][j] = INF;
            }
        }

        AdjListNode* current = graph->array[i].head;
        while (current != NULL) {
            dist[i][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    // Appliquer l'algorithme de Floyd-Warshall
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

// Fonction pour calculer le coût total d'un chemin donné
float calculate_path_cost(int* path, int V, float** dist) {
    float cost = 0;
    for (int i = 0; i < V - 1; i++) {
        cost += dist[path[i]][path[i + 1]];
    }
    cost += dist[path[V - 1]][path[0]]; // Retour au point de départ
    return cost;
}

// Fonction pour générer toutes les permutations des sommets
void tsp_permute(int* path, int l, int r, float** dist, float* min_cost, int* best_path, int V) {
    if (l == r) {
        float cost = calculate_path_cost(path, V, dist);
        if (cost < *min_cost) {
            *min_cost = cost;
            memcpy(best_path, path, V * sizeof(int));
        }
        return;
    }

    for (int i = l; i <= r; i++) {
        // Échanger les sommets
        int temp = path[l];
        path[l] = path[i];
        path[i] = temp;

        tsp_permute(path, l + 1, r, dist, min_cost, best_path, V);

        // Revenir à l'état précédent
        temp = path[l];
        path[l] = path[i];
        path[i] = temp;
    }
}

// Fonction principale pour résoudre le TSP
void solve_tsp(Graph* graph) {
    int V = graph->V;

    // Calculer la matrice des distances avec Floyd-Warshall
    float** dist = (float**)malloc(V * sizeof(float*));
    for (int i = 0; i < V; i++) {
        dist[i] = (float*)malloc(V * sizeof(float));
    }
    floyd_warshall(graph, dist);

    // Initialiser les variables pour le TSP
    int* path = (int*)malloc(V * sizeof(int));
    int* best_path = (int*)malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) {
        path[i] = i;
    }

    float min_cost = INF;

    // Trouver le chemin optimal
    tsp_permute(path, 0, V - 1, dist, &min_cost, best_path, V);

    // Afficher le résultat
    printf("Chemin optimal : ");
    for (int i = 0; i < V; i++) {
        printf("%d -> ", best_path[i]);
    }
    printf("%d\n", best_path[0]);
    printf("Coût minimal : %.2f\n", min_cost);

    // Libérer la mémoire
    for (int i = 0; i < V; i++) {
        free(dist[i]);
    }
    free(dist);
    free(path);
    free(best_path);
}

// Fonction pour ajuster les poids des arêtes en fonction des contraintes temporelles
float adjust_edge_weight(EdgeAttr attr, int hour) {
    float adjusted_weight = attr.distance; // Poids de base : distance

    // Ajuster le poids en fonction des heures de pointe
    if ((hour >= 7 && hour <= 9) || (hour >= 17 && hour <= 19)) {
        adjusted_weight *= 1.5; // Augmenter le poids de 50% pendant les heures de pointe
    } else if (hour >= 22 || hour <= 5) {
        adjusted_weight *= 0.8; // Réduire le poids de 20% pendant la nuit
    }

    // Ajouter un coût variable (par exemple, coût monétaire)
    adjusted_weight += attr.cost / 1000.0; // Normaliser le coût pour l'ajouter au poids

    return adjusted_weight;
}

// Fonction principale de Bellman-Ford
bool bellman_ford(Graph* graph, int src, int hour, float* dist) {
    int V = graph->V;
    int E = 0; // Nombre d'arêtes (sera calculé)

    // Initialiser les distances à l'infini
    for (int i = 0; i < V; i++) {
        dist[i] = FLT_MAX;
    }
    dist[src] = 0; // La distance à la source est 0

    // Compter le nombre total d'arêtes
    for (int v = 0; v < V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            E++;
            current = current->next;
        }
    }

    // Relaxer les arêtes V-1 fois
    for (int i = 1; i <= V - 1; i++) {
        for (int v = 0; v < V; v++) {
            AdjListNode* current = graph->array[v].head;
            while (current != NULL) {
                int u = v;
                int dest = current->dest;
                float weight = adjust_edge_weight(current->attr, hour);

                if (dist[u] != FLT_MAX && dist[u] + weight < dist[dest]) {
                    dist[dest] = dist[u] + weight;
                }

                current = current->next;
            }
        }
    }

    // Vérifier les cycles de poids négatif
    for (int v = 0; v < V; v++) {
        AdjListNode* current = graph->array[v].head;
        while (current != NULL) {
            int u = v;
            int dest = current->dest;
            float weight = adjust_edge_weight(current->attr, hour);

            if (dist[u] != FLT_MAX && dist[u] + weight < dist[dest]) {
                printf("Le graphe contient un cycle de poids négatif.\n");
                return false;
            }

            current = current->next;
        }
    }

    return true;
}

// Fonction pour trouver le sommet le plus proche non visité
int find_nearest_neighbor(int current, bool* visited, float** dist, int V) {
    int nearest = -1;
    float min_distance = FLT_MAX;

    for (int i = 0; i < V; i++) {
        if (!visited[i] && dist[current][i] < min_distance) {
            min_distance = dist[current][i];
            nearest = i;
        }
    }

    return nearest;
}

// Approche gloutonne pour résoudre le TSP
void greedy_tsp(Graph* graph) {
    int V = graph->V;

    // Calculer la matrice des distances avec Floyd-Warshall
    float** dist = (float**)malloc(V * sizeof(float*));
    for (int i = 0; i < V; i++) {
        dist[i] = (float*)malloc(V * sizeof(float));
    }
    floyd_warshall(graph, dist);

    // Initialiser les variables
    bool* visited = (bool*)calloc(V, sizeof(bool));
    int* path = (int*)malloc(V * sizeof(int));
    float total_cost = 0;

    int current = 0; // Commencer par le sommet 0
    visited[current] = true;
    path[0] = current;

    for (int i = 1; i < V; i++) {
        int next = find_nearest_neighbor(current, visited, dist, V);
        if (next == -1) {
            printf("Erreur : Aucun sommet accessible.\n");
            return;
        }

        total_cost += dist[current][next];
        visited[next] = true;
        path[i] = next;
        current = next;
    }

    // Retour au point de départ
    total_cost += dist[current][path[0]];

    // Afficher le chemin et le coût total
    printf("Chemin glouton : ");
    for (int i = 0; i < V; i++) {
        printf("%d -> ", path[i]);
    }
    printf("%d\n", path[0]);
    printf("Coût total : %.2f\n", total_cost);

    // Libérer la mémoire
    for (int i = 0; i < V; i++) {
        free(dist[i]);
    }
    free(dist);
    free(visited);
    free(path);
}

// Structure pour représenter une livraison
typedef struct {
    int cost;       // Coût de la livraison
    int time;       // Temps requis pour la livraison
    int priority;   // Priorité de la livraison
    int window;     // Fenêtre temporelle (jour où la livraison doit être effectuée)
} Delivery;

// Fonction pour planifier les livraisons
int plan_deliveries(Delivery* deliveries, int n, int days, int capacity) {
    // Table de programmation dynamique
    int*** dp = (int***)malloc((days + 1) * sizeof(int**));
    for (int d = 0; d <= days; d++) {
        dp[d] = (int**)malloc((capacity + 1) * sizeof(int*));
        for (int c = 0; c <= capacity; c++) {
            dp[d][c] = (int*)malloc((n + 1) * sizeof(int));
            for (int i = 0; i <= n; i++) {
                dp[d][c][i] = INF; // Initialiser à l'infini
            }
        }
    }

    // Initialisation : coût nul si aucune livraison n'est effectuée
    for (int d = 0; d <= days; d++) {
        for (int c = 0; c <= capacity; c++) {
            dp[d][c][0] = 0;
        }
    }

    // Remplir la table de programmation dynamique
    for (int d = 1; d <= days; d++) {
        for (int c = 0; c <= capacity; c++) {
            for (int i = 1; i <= n; i++) {
                // Ne pas effectuer la livraison i
                dp[d][c][i] = dp[d][c][i - 1];

                // Effectuer la livraison i si elle est dans la fenêtre temporelle et si la capacité le permet
                if (deliveries[i - 1].window == d && deliveries[i - 1].time <= c) {
                    int remaining_capacity = c - deliveries[i - 1].time;
                    int cost_with_delivery = dp[d][remaining_capacity][i - 1] + deliveries[i - 1].cost;

                    if (cost_with_delivery < dp[d][c][i]) {
                        dp[d][c][i] = cost_with_delivery;
                    }
                }
            }
        }
    }

    // Trouver le coût minimal pour toutes les livraisons
    int min_cost = INF;
    for (int d = 1; d <= days; d++) {
        for (int c = 0; c <= capacity; c++) {
            if (dp[d][c][n] < min_cost) {
                min_cost = dp[d][c][n];
            }
        }
    }

    // Libérer la mémoire
    for (int d = 0; d <= days; d++) {
        for (int c = 0; c <= capacity; c++) {
            free(dp[d][c]);
        }
        free(dp[d]);
    }
    free(dp);

    return min_cost;
}

#include <time.h>

// Structure pour représenter un individu
typedef struct {
    int* path;       // Chemin (ordre des sommets)
    float fitness;   // Coût total du chemin
} Individual;

// Fonction pour calculer le coût d'un chemin
float calculate_fitness(int* path, int V, float** dist) {
    float cost = 0;
    for (int i = 0; i < V - 1; i++) {
        cost += dist[path[i]][path[i + 1]];
    }
    cost += dist[path[V - 1]][path[0]]; // Retour au point de départ
    return cost;
}

// Fonction pour générer un individu aléatoire
void generate_random_individual(Individual* individual, int V) {
    for (int i = 0; i < V; i++) {
        individual->path[i] = i;
    }

    // Mélanger les sommets pour créer un chemin aléatoire
    for (int i = V - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = individual->path[i];
        individual->path[i] = individual->path[j];
        individual->path[j] = temp;
    }
}

// Fonction pour effectuer un croisement entre deux individus
void crossover(Individual* parent1, Individual* parent2, Individual* child, int V) {
    bool* visited = (bool*)calloc(V, sizeof(bool));

    // Copier la première moitié du chemin du parent1
    for (int i = 0; i < V / 2; i++) {
        child->path[i] = parent1->path[i];
        visited[parent1->path[i]] = true;
    }

    // Compléter avec les sommets restants du parent2
    int index = V / 2;
    for (int i = 0; i < V; i++) {
        if (!visited[parent2->path[i]]) {
            child->path[index++] = parent2->path[i];
        }
    }

    free(visited);
}

// Fonction pour effectuer une mutation sur un individu
void mutate(Individual* individual, int V) {
    int i = rand() % V;
    int j = rand() % V;

    // Échanger deux sommets
    int temp = individual->path[i];
    individual->path[i] = individual->path[j];
    individual->path[j] = temp;
}

// Algorithme génétique pour résoudre le TSP
void genetic_tsp(Graph* graph, int population_size, int generations) {
    int V = graph->V;

    // Calculer la matrice des distances avec Floyd-Warshall
    float** dist = (float**)malloc(V * sizeof(float*));
    for (int i = 0; i < V; i++) {
        dist[i] = (float*)malloc(V * sizeof(float));
    }
    floyd_warshall(graph, dist);

    // Initialiser la population
    Individual* population = (Individual*)malloc(population_size * sizeof(Individual));
    for (int i = 0; i < population_size; i++) {
        population[i].path = (int*)malloc(V * sizeof(int));
        generate_random_individual(&population[i], V);
        population[i].fitness = calculate_fitness(population[i].path, V, dist);
    }

    // Évolution sur plusieurs générations
    for (int gen = 0; gen < generations; gen++) {
        // Trier la population par fitness (coût croissant)
        for (int i = 0; i < population_size - 1; i++) {
            for (int j = i + 1; j < population_size; j++) {
                if (population[i].fitness > population[j].fitness) {
                    Individual temp = population[i];
                    population[i] = population[j];
                    population[j] = temp;
                }
            }
        }

        // Croisement et mutation pour créer la nouvelle génération
        for (int i = population_size / 2; i < population_size; i++) {
            crossover(&population[i - population_size / 2], &population[(i + 1) % (population_size / 2)], &population[i], V);
            if (rand() % 100 < 10) { // 10% de chance de mutation
                mutate(&population[i], V);
            }
            population[i].fitness = calculate_fitness(population[i].path, V, dist);
        }
    }

    // Afficher le meilleur chemin
    printf("Chemin optimal (algorithme génétique) : ");
    for (int i = 0; i < V; i++) {
        printf("%d -> ", population[0].path[i]);
    }
    printf("%d\n", population[0].path[0]);
    printf("Coût minimal : %.2f\n", population[0].fitness);

    // Libérer la mémoire
    for (int i = 0; i < population_size; i++) {
        free(population[i].path);
    }
    free(population);
    for (int i = 0; i < V; i++) {
        free(dist[i]);
    }
    free(dist);
}

// Fonction pour générer un graphe aléatoire
Graph* generate_random_graph(int num_nodes, int num_edges) {
    Graph* graph = create_graph(num_nodes);

    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires

    for (int i = 0; i < num_edges; i++) {
        int src = rand() % num_nodes;
        int dest = rand() % num_nodes;

        // Éviter les boucles et les doublons
        if (src == dest) continue;

        EdgeAttr attr;
        attr.distance = (float)(rand() % 100 + 1); // Distance entre 1 et 100 km
        attr.baseTime = (float)(rand() % 120 + 10); // Temps entre 10 et 120 minutes
        attr.cost = (float)(rand() % 1000 + 100); // Coût entre 100 et 1000 unités
        attr.roadType = rand() % 2; // Type de route : 0 ou 1
        attr.reliability = (float)(rand() % 100) / 100.0; // Fiabilité entre 0.0 et 1.0
        attr.restrictions = rand() % 4; // Restrictions codées en bits

        add_edge(graph, src, dest, attr);
    }

    return graph;
}

// Fonction pour générer et sauvegarder un graphe dans un fichier JSON
void generate_and_save_graph_to_json(const char* filename, int num_nodes, int num_edges) {
    Graph* graph = generate_random_graph(num_nodes, num_edges);

    // Sauvegarder le graphe dans un fichier JSON
    save_graph_to_json(graph, filename);

    printf("Graphe généré et sauvegardé dans le fichier '%s'.\n", filename);

    free_graph(graph);
}

// Fonction pour générer des jeux de données de test et les sauvegarder dans des fichiers JSON
void generate_test_datasets_to_json() {
    printf("Génération des jeux de données et sauvegarde dans des fichiers JSON...\n");

    // Petit réseau (10-20 nœuds)
    printf("\nPetit réseau (10-20 nœuds) :\n");
    generate_and_save_graph_to_json("small_graph.json", 15, 30);

    // Réseau moyen (100-150 nœuds)
    printf("\nRéseau moyen (100-150 nœuds) :\n");
    generate_and_save_graph_to_json("medium_graph.json", 120, 300);

    // Grand réseau (200+ nœuds)
    printf("\nGrand réseau (200+ nœuds) :\n");
    generate_and_save_graph_to_json("large_graph.json", 250, 1000);
}

// Fonction pour appliquer un scénario au graphe
void apply_scenario(Graph* graph, const char* scenario) {
    if (strcmp(scenario, "Jour normal d’activité") == 0) {
        adjust_graph_attributes(graph, 12); // Exemple : ajuster pour midi
    } else if (strcmp(scenario, "Période de pointe") == 0) {
        adjust_graph_attributes(graph, 8); // Exemple : ajuster pour 8h
    } else if (strcmp(scenario, "Situation de crise") == 0) {
        for (int v = 0; v < graph->V; v++) {
            AdjListNode* current = graph->array[v].head;
            while (current != NULL) {
                current->attr.reliability *= 0.5; // Réduire la fiabilité de moitié
                current = current->next;
            }
        }
    }
}

// Exemple d'utilisation
int main() {
    // Générer les jeux de données et les sauvegarder dans des fichiers JSON
    generate_test_datasets_to_json();

    // Charger un petit graphe pour tester les scénarios
    Graph* test_graph = load_graph_from_json("small_graph.json");

    // Appliquer les scénarios
    apply_scenario(test_graph, "Jour normal d’activité");
    print_graph(test_graph);

    apply_scenario(test_graph, "Période de pointe");
    print_graph(test_graph);

    apply_scenario(test_graph, "Situation de crise");
    print_graph(test_graph);

    free_graph(test_graph);

    return 0;
}