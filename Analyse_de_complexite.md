# Analyse des Complexités Temporelles des Algorithmes

## 1. Algorithme de Floyd-Warshall
- **Complexité temporelle** : \(O(V^3)\)
- **Justification** :
  - Initialisation de la matrice des distances : \(O(V^2)\).
  - Trois boucles imbriquées pour mettre à jour les distances : \(O(V^3)\).

---

## 2. Algorithme de Bellman-Ford
- **Complexité temporelle** : \(O(V \times E)\)
- **Justification** :
  - Initialisation des distances : \(O(V)\).
  - Relaxation des arêtes \(V-1\) fois : \(O(V \times E)\).
  - Vérification des cycles de poids négatif : \(O(E)\).

---

## 3. Problème du voyageur de commerce (TSP) - Approche brute
- **Complexité temporelle** : \(O(V! \times V)\)
- **Justification** :
  - Génération de toutes les permutations : \(O(V!)\).
  - Calcul du coût pour chaque permutation : \(O(V)\).

---

## 4. Problème du voyageur de commerce (TSP) - Approche gloutonne
- **Complexité temporelle** : \(O(V^2)\)
- **Justification** :
  - Trouver le sommet le plus proche pour chaque sommet : \(O(V^2)\).

---

## 5. Algorithme génétique pour le TSP
- **Complexité temporelle** : \(O(G \times P \times V)\)
- **Justification** :
  - Initialisation de la population : \(O(P \times V)\).
  - Évaluation de la fitness pour chaque individu : \(O(P \times V)\).
  - Croisement et mutation sur plusieurs générations : \(O(G \times P \times V)\).

---

## 6. Planification des livraisons (Programmation dynamique)
- **Complexité temporelle** : \(O(D \times C \times N)\)
- **Justification** :
  - Initialisation de la table : \(O(D \times C \times N)\).
  - Remplissage de la table : \(O(D \times C \times N)\).

---

## 7. Génération de graphe aléatoire
- **Complexité temporelle** : \(O(E)\)
- **Justification** :
  - Chaque arête est générée en temps constant.

---

## Résumé des Complexités

| **Algorithme**                              | **Complexité temporelle** |
|---------------------------------------------|---------------------------|
| Floyd-Warshall                              | \(O(V^3)\)                |
| Bellman-Ford                                | \(O(V \times E)\)         |
| TSP (Approche brute)                        | \(O(V! \times V)\)        |
| TSP (Approche gloutonne)                    | \(O(V^2)\)                |
| TSP (Algorithme génétique)                  | \(O(G \times P \times V)\)|
| Planification des livraisons (Prog. dyn.)   | \(O(D \times C \times N)\)|
| Génération de graphe aléatoire              | \(O(E)\)                  |