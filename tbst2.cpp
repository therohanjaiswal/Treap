/*Program to implement Threaded Binary Search Tree*/
/*Submitted by Rohan Jaiswal*/

#include <iostream>
#include <limits.h>
#include <queue>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <fstream>
#include <cmath>

using namespace std;

// Node class represents threaded binary search tree node structure
class Node
{
private:
    int data;         // store element
    Node *leftChild;  // points to left child node if exists, else inorder predecessor
    Node *rightChild; // points to right child node if exists, else inorder successor
    bool leftThread;  // true if left child node exists else false
    bool rightThread; // true if right child node exists else false

public:
    // constructor
    Node(int aData)
    {
        data = aData;
    }
    friend class TBST;
};

int result = 0;
/* Threaded Binary Search Tree class.
This tree consist of a dummy node and the actual 
TBST exists as the left child of the dummy node*/
class TBST
{
private:
    Node *root; // root pointer pointing to dummy node
    int totalKeyComparisons;
    Node *deleteTBSTLeafNode(Node *, Node *);
    Node *deleteTBSTSingleChildNode(Node *, Node *);
    void deleteTBSTDoubleChildNode(Node *, Node *);
    Node *inorderSuccessor(Node *);
    Node *leftMostNode(Node *);
    Node *rightMostNode(Node *);
    int getHeightUtils(Node *);
    void getAvgTreeHeightUtils(Node *, float &, int &);

public:
    /* constructor initializes the dummy node with INT_MAX
        and right child and left child pointing to itself
        and leftThread and rightThread to false as no child 
        of dummy node existing as of now*/
    TBST()
    {
        root = new Node(INT_MAX);
        root->leftChild = root;
        root->leftThread = false;
        root->rightChild = root;
        root->rightThread = false;
        totalKeyComparisons = 0;
    }

    void insertTBSTNode(int);
    void deleteTBSTNode(int);
    void printTree();
    Node *search(int);
    int getTreeHeight();
    float getAvgTreeHeight();
    int getTotalKeyComparisons();
    int getTotalRotations();

    ~TBST()
    {
        delete root;
    }
};

// This function performs insertion in the Threaded Binary Search Tree
void TBST::insertTBSTNode(int data)
{

    Node *node = new Node(data); // creating new Threaded Binary Search Tree node

    /* for first insertion in the tree i.e., when the actual tree is empty
            and consist of only dummy node */
    if (root->leftChild == root && root->rightChild == root)
    {
        node->leftChild = root->leftChild;
        node->leftThread = root->leftThread;
        node->rightChild = root;
        node->rightThread = false;

        // performing first insertion as the left child of dummy node
        root->leftChild = node;
        root->leftThread = true;
        return;
    }

    // when the tree is not empty i.e., it has some element other than dummy node
    Node *curr = root->leftChild; // curr points to left child of dummy node i.e., root of actual TBST
    while (true)
    {
        try
        {
            ++totalKeyComparisons;
            if (node->data < curr->data) // if current node data is greater than data to be inserted
            {
                if (curr->leftThread) // if left Child exists of current node
                {
                    curr = curr->leftChild;
                }
                else // if left child doesn't exist, inserting new node to the left of curr node
                {
                    node->leftChild = curr->leftChild;
                    node->leftThread = curr->leftThread;
                    node->rightChild = curr;
                    node->rightThread = false;

                    curr->leftChild = node;
                    curr->leftThread = true;
                    return;
                }
            }
            else if (node->data > curr->data) // if current node data is smaller than data to be inserted
            {
                if (curr->rightThread) // if right Child exists of current node
                {
                    curr = curr->rightChild;
                }
                else // if right child doesn't exist, inserting new node to the right of curr node
                {
                    node->rightChild = curr->rightChild;
                    node->rightThread = curr->rightThread;
                    node->leftChild = curr;
                    node->leftThread = false;

                    curr->rightChild = node;
                    curr->rightThread = true;
                    return;
                }
            }
            else // if data to be inserted already present in the tree
            {
                throw node->data;
            }
        }
        catch (int x)
        {
            cout << "\nElement " << x << " is already present in the tree.";
            return;
        }
    }
}

/*Function deletes the node containing key from the tree*/
void TBST::deleteTBSTNode(int key)
{
    Node *curr = root->leftChild; // // curr points to left child of dummy node i.e., root of actual TBST
    if (curr == root)
    {
        cout << "\nTree is empty\n";
        return;
    }

    bool isFound = false;
    isFound = search(key);
    // if key is not present in the tree
    if (!isFound)
    {
        cout << "\nElement " << key << " is not present in the tree.";
        return;
    }

    // if root is deleted
    if (curr->data == key)
        ++totalKeyComparisons;

    Node *par = root;         // points to the parent of curr node
    while (curr->data != key) // until the key is found in the tree
    {
        par = curr;
        ++totalKeyComparisons;
        if (key < curr->data)
            curr = curr->leftChild;
        else
            curr = curr->rightChild;
    }

    // if the node to be deleted is a leaf node
    if (!curr->leftThread && !curr->rightThread)
        deleteTBSTLeafNode(par, curr);
    // if the node to be deleted is a node with single child
    else if ((!curr->leftThread && curr->rightThread) || (curr->leftThread && !curr->rightThread))
        deleteTBSTSingleChildNode(par, curr);
    else // if the node to be deleted has two children
        deleteTBSTDoubleChildNode(par, curr);
}

