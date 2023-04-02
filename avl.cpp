/*
Instructions :

1 - Compléter le code ci-dessous pour avoir une implémentation
d'un arbre de type AVL. 

2 - Faire des tests d'insertion, de suppression et de recherche (penser à faire une fonction d’affichage). 
Faire en sorte d’avoir toutes les possibilités dans vos tests.

3 – Considérer le fichier "avl_bench.cpp". Il génère des ensembles de benchmarking avec des tailles d’arbre différentes : 
de 2^5 éléments dans l’arbre à 2^25 éléments. Pour chaque taille, il donne 3 fichiers :
Values, Search et Delete, contenant respectivement les valeurs à insérer dans l’arbre, 
les valeurs à rechercher, et celles à supprimer. Ajouter dans ce fichier (avl.cpp) les codes nécessaire pour lire 
les fichiers de benchmark et effectuer les opérations correspondantes.

4 – Ajouter du code pour chronométrer les opérations. Réaliser des courbes comparatives 
des temps d’exécutions des différentes opération en fonction de la taille de l’arbre. 
Retrouvons-nous bien les complexité logarithmiques promises ?

*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <ctime>
#include <ratio>


using namespace std;

// Définition du type noeud de l'arbre ...
typedef struct node {
    struct node *left;
    uint64_t data;
    int height;
    struct node *right;

} node;

bool hasRight(node *n){
    return n->right != nullptr;
}

bool hasLeft(node *n){
    return n->left != nullptr;
}

bool isLeaf(node *n){
    return !hasLeft(n) && !hasRight(n);
}

int calcHeight(node *n) {
    if (isLeaf(n)){
        return 1;
    }
    int h = 0;
    if (hasLeft(n)) {
        h = n->left->height;
    }
    if (hasRight(n)) {
        h = max(h, n->right->height);
    }
    return h + 1;
}

int calcHeightRecursive(node* n) {
    if (n == nullptr) {
        return 0;
    }
    if (isLeaf(n)) {
        n->height = 1;
        return 1;
    }
	int h = 0;
	if (hasLeft(n)) {
		h = calcHeightRecursive(n->left);
	}
	if (hasRight(n)) {
		h = max(h, calcHeightRecursive(n->right));
	}
	n->height = h + 1;
	return h + 1;
}


int bf(node *n){ // Balance Factor BF
    int bf = 0;
    // Calcul le BF au niveau du noeud n dans l'arbre

    if (isLeaf(n)) {
        bf = 0;
    }
    else {
        if (hasLeft(n)) {
            bf += n->left->height;
        }
        if (hasRight(n)) {
            bf -= n->right->height;
        }
    }
    return bf;
}

node* createNode(uint64_t data){
    node* n = new node();
    n->height = 1;
    n->data = data;
    n->right = nullptr;
    n->left = nullptr;
    return n;
}

node* llrotation(node *n){ // Rotation LL
    node *p;
    node *tp;
    p = n;
    tp = p->left;

    p->left = tp->right;
    tp->right = p;

    return tp; 
}


node* rrrotation(node *n){ // Rotation RR
    node *p;
    node *tp;
    p = n;
    tp = p->right;

    p->right = tp->left;
    tp->left = p;

    return tp; 
}


node* rlrotation(node *n){
    node *p;
    node *tp;
    p = n;

    // effectue la rotation RL
    tp = p->left->right;
    p->left->right = tp->left;
    tp->left = p->left;
    p->left = tp->right;
    tp->right = p;


    return tp; 
}

node* lrrotation(node *n){
    node* p;
    node *tp;
    p = n;
    
    // effectue la rotation LR
    tp = p->right->left;
    p->right->left = tp->right;
    tp->right = p->right;
    p->right = tp->left;
    tp->left = p;
    
    return tp; 
}

node* insert(node *root, uint64_t data){
    // insère la valeur data au bon endroit dans l'arbre enraciné en root ...

    // INSERT DATA IN TREE
    if (data > root->data){
        // Data must be inserted on the right
        if (hasRight(root)) {
            root->right = insert(root->right, data);
        }
        else {
            root->right = createNode(data);
        }
    } else {
        // Data must be inserted on the left
        if (hasLeft(root)) {
            root->left = insert(root->left, data);
        }
        else {
            root->left = createNode(data);
        }
    }

    // REBALANCE TREE IF NECESSARY
    if (bf(root) > 1) { // Need rebalance to the right
        if (bf(root->left) > 0){
            root = llrotation(root);
        } else {
            root = rlrotation(root);
        }
        root->right->height = calcHeight(root->right);
        root->left->height = calcHeight(root->left);
        root->height = calcHeight(root);
    } else if (bf(root) < -1){ // Need rebalance to the left
        if (bf(root->right) < 0) {
            root = rrrotation(root);
        } else {
            root = lrrotation(root);
        }
        root->right->height = calcHeight(root->right);
        root->left->height = calcHeight(root->left);
        root->height = calcHeight(root);
    } else { // Is balanced
        root->height = calcHeight(root);
    }

    return root;
}

node* getParentOfMax(node *root){
    if (!hasRight(root)){
        return nullptr;
    }
    while (hasRight(root->right)){
        root = root->right;
    }
    return root;
}

node* deleteNode(node *root, uint64_t data){
    // cherche la valeur data dans l'arbre enraciné en root puis la supprime ...    

    if (data == root->data){
        node* oldRoot = root;

        // Delete root and return child
        if (isLeaf(root)) {
            root = nullptr;
            return nullptr;
        } else if (hasLeft(root)){
            if (!hasRight(root)){
                root = root->left;
            } else {
                // Has Left and Right
                node* m = getParentOfMax(root->left);
                if (m != nullptr) {
                    if (hasLeft(m->right)) {
                        node* tmp = m->right->left;
                        m->right->right = root->right;
                        m->right->left = root->left;
                        root = m->right;
                        m->right = tmp;
                    } else {
                        m->right->right = root->right;
                        m->right->left = root->left;
                        root = m->right;
                        m->right = nullptr;
                    }
                } else {
                    root->left->right = root->right;
                    root = root->left;
                }
            }
        } else {
            root = root->right;
        }
        calcHeightRecursive(root);
        //cout << "Deleted node " << oldRoot->data << " for new root : " << root->data << endl;
        delete(oldRoot);
    } else {
        if (data > root->data && hasRight(root)){
            root->right = deleteNode(root->right, data);
        } else if (data <= root->data && hasLeft(root)){
            root->left = deleteNode(root->left, data);
        } else {
            //cout << "Value not found" << endl;
        }
        root->height = calcHeight(root);
    }


    // REBALANCE TREE IF NECESSARY
    if (bf(root) > 1) { // Need rebalance to the right
        if (bf(root->left) > 0){
            root = llrotation(root);
        } else {
            root = rlrotation(root);
        }
        root->right->height = calcHeight(root->right);
        root->left->height = calcHeight(root->left);
        root->height = calcHeight(root);
    } else if (bf(root) < -1){ // Need rebalance to the left
        if (bf(root->right) < 0) {
            root = rrrotation(root);
        } else {
            root = lrrotation(root);
        }
        root->right->height = calcHeight(root->right);
        root->left->height = calcHeight(root->left);
        root->height = calcHeight(root);
    } else { // Is balanced
        root->height = calcHeight(root);
    }
    
    return root;
}

bool search(node* root, uint64_t data) {
	if (root == nullptr) {
		return false;
	}

	if (root->data == data) {
        return true;
	}
    
	if (data > root->data) {
        return search(root->right, data);
	}
	else {
        return search(root->left, data);
	}
}

node* searchAndInsert(node *root, uint64_t data){
    if (!search(root, data)){
        cout << "Data does not exist. Inserting a new node with data." << endl;
        return insert(root, data);
    }
    cout << "Data already exists." << endl;
    return root;
}

void displayTree(node* tree, int depth = 0, bool isRight = true) {
    if (hasRight(tree)) {
        displayTree(tree->right, depth+1, true);
    }
    string n = "";
    for (int i = 0; i < depth; i++) {
        n += "\t";
    }
    n += (depth!=0 ? (isRight ? "/ " : "\\ ") : "- ") + to_string(tree->height - 1) + "-" + to_string(tree->data);
    cout << n << endl;

    if (hasLeft(tree)) {
        displayTree(tree->left, depth+1, false);
    }
}

vector<uint64_t> loadValues(string filename) {
	vector<uint64_t> values;
    ifstream f1(filename);
    string line;
    getline(f1, line);
    uint64_t value;
    istringstream iss(line);
    iss >> value;
	values.push_back(value);

    while (getline(f1, line))
    {
        istringstream iss(line);
        iss >> value;
		values.push_back(value);
    }
    values.shrink_to_fit();
    return values;
}

node* genTreeFromValuesFile(vector<uint64_t> &values) {
    int n = values.size();

    cout << "Begin insert bench for " << n << " values\n";

    chrono::high_resolution_clock::time_point timeStart = chrono::high_resolution_clock::now();

    node* root = createNode(values.at(0));

    for (int i = 1; i < n; i++)
    {
        root = insert(root, values.at(i));
    }

    chrono::high_resolution_clock::time_point timeEnd = chrono::high_resolution_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(timeEnd - timeStart);

    cout << "End of insert bench\n\tTotal time : " << duration.count() << " microseconds\n\n";

    return root;
}

void searchValuesFromSearchFile(node* root, vector<uint64_t> &values) {
    int n = values.size();
    cout << "Begin search bench for " << n << " values\n";
    chrono::high_resolution_clock::time_point timeStart = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < n; i++)
    {
        search(root, values.at(i));
    }

	chrono::high_resolution_clock::time_point timeEnd = chrono::high_resolution_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(timeEnd - timeStart);

    cout << "End of search bench\n\tTotal time : " << duration.count() << " microseconds\n\n";
}

node* deleteValueFromDeleteFile(node* root, vector<uint64_t> &values) {
    int n = values.size();
    cout << "Begin delete bench for " << n << " values\n";
    chrono::high_resolution_clock::time_point timeStart = chrono::high_resolution_clock::now();


    for (int i = 0; i < n; i++)
    {
        root = deleteNode(root, values.at(i));
    }

    chrono::high_resolution_clock::time_point timeEnd = chrono::high_resolution_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(timeEnd - timeStart);

    cout << "End of delete bench\n\tTotal time : " << duration.count() << " microseconds\n\n";

    return root;
}

void TestValues(string number) {
    chrono::high_resolution_clock::time_point timeStart = chrono::high_resolution_clock::now();
    cout << "Loading values from files..." << endl;
    vector<uint64_t> values = loadValues("Values_" + number + ".txt");
    vector<uint64_t> searchValues = loadValues("Search_" + number + ".txt");
    vector<uint64_t> deleteValues = loadValues("Delete_" + number + ".txt");
    chrono::high_resolution_clock::time_point timeEnd = chrono::high_resolution_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(timeEnd - timeStart);
    cout << "Values loaded in " << duration.count() << " microseconds." << endl << endl;

    //Insert values
    node* tree = genTreeFromValuesFile(values);

    //Search values
    searchValuesFromSearchFile(tree, searchValues);

    //Delete values
    tree = deleteValueFromDeleteFile(tree, deleteValues);

    //Search values after delete
    searchValuesFromSearchFile(tree, searchValues);

    timeEnd = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(timeEnd - timeStart);
    cout << "End benchmark.\n\tTotal time : " << duration.count() << " microseconds\n\n";
}


int main(){
    
    /*
    //Part 2 - Tests
    // Insert values
    node * root = createNode(5); // Create the AVL tree.
    root = insert(root, 4);
    root = insert(root, 3);
    root = insert(root, 2);
    root = insert(root, 1);

    // Delete values
    root = deleteNode(root, 12);
    root = deleteNode(root, 20);

	// Search values
    root = searchAndInsert(root, 23);
    root = searchAndInsert(root, 23);
    */

    //Part 3/4 + 4/4 - Benchmark
    // Load values from files
    TestValues("5");
    TestValues("10");
    TestValues("15");
    TestValues("20");
    TestValues("25");

    return 0;
}