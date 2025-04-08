# Guide d'exécution de l'algorithme en local

Ce guide explique comment configurer votre environnement et exécuter le programme en local en utilisant **MSYS2** pour compiler et exécuter le fichier `.c`.

---

## **1. Préparer l'environnement**

### **1.1 Installer MSYS2**
1. Téléchargez MSYS2 depuis le site officiel : [https://www.msys2.org/](https://www.msys2.org/).
2. Installez MSYS2 en suivant les instructions fournies sur le site.

### **1.2 Mettre à jour MSYS2**
1. Ouvrez l'application **MSYS2 MSYS**.
2. Exécutez les commandes suivantes pour mettre à jour les packages :
   ```bash
   pacman -Syu
3. Installez le compilateur GCC et d'autres outils nécessaires avec la commande suivante :
    ```bash
    pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
4. Vérifiez que GCC est installé en exécutant :
    ```bash
    gcc --version
### **1.3 Installer la bibliothèque cJSON**
1. Téléchargez la bibliothèque cJSON depuis son dépôt officiel : [https://github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON).

2. Placez les fichiers cJSON.c et cJSON.h dans le même répertoire que votre fichier graphe_logistic_json.c

---

## **2. Compiler et exécuter le programme**

### **2.1 Compiler le fichier**
1. Placez le fichier graphe_logistic_json.c dans un répertoire de votre choix.

2. Ouvrez MSYS2 MinGW (pas MSYS2 MSYS).

3. Naviguez jusqu'au répertoire contenant le fichier avec la commande cd. Par exemple :
    ```bash
    cd /c/chemin/vers/le/dossier

4. Compilez le fichier avec la commande suivante :
    ```bash
    gcc graphe_logistic_json.c cJSON.c -o graphe_logistic_json
    ````
    - graphe_logistic_json.c : Le fichier source principal.
    - cJSON.c : Le fichier source de la bibliothèque cJSON.
    - -o graphe_logistic_json : Spécifie le nom de l'exécutable généré.

### **2.2 Exécuter le programme**
1. Une fois la compilation terminée, exécutez le programme avec la commande suivante :
    ```bash
    ./graphe_logistic_json

2. Le programme générera les fichiers JSON pour les jeux de données et affichera les résultats des scénarios dans la console.

---

## **3. Résultats attendus**
### **3.1 Exécuter le programme**

Le programme génère trois fichiers JSON dans le répertoire courant :

- *small_graph.json* : Petit réseau (10-20 nœuds).
- *medium_graph.json* : Réseau moyen (100-150 nœuds).
- *large_graph.json* : Grand réseau (200+ nœuds).

### **3.2 Affichage des scénarios**

Le programme applique trois scénarios réalistes :

1. **Jour normal d’activité** : Le graphe reste inchangé.
2. **Période de pointe** : Les temps et les coûts des arêtes augmentent.
3. **Situation de crise** : La fiabilité des arêtes est réduite pour simuler des routes coupées.

Exemple de sortie console :
```bash
    Génération des jeux de données et sauvegarde dans des fichiers JSON...
    Petit réseau (10-20 nœuds) :
    Graphe généré et sauvegardé dans le fichier 'small_graph.json'.

    Réseau moyen (100-150 nœuds) :
    Graphe généré et sauvegardé dans le fichier 'medium_graph.json'.

    Grand réseau (200+ nœuds) :
    Graphe généré et sauvegardé dans le fichier 'large_graph.json'.

    Application du scénario : Jour normal d’activité
    Liste d’adjacence du sommet 0
    -> 1 (distance: 45.00, coût: 500.00, fiabilité: 0.85)
````
---

### **Explications**
1. Ce fichier `README.md` guide l'utilisateur pour configurer **MSYS2**, compiler le fichier `.c`, et exécuter le programme en local.
2. Il inclut des étapes détaillées pour installer les dépendances, compiler le code, et résoudre les problèmes courants.
3. Les résultats attendus (fichiers JSON et affichage des scénarios) sont également décrits pour que l'utilisateur puisse vérifier le bon fonctionnement du programme.