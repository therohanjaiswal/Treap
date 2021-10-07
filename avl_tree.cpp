#include <iostream>
#include <limits.h>
#include <queue>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <unordered_map>
#include <fstream>
#include <cmath>

using namespace std;

class AVL_Node
{
private:
    int key;
    int bf; // height of left subtree - height of right subtree
    AVL_Node *LChild, *RChild;

public:
    AVL_Node(int data)
    {
        key = data;
        LChild = nullptr;
        RChild = nullptr;
        bf = 0;
    }
    friend class AVL_Tree;
};

int result = 0;

class AVL_Tree
{
private:
    AVL_Node *head; // head points to dummy node
    int totalKeyComparisons;
    int totalRotations;
    void AVL_RR_Rotation(AVL_Node *, AVL_Node *, int);
    void AVL_LL_Rotation(AVL_Node *, AVL_Node *, int);
    void AVL_LR_Rotation(AVL_Node *, AVL_Node *, int, string);
    void AVL_RL_Rotation(AVL_Node *, AVL_Node *, int, string);
    int getHeightUtils(AVL_Node *);
    void getAvgTreeHeightUtils(AVL_Node *, float &, int &);

public:
    AVL_Tree()
    {
        head = new AVL_Node(INT_MAX);
        totalKeyComparisons = 0;
        totalRotations = 0;
    }

    void insertAVLNode(int);
    void deleteAVLNode(int);
    void printTree();
    int getTreeHeight();
    float getAvgTreeHeight();
    int getTotalKeyComparisons();
    int getTotalRotations();

    ~AVL_Tree()
    {
        delete head;
    }
};

void AVL_Tree::insertAVLNode(int k)
{
    AVL_Node *newNode = new AVL_Node(k);
    AVL_Node *root = head->RChild;

    // first insertion
    if (root == nullptr)
    {
        head->RChild = newNode;
        return;
    }

    // rebalancingPoint points to node where rebalancing may be necessary
    // parent points to parent of rebalancingPoint
    // cursor is used for traversal
    // cursorNext point to the child of cursor
    AVL_Node *parent = head, *rebalancingPoint = root;
    AVL_Node *cursor = root, *cursorNext;

    while (true)
    {
        try
        {
            ++totalKeyComparisons;
            if (k < cursor->key) // if k is smaller than cursor->key
            {
                cursorNext = cursor->LChild;
                if (cursorNext == nullptr) // if cursor reached the leaf node
                {
                    cursorNext = newNode;
                    cursor->LChild = cursorNext; // make newNode as left child of cursor
                    break;
                }
                else if (cursorNext->bf != 0) // if balance factor of cursorNext is non-zero
                {
                    parent = cursor;
                    rebalancingPoint = cursorNext;
                }
                cursor = cursorNext;
            }
            else if (k > cursor->key) // if k is greater than cursor->key
            {
                cursorNext = cursor->RChild;
                if (cursorNext == nullptr) // if cursor reached the leaf node
                {
                    cursorNext = newNode;
                    cursor->RChild = cursorNext; // make newNode as right child of cursor
                    break;
                }
                else if (cursorNext->bf != 0) // if balance factor of cursorNext is non-zero
                {
                    parent = cursor;
                    rebalancingPoint = cursorNext;
                }
                cursor = cursorNext;
            }
            else                    // if k already exists in the tree
                throw newNode->key; // throw exception
        }
        catch (int x)
        {
            cout << "\nElement " << x << " is already present in the tree.";
            return;
        }
    }

    // a denotes in which side of rebalancing point the insertion took place
    ++totalKeyComparisons;
    int a = k < rebalancingPoint->key ? 1 : -1;
    cursor = (a == 1) ? rebalancingPoint->LChild : rebalancingPoint->RChild;
    /* temp points to the insertion side child of rebalancing point
        i.e., if insertion happened on left side then temp point to left child
        else right child */
    AVL_Node *temp = cursor;

    // updating the balance factor of nodes until cursor != cursorNext
    while (cursor != cursorNext)
    {
        ++totalKeyComparisons;
        if (k < cursor->key)
        {
            cursor->bf = 1;
            cursor = cursor->LChild;
        }
        else if (k > cursor->key)
        {
            cursor->bf = -1;
            cursor = cursor->RChild;
        }
    }

    // if balance factor of rebalancing point is 0
    if (rebalancingPoint->bf == 0)
    {
        rebalancingPoint->bf = a; //set the new balance factor in it
        return;
    }
    /* if balance factor of rebalancing point is opposite to the new
     balance factor i.e., tree is more balanced now */
    else if (rebalancingPoint->bf == -1 * a)
    {
        rebalancingPoint->bf = 0;
        return;
    }
    else // rebalancingPoint->bf == a
    {
        /* insertion is done on the same side of temp as that of rebalancing point
           so, single rotation is required */
        if (temp->bf == a)
        {
            cursor = temp;
            /* insertion is done in the left side of rebalancing point
               and left side of temp */
            if (a == 1)
                AVL_LL_Rotation(rebalancingPoint, temp, a);
            /* insertion is done in the right side of rebalancing point
               and right side of temp */
            else if (a == -1)
                AVL_RR_Rotation(rebalancingPoint, temp, a);
        }
        /* insertion is done on the opposite side of temp as that of rebalancing point
           double rotation  */
        else if (temp->bf == -1 * a)
        {
            /* insertion is done in the left side of rebalancing point
               and right side of temp */
            if (a == 1)
            {
                cursor = temp->RChild;
                AVL_LR_Rotation(rebalancingPoint, temp, a, "insertion");
            }
            /* insertion is done in the right side of rebalancing point
               nd left side of temp */
            else if (a == -1)
            {
                cursor = temp->LChild;
                AVL_RL_Rotation(rebalancingPoint, temp, a, "insertion");
            }
        }
    }

    if (rebalancingPoint == parent->RChild)
        parent->RChild = cursor;
    else
        parent->LChild = cursor;
}