/*Utility Function to delete the leaf node. It takes curr pointer pointing to the 
    node to be deleted and par pointer pointing to the parent of curr node. It 
    returns the address of deleted node*/
Node *TBST::deleteTBSTLeafNode(Node *par, Node *curr)
{
    if (curr == par->leftChild) // when the curr node is left child of par node
    {
        par->leftChild = curr->leftChild;
        par->leftThread = curr->leftThread;
    }
    else // when the curr node is right child of par node
    {
        par->rightChild = curr->rightChild;
        par->rightThread = curr->rightThread;
    }

    curr->leftChild = nullptr;
    curr->rightChild = nullptr;
    curr->leftThread = false;
    curr->rightThread = false;
    return curr; // returning the address of deleted node
}

/*Utility Function to delete the node with single child. It takes curr pointer pointing to the 
    node to be deleted and par pointer pointing to the parent of curr node. It 
    returns the address of deleted node*/
Node *TBST::deleteTBSTSingleChildNode(Node *par, Node *curr)
{
    Node *leftMost, *rightMost;
    // curr->data < par->data
    if (curr == par->leftChild) // if the curr node is left child of parent node
    {
        if (curr->rightThread) // if the current node has right child
        {
            par->leftChild = curr->rightChild;
            leftMost = leftMostNode(curr->rightChild);
            leftMost->leftChild = curr->leftChild;
            leftMost->leftThread = curr->leftThread;
        }
        else if (curr->leftThread) // if the current node has left child
        {
            par->leftChild = curr->leftChild;
            rightMost = rightMostNode(curr->leftChild);
            rightMost->rightChild = curr->rightChild;
            rightMost->rightThread = curr->rightThread;
        }
    }
    else // if the curr nodes is right child of parent node
    {
        if (curr->rightThread) // if the current node has right child
        {
            par->rightChild = curr->rightChild;
            leftMost = leftMostNode(curr->rightChild);
            leftMost->leftChild = curr->leftChild;
            leftMost->leftThread = curr->leftThread;
        }
        else if (curr->leftThread) // if the current node has left child
        {
            par->rightChild = curr->leftChild;
            rightMost = rightMostNode(curr->leftChild);
            rightMost->rightChild = curr->rightChild;
            rightMost->rightThread = curr->rightThread;
        }
    }

    curr->leftChild = nullptr;
    curr->rightChild = nullptr;
    curr->leftThread = false;
    curr->rightThread = false;
    return curr; // returning the deleted node
}

/*Utility Function to delete the node with two child nodes. It takes curr pointer pointing to the 
    node to be deleted and par pointer pointing to the parent of curr node. */
void TBST::deleteTBSTDoubleChildNode(Node *par, Node *curr)
{
    // finds the leftmost of right sub tree of current node (inorder successor of curr node)
    Node *leftMost = leftMostNode(curr->rightChild);
    // prev points to parent of temp node, temp points to right child of curr node
    Node *prev = curr, *temp = curr->rightChild;

    // finding the parent of leftmost node and storing its address in prev
    // temp->data != leftMost->data
    while (temp != leftMost)
    {
        prev = temp;
        ++totalKeyComparisons;
        if (leftMost->data < temp->data)
            temp = temp->leftChild;
        else
            temp = temp->rightChild;
    }

    // deleting the leftmost node when it is a leaf node
    if (!leftMost->leftThread && !leftMost->rightThread)
        temp = deleteTBSTLeafNode(prev, leftMost);
    else // deleting the left most node when it is a node with single child
        temp = deleteTBSTSingleChildNode(prev, leftMost);

    //changing the data value of curr node to inorder successor data value
    curr->data = temp->data;
    free(temp);
}

/* Fucntion searches for an element in the TBST*/
Node *TBST::search(int key)
{
    Node *curr = root->leftChild; // curr points to left child of dummy node i.e., root of actual TBST
    bool elementFound = false;

    // to reach to the left most leaf node {it contains the smallest element of the tree}
    curr = leftMostNode(curr);

    // to traverse th tree from smallest element
    while (curr != root)
    {
        if (curr->data == key)
        {
            elementFound = true;
            break;
        }
        curr = inorderSuccessor(curr); // find inorder successor of the curr node
    }

    if (elementFound)
        return curr;
    return nullptr;
}

/*Utility function to find the inorder successor of curr node*/
Node *TBST::inorderSuccessor(Node *curr)
{
    if (!curr->rightThread)      // if the curr node has no right child
        return curr->rightChild; // return the rightchild
    else                         // if it has right child
        curr = curr->rightChild; // point to right child

    return leftMostNode(curr); // return the left most node of curr right child
}

/*Utility function to find the leftmost child node of curr node*/
Node *TBST::leftMostNode(Node *curr)
{
    while (curr->leftThread)
        curr = curr->leftChild;
    return curr;
}

