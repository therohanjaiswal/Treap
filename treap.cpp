#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <fstream>
#include <cmath>
#include <time.h>

using namespace std;

class TreapNode
{
private:
    int key;
    int priority;
    TreapNode *leftChild, *rightChild;

public:
    TreapNode(int value, int pr)
    {
        key = value;
        priority = pr;
        leftChild = nullptr;
        rightChild = nullptr;
    }
    friend class Treap;
};

int result = 0;
class Treap
{
private:
    TreapNode *head;
    int totalKeyComparisons;
    int totalRotations;
    void deleteTreapLeafNode(TreapNode *, TreapNode *);
    void rotateLeft(TreapNode *, TreapNode *);
    void rotateRight(TreapNode *, TreapNode *);
    int getHeightUtils(TreapNode *);
    void getAvgTreeHeightUtils(TreapNode *, float &, int &);

public:
    Treap()
    {
        head = new TreapNode(INT_MAX, INT_MAX);
        totalKeyComparisons = 0;
        totalRotations = 0;
    }

    void insertTreapNode(int, int);
    void deleteTreapNode(int);
    bool treapSearch(int);
    void printTree();
    int getTreeHeight();
    float getAvgTreeHeight();
    int getTotalKeyComparisons();
    int getTotalRotations();

    ~Treap()
    {
        delete head;
    }
};

void Treap::insertTreapNode(int key, int priority)
{

    TreapNode *newNode = new TreapNode(key, priority);
    TreapNode *root = head->rightChild;

    if (root == nullptr)
    {
        head->rightChild = newNode;
        return;
    }

    stack<TreapNode *> path;
    path.push(head);
    TreapNode *prev = nullptr;
    TreapNode *curr = root;
    while (curr != nullptr)
    {
        prev = curr;
        path.push(prev);
        ++totalKeyComparisons;
        try
        {
            if (key > curr->key)
                curr = curr->rightChild;
            else if (key < curr->key)
                curr = curr->leftChild;
            else
                throw key;
        }
        catch (int x)
        {
            cout << key << " is already present.\n";
            return;
        }
    }

    ++totalKeyComparisons;
    if (key > prev->key)
        prev->rightChild = newNode;
    else
        prev->leftChild = newNode;

    TreapNode *parent = path.top();
    TreapNode *child = newNode;
    TreapNode *gparent = nullptr;
    while (path.top() != head)
    {
        path.pop();
        if (parent->priority > child->priority)
        {
            ++totalRotations;
            if (child == parent->leftChild)
                rotateRight(parent, child);
            else if (child == parent->rightChild)
                rotateLeft(parent, child);

            gparent = path.top();
            if (gparent->leftChild == parent)
            {
                gparent->leftChild = child;
                child = gparent->leftChild;
            }
            else if (gparent->rightChild == parent)
            {
                gparent->rightChild = child;
                child = gparent->rightChild;
            }
            parent = gparent;
        }
    }
}

void Treap::deleteTreapNode(int k)
{

    TreapNode *curr = head->rightChild;
    if (curr == nullptr)
    {
        cout << "\nTree is empty\n";
        return;
    }

    // if root is deleted
    if (curr->key == k)
        ++totalKeyComparisons;

    TreapNode *prev = head;
    while (curr != nullptr && curr->key != k)
    {
        prev = curr;
        ++totalKeyComparisons;
        if (curr->key < k)
            curr = curr->rightChild;
        else
            curr = curr->leftChild;
    }

    // if key is absent
    try
    {
        if (curr == nullptr)
            throw k;
    }
    catch (int x)
    {
        cout << x << " is absent in the tree.\n";
        return;
    }

    // if curr already leaf
    if (curr->leftChild == nullptr && curr->rightChild == nullptr)
    {
        deleteTreapLeafNode(prev, curr);
        return;
    }

    // if not leaf
    while (curr->leftChild != nullptr || curr->rightChild != nullptr)
    {
        TreapNode *currChild;
        // if curr has single child
        if (curr->leftChild == nullptr || curr->rightChild == nullptr)
            currChild = curr->leftChild != nullptr ? curr->leftChild : curr->rightChild;
        else // curr has double child
            currChild = curr->leftChild->priority < curr->rightChild->priority ? curr->leftChild : curr->rightChild;

        // ++totalRotations;
        if (currChild == curr->leftChild)
            rotateRight(curr, currChild);
        else if (currChild == curr->rightChild)
            rotateLeft(curr, currChild);

        if (prev->leftChild == curr)
            prev->leftChild = currChild;
        else if (prev->rightChild == curr)
            prev->rightChild = currChild;
        prev = currChild;
    }

    // curr is leaf now
    deleteTreapLeafNode(prev, curr);
}

void Treap::deleteTreapLeafNode(TreapNode *prev, TreapNode *curr)
{
    if (prev->leftChild == curr)
        prev->leftChild = nullptr;
    else
        prev->rightChild = nullptr;
}

void Treap::rotateLeft(TreapNode *parent, TreapNode *child)
{
    TreapNode *temp = child->leftChild;
    child->leftChild = parent;
    parent->rightChild = temp;
    ++totalRotations;
}

void Treap::rotateRight(TreapNode *parent, TreapNode *child)
{
    TreapNode *temp = child->rightChild;
    child->rightChild = parent;
    parent->leftChild = temp;
    ++totalRotations;
}