void AVL_Tree::AVL_RL_Rotation(AVL_Node *rebalancingPoint, AVL_Node *temp, int a, string operation)
{
    // first rotaion
    AVL_Node *cursor = temp->LChild;
    temp->LChild = cursor->RChild;
    cursor->RChild = temp;
    ++totalRotations;

    // second rotation
    rebalancingPoint->RChild = cursor->LChild;
    cursor->LChild = rebalancingPoint;
    ++totalRotations;

    // update balance factor
    if (operation.compare("insertion") == 0)
    {
        rebalancingPoint->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? 0 : 1);
        temp->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? -1 : 0);
    }
    else if (operation.compare("deletion") == 0)
    {
        rebalancingPoint->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? 0 : 1);
        temp->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? -1 : 0);
    }
    cursor->bf = 0;
}

void AVL_Tree::AVL_LR_Rotation(AVL_Node *rebalancingPoint, AVL_Node *temp, int a, string operation)
{
    // first rotation
    AVL_Node *cursor = temp->RChild;
    temp->RChild = cursor->LChild;
    cursor->LChild = temp;
    ++totalRotations;

    // second rotation
    rebalancingPoint->LChild = cursor->RChild;
    cursor->RChild = rebalancingPoint;
    ++totalRotations;

    // update balance factor
    if (operation.compare("insertion") == 0)
    {
        rebalancingPoint->bf = (cursor->bf == 0) ? 0 : (cursor->bf == 1 ? -1 : 0);
        temp->bf = (cursor->bf == 0) ? 0 : (cursor->bf == 1 ? 0 : 1);
    }
    else if (operation.compare("deletion") == 0)
    {
        rebalancingPoint->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? -1 : 0);
        temp->bf = cursor->bf == 0 ? 0 : (cursor->bf == 1 ? 0 : 1);
    }
    cursor->bf = 0;
}

void AVL_Tree::AVL_RR_Rotation(AVL_Node *rebalancingPoint, AVL_Node *temp, int a)
{
    // rotation
    AVL_Node *cursor = temp;
    rebalancingPoint->RChild = temp->LChild;
    temp->LChild = rebalancingPoint;
    ++totalRotations;

    // update balance factor
    if (temp->bf == 0)
    {
        temp->bf = a;
    }
    else
    {
        rebalancingPoint->bf = 0;
        temp->bf = 0;
    }
}

void AVL_Tree::AVL_LL_Rotation(AVL_Node *rebalancingPoint, AVL_Node *temp, int a)
{
    // rotation
    AVL_Node *cursor = temp;
    rebalancingPoint->LChild = temp->RChild;
    temp->RChild = rebalancingPoint;
    ++totalRotations;

    // update balance factor
    if (temp->bf == 0)
    {
        temp->bf = a;
    }
    else
    {
        rebalancingPoint->bf = 0;
        temp->bf = 0;
    }
}