/*Utility function to find the rightmost child node of curr node*/
Node *TBST::rightMostNode(Node *curr)
{
    while (curr->rightThread)
        curr = curr->rightChild;
    return curr;
}

/*Function to form the "graph.gv" file which helps to generate the
    picture of the tree*/
void TBST::printTree()
{
    // forming initial part of "graph.gv file"
    ofstream stream;
    //string str = "grapgh.gv";
    string str = "graph";
    str.append(to_string(++result));
    str.append(".gv");
    stream.open(str);
    stream << "digraph g { \n\tnode[shape = record, height = .1];\n";
    stream << "\tnode" << INT_MAX << "[label = \"<l> | <d> Dummy Node | <r> \"];\n";

    Node *curr = root->leftChild; // curr points to left child of dummy node i.e., root of actual TBST
    // when tree is empty
    if (curr == root)
    {
        cout << "Tree is empty" << endl;
        return;
    }

    string nodeString = "";    // to store the details of every node
    string pointerString = ""; // to store the pointers details
    ostringstream oss;
    queue<Node *> q; // queue to perform level order traversal
    q.push(curr);

    while (!q.empty())
    {
        Node *node = q.front();
        // storing node details
        oss << "\tnode" << node->data << "[label = \"<l> | <d> " << node->data << " | <r>\"];\n";
        nodeString.append(oss.str());
        oss.str("");
        oss.clear();

        // Enqueue left child
        if (node->leftThread)
        {
            q.push(node->leftChild);
            // storing node details
            oss << "\t\"node" << node->data << "\":l -> \"node" << node->leftChild->data << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }

        // if left chid doesn't exists
        if (!node->leftThread)
        {
            // storing pointer details
            oss << "\t\"node" << node->data << "\":l -> \"node" << node->leftChild->data << "\":d [color=\"red\"];\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }

        // Enqueue right child
        if (node->rightThread)
        {
            q.push(node->rightChild);
            // storing node details
            oss << "\t\"node" << node->data << "\":r -> \"node" << node->rightChild->data << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }

        // if right child doesn't exist
        if (!node->rightThread)
        {
            // storing pointer details
            oss << "\t\"node" << node->data << "\":r -> \"node" << node->rightChild->data << "\":d [color=\"red\"];\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }
        q.pop();
    }

    // writing nodes details and pointer details on graph.gv
    stream << nodeString;
    stream << "\t\"node" << root->data << "\":l -> \"node" << root->leftChild->data << "\":d;\n";
    stream << "\t\"node" << root->data << "\":r -> \"node" << root->rightChild->data << "\":d [color=\"red\"];\n";
    stream << pointerString;
    stream << "}";
    stream.close();
}

int TBST::getHeightUtils(Node *curr)
{
    if (curr == root || (!curr->leftThread && !curr->rightThread))
        return 0;

    int leftHeight = 0, rightHeight = 0;
    if (curr->leftThread)
        leftHeight = getHeightUtils(curr->leftChild);

    if (curr->rightThread)
        rightHeight = getHeightUtils(curr->rightChild);

    return 1 + max(leftHeight, rightHeight);
}

int TBST::getTreeHeight()
{
    return getHeightUtils(root->leftChild);
}

/*Inorder traversal of the threaded binary search tree*/
void TBST::getAvgTreeHeightUtils(Node *curr, float &sum, int &totalNodes)
{
    while (curr->leftThread)
        curr = curr->leftChild;
    // Loop to traverse the tree
    while (curr != root)
    {
        sum += getHeightUtils(curr);
        ++totalNodes;
        curr = inorderSuccessor(curr);
    }
}

float TBST::getAvgTreeHeight()
{
    float sum = 0;
    int totalNodes = 0;
    getAvgTreeHeightUtils(root->leftChild, sum, totalNodes);
    cout << sum << "- sum\n";
    cout << totalNodes << "- nodes\n";
    return sum / totalNodes;
}

int TBST::getTotalKeyComparisons()
{
    return totalKeyComparisons;
}

void generateInput()
{
    int low, high, noOfInput;
    int num, ratio, op;
    cout << "\nEnter total number of keys: ";
    cin >> noOfInput;
    cout << "\nEnter lower limit: ";
    cin >> low;
    cout << "\nEnter upper limit: ";
    cin >> high;

    ofstream file;
    file.open("inputFile1.txt");
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

TBST *readInputFile(TBST *tbst)
{
    //TBST tbst;
    ifstream file;
    file.open("inputFile1.txt");
    string operation;
    int value;
    while (file >> operation)
    {
        file >> value;
        if (operation.compare("Insert") == 0)
            tbst->insertTBSTNode(value);
        else
            tbst->deleteTBSTNode(value);
    }
    file.close();
    return tbst;
}

int main()
{
    TBST *tbst = new TBST();
    // pass by reference
    generateInput();
    tbst = readInputFile(tbst);
    tbst->printTree();
    printf("\n");
    cout << tbst->getTreeHeight() << "- Tree Height\n";
    cout << tbst->getAvgTreeHeight() << "- Avg Height\n";
    cout << tbst->getTotalKeyComparisons() << "- key comp\n";
    return 0;
}