bool Treap::treapSearch(int k)
{
    TreapNode *temp = head->rightChild; // temp points to root of the tree which is the right child of head node
    if (temp == nullptr)                // if tree is empty
        return false;

    // iterate until temp becomes null i.e., temp reaches leaf node
    while (temp != nullptr)
    {
        if (temp->key == k) // if k is found
            return true;
        else if (temp->key < k) // if k is greater than key of root
            temp = temp->rightChild;
        else // if k is smaller than key of root
            temp = temp->leftChild;
    }
    return false; // if k is not present in the tree
}

void Treap::printTree()
{
    // forming initial part of "graph.gv file"
    ofstream stream;
    string str = "graph";
    str.append(to_string(++result));
    str.append(".gv");
    stream.open(str);
    stream << "digraph g { \n\tnode[shape = record, height = .1];\n";
    stream << "\tnode" << INT_MAX << "[label = \"<l> | <d> Head Node | <r> \"];\n";

    TreapNode *root = head->rightChild; // curr points to left child of dummy node i.e., root of actual TBST
    if (root == nullptr)
    { // when tree is empty
        cout << "Tree is empty" << endl;
        return;
    }

    string nodeString = "";    // to store the details of every node
    string pointerString = ""; // to store the pointers details
    ostringstream oss;
    queue<TreapNode *> q; // queue to perform level order traversal
    q.push(root);

    while (!q.empty())
    {
        TreapNode *node = q.front();
        // storing node details
        oss << "\tnode" << node->key << "[label = \"<l> | <d> " << node->key << "," << node->priority << " | <r>\"];\n";
        nodeString.append(oss.str());
        oss.str("");
        oss.clear();

        // Enqueue left child
        if (node->leftChild != nullptr)
        {
            q.push(node->leftChild);
            // storing node details
            oss << "\t\"node" << node->key << "\":l -> \"node" << node->leftChild->key << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }

        // Enqueue right child
        if (node->rightChild != nullptr)
        {
            q.push(node->rightChild);
            // storing node details
            oss << "\t\"node" << node->key << "\":r -> \"node" << node->rightChild->key << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }
        q.pop();
    }
    stream << nodeString;
    stream << "\t\"node" << head->key << "\":r -> \"node" << head->rightChild->key << "\":d;\n";
    stream << pointerString;
    stream << "}";
    stream.close();

    // generate tree image
    // string cmd = "dot -Tpng graph.gv -o " + string(filename) + ".png";
    // system((const char *)cmd.c_str());
    // dot -Tpng graph.gv -o graph.png
}

int Treap::getHeightUtils(TreapNode *root)
{
    if (root == nullptr || (root->leftChild == nullptr && root->rightChild == nullptr))
        return 0;
    return 1 + max(getHeightUtils(root->leftChild), getHeightUtils(root->rightChild));
}

int Treap::getTreeHeight()
{
    return getHeightUtils(head->rightChild);
}

void Treap::getAvgTreeHeightUtils(TreapNode *root, float &sum, int &totalNodes)
{
    if (root == nullptr)
        return;
    getAvgTreeHeightUtils(root->leftChild, sum, totalNodes);
    sum += getHeightUtils(root);
    ++totalNodes;
    getAvgTreeHeightUtils(root->rightChild, sum, totalNodes);
}

float Treap::getAvgTreeHeight()
{
    float sum = 0;
    int totalNodes = 0;
    getAvgTreeHeightUtils(head->rightChild, sum, totalNodes);
    cout << sum << "- sum\n";
    cout << totalNodes << "- nodes\n";
    return sum / totalNodes;
}

int Treap::getTotalKeyComparisons()
{
    return totalKeyComparisons;
}

int Treap::getTotalRotations()
{
    return totalRotations;
}

void generateInput()
{
    int low, high, noOfInput;
    int num, ratio, op;
    cout << "\nEnter total number of keys: ";
    cin >> noOfInput;
    cout << "\nEnter lower limit" << endl;
    cin >> low;
    cout << "\nEnter upper limit" << endl;
    cin >> high;

    ofstream file;
    file.open("inputFile.txt");
    srand(time(0));
    while (noOfInput--)
    {
        num = low + rand() % (high - low + 1);
        ratio = 1 + rand() % 100;
        op = ratio > 70 ? 1 : 0; // 0 - insert, 1 - delete
        if (op == 0)
            file << "Insert " << num << endl;
        else
            file << "Delete " << num << endl;
    }
    file.close();
}

void readInputFile(Treap *treap)
{
    ifstream file;
    file.open("inputFile.txt");
    string operation;
    int priority, value;
    srand(time(0));
    while (file >> operation)
    {
        file >> value;
        if (operation.compare("Insert") == 0)
        {
            priority = rand();
            treap->insertTreapNode(value, priority);
        }
        else
            treap->deleteTreapNode(value);
    }
    file.close();
}

int main()
{
    Treap *treap = new Treap();
    // treap.insertTreapNode(100, 100);
    // treap.insertTreapNode(30, 70);
    // treap.insertTreapNode(70, 80);
    // treap.insertTreapNode(20, 90);
    // treap.insertTreapNode(20, 100);
    // treap.printTree();
    // treap.deleteTreapNode(200);
    // treap.deleteTreapNode(30);
    generateInput();
    readInputFile(treap);
    treap->printTree();
    cout << treap->getTreeHeight() << "- Tree Height\n";
    cout << treap->getAvgTreeHeight() << "- Avg Height\n";
    cout << treap->getTotalKeyComparisons() << "- key comp\n";
    cout << treap->getTotalRotations() << "- rotations\n";
    return 0;
}