void AVL_Tree::deleteAVLNode(int k)
{
    AVL_Node *root = head->RChild; // root is the root of the tree which is right child of head node
    if (root == nullptr)
    {
        cout << "\nTree is empty\n";
        return;
    }

    AVL_Node *curr = root;   // curr used for path traversal
    stack<AVL_Node *> stack; // store the nodes which comes in the path upto the deleted node
    stack.push(head);
    AVL_Node *prev = head; //prev points to prev of curr

    // if root is deleted
    if (curr->key == k)
        ++totalKeyComparisons;

    // finding the node with key = k
    while (curr != nullptr && curr->key != k)
    {
        prev = curr;
        ++totalKeyComparisons;   // updating prev
        stack.push(prev);        // pushing curr into stack
        if (curr->key < k)       // if curr->key is smaller than k
            curr = curr->RChild; // update curr to right child of curr
        else if (curr->key > k)  // if curr->key is greater than k
            curr = curr->LChild; // update curr to right child of curr
    }

    // k doesn't exists
    try
    {
        if (curr == nullptr)
            throw k;
    }
    catch (int x)
    {
        cout << "\nElement " << x << " is absent in the tree.\n";
        return;
    }

    // leaf node deletion i.e., k is present in the leaf node
    if (curr->LChild == nullptr && curr->RChild == nullptr)
    {
        if (prev == head) // if deleted node is root node itself with one child
            head->RChild = curr->RChild;
        else if (curr->key < prev->key) // if node to be deleted is left child of prev
            prev->LChild = nullptr;
        else // if node to be deleted is right child of prev
            prev->RChild = nullptr;
    }
    // node with single child deletion i.e., k has one child
    else if (curr->LChild == nullptr || curr->RChild == nullptr)
    {
        AVL_Node *temp;
        // if curr has no left child
        if (curr->LChild == nullptr)
            temp = curr->RChild;
        else // otherwise
            temp = curr->LChild;

        if (prev == head) // if deleted node is root node itself with one child
            head->RChild = temp;
        else if (curr == prev->RChild) // if the node to be deleted is right child of prev
            prev->RChild = temp;
        else // if the node to be deleted is left child of prev
            prev->LChild = temp;
    }
    // node with double child deletion i.e., k has two children
    else
    {
        AVL_Node *temp;                // temp is used for traversal to find out inorder successor of curr
        AVL_Node *parOfTemp = nullptr; // parOfTemp points to parent of temp
        temp = curr->RChild;
        stack.push(curr);

        // finding inorder successor of curr
        while (temp->LChild != nullptr)
        {
            stack.push(temp);
            parOfTemp = temp;
            temp = temp->LChild;
        }

        /* if the right child of curr is itself the
            inorder successor of curr */
        if (parOfTemp == nullptr)
            curr->RChild = temp->RChild;
        else
            parOfTemp->LChild = temp->RChild;
        curr->key = temp->key; // copying the key value
        k = temp->key;
    }

    // rebalancingPoint points to node where rebalancing may be necessary
    // parent points to parent of rebalancingPoint
    // cursor is used for traversal
    // temp point to the child of rebalancingPoint
    AVL_Node *rebalancingPoint, *parent;
    AVL_Node *temp, *cursor;

    while (stack.top() != head)
    {
        rebalancingPoint = stack.top();
        int a = k < rebalancingPoint->key ? 1 : -1; // a denotes in which side of rebalancing point the deletion took place
        ++totalKeyComparisons;
        stack.pop();
        parent = stack.top();

        if (rebalancingPoint->bf == a) //if balance factor of rebalancingPoint is same as new balance factor
        {
            rebalancingPoint->bf = 0;
            continue;
        }
        // if balance factor of rebalancing point is 0
        else if (rebalancingPoint->bf == 0)
        {
            rebalancingPoint->bf = -1 * a;
            return;
        }
        //if (rebalancingPoint->bF == -1*a) i.e., if balance factor of rebalancingPoint is opposite of a
        else
        {
            temp = rebalancingPoint->bf == 1 ? rebalancingPoint->LChild : rebalancingPoint->RChild;
            //single rotation when temp has one child
            if (temp->bf == -1 * a)
            {
                cursor = temp;
                if (a == -1)
                    AVL_LL_Rotation(rebalancingPoint, temp, a);
                else if (a == 1)
                    AVL_RR_Rotation(rebalancingPoint, temp, a);
            }
            //single rotation when temp has two children
            else if (temp->bf == 0)
            {
                cursor = temp;
                if (a == -1)
                    AVL_LL_Rotation(rebalancingPoint, temp, a);
                else if (a == 1)
                    AVL_RR_Rotation(rebalancingPoint, temp, a);
            }
            //double rotation
            else if (temp->bf == a)
            {
                if (a == -1)
                {
                    cursor = temp->RChild;
                    AVL_LR_Rotation(rebalancingPoint, temp, a, "deletion");
                }
                else if (a == 1)
                {
                    cursor = temp->LChild;
                    AVL_RL_Rotation(rebalancingPoint, temp, a, "deletion");
                }
            }

            // No further rotations required if the balance factor has not changed for the subtree
            if (cursor->bf == 1 || cursor->bf == -1)
            {
                if (rebalancingPoint == parent->RChild)
                    parent->RChild = cursor;
                else
                    parent->LChild = cursor;
                return;
            }
        }

        if (rebalancingPoint == parent->RChild)
            parent->RChild = cursor;
        else
            parent->LChild = cursor;
    }
}

void AVL_Tree::printTree()
{
    // forming initial part of "graph.gv file"
    // const char *filename
    ofstream stream;
    //string str = "graph";
    string str = "graph";
    str.append(to_string(++result));
    str.append(".gv");
    stream.open(str);
    stream << "digraph g { \n\tnode[shape = record, height = .1];\n";
    stream << "\tnode" << INT_MAX << "[label = \"<l> | <d> Head Node | <r> \"];\n";

    AVL_Node *root = head->RChild; // curr points to left child of dummy node i.e., root of actual TBST
    if (root == nullptr)
    { // when tree is empty
        cout << "Tree is empty" << endl;
        return;
    }

    string nodeString = "";    // to store the details of every node
    string pointerString = ""; // to store the pointers details
    ostringstream oss;
    queue<AVL_Node *> q; // queue to perform level order traversal
    q.push(root);

    while (!q.empty())
    {
        AVL_Node *node = q.front();
        // storing node details
        oss << "\tnode" << node->key << "[label = \"<l> | <d> " << node->key << "," << node->bf << " | <r>\"];\n";
        nodeString.append(oss.str());
        oss.str("");
        oss.clear();

        // Enqueue left child
        if (node->LChild != nullptr)
        {
            q.push(node->LChild);
            // storing node details
            oss << "\t\"node" << node->key << "\":l -> \"node" << node->LChild->key << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }

        // Enqueue right child
        if (node->RChild != nullptr)
        {
            q.push(node->RChild);
            // storing node details
            oss << "\t\"node" << node->key << "\":r -> \"node" << node->RChild->key << "\":d;\n";
            pointerString.append(oss.str());
            oss.str("");
            oss.clear();
        }
        q.pop();
    }
    stream << nodeString;
    stream << "\t\"node" << head->key << "\":r -> \"node" << head->RChild->key << "\":d;\n";
    stream << pointerString;
    stream << "}";
    stream.close();

    // generate tree image
    // string cmd = "dot -Tpng graph.gv -o " + string(filename) + ".png";
    //system((const char *)cmd.c_str());
}

int AVL_Tree::getHeightUtils(AVL_Node *root)
{
    if (root == nullptr || (root->LChild == nullptr && root->RChild == nullptr))
        return 0;
    return 1 + max(getHeightUtils(root->LChild), getHeightUtils(root->RChild));
}

int AVL_Tree::getTreeHeight()
{
    return getHeightUtils(head->RChild);
}

void AVL_Tree::getAvgTreeHeightUtils(AVL_Node *root, float &sum, int &totalNodes)
{
    if (root == nullptr)
        return;
    getAvgTreeHeightUtils(root->LChild, sum, totalNodes);
    sum += getHeightUtils(root);
    ++totalNodes;
    getAvgTreeHeightUtils(root->RChild, sum, totalNodes);
}

float AVL_Tree::getAvgTreeHeight()
{
    float sum = 0;
    int totalNodes = 0;
    getAvgTreeHeightUtils(head->RChild, sum, totalNodes);
    cout << sum << "- sum\n";
    cout << totalNodes << "- nodes\n";
    return sum / totalNodes;
}

int AVL_Tree::getTotalKeyComparisons()
{
    return totalKeyComparisons;
}

int AVL_Tree::getTotalRotations()
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

void readInputFile(AVL_Tree *avl)
{
    ifstream file;
    file.open("inputFile.txt");
    string operation;
    int value;
    while (file >> operation)
    {
        file >> value;
        if (operation.compare("Insert") == 0)
            avl->insertAVLNode(value);
        else
            avl->deleteAVLNode(value);
    }
    file.close();
}

int main()
{
    AVL_Tree *avl = new AVL_Tree();
    generateInput();
    readInputFile(avl);
    avl->printTree();
    printf("\n");
    cout << avl->getTreeHeight() << "- Tree Height\n";
    cout << avl->getAvgTreeHeight() << "- Avg Height\n";
    cout << avl->getTotalKeyComparisons() << "- key comp\n";
    cout << avl->getTotalRotations() << "- rotations\n";
    return 0;
}