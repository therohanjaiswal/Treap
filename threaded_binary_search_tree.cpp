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

// ListNode represents linkedlist node structure
class ListNode
{

private:
    int data;       // store element
    ListNode *next; // points to next linkedlist node

public:
    // constructor
    ListNode(int aData)
    {
        data = aData;
        next = nullptr;
    }
    friend class LinkedList;
};

// Linked List class
class LinkedList
{
    ListNode *head; // head pointer pointing to first node

public:
    // constructor
    LinkedList()
    {
        head = nullptr;
    }

    // This function adds a new node in the end of the linked list.
    void addNode(int aData)
    {
        ListNode *new_node = new ListNode(aData); // creating new node
        if (head == nullptr)                      // for first insertion i.e., list is empty
        {
            head = new_node;
            return;
        }
        // if list is not empty
        ListNode *temp = head;
        while (temp->next != nullptr)
            temp = temp->next;
        temp->next = new_node;
    }

    // This function prints all the elements in the linked list.
    void display()
    {
        ListNode *temp = head;
        while (temp != nullptr)
        {
            cout << temp->data << " ";
            temp = temp->next;
        }
    }
    friend class TBST;
};

// Node class represents threaded binary search tree node structure
class Node
{
private:
    int data;         // store element
    Node *leftChild;  // points to left child node if exists, else inorder predecessor
    Node *rightChild; // points to right child node if exists, else inorder successor
    bool leftThread;  // true if left child node exists else false
    bool rightThread; // true if right child node exists else false
    int leftCount;    // stores the number of nodes in left sub tree
    int rightCount;   // stores the number of nodes in right sub tree

public:
    // constructor
    Node(int aData)
    {
        data = aData;
        leftCount = 0;
        rightCount = 0;
    }
    friend class TBST;
};

/* Threaded Binary Search Tree class.
This tree consist of a dummy node and the actual 
TBST exists as the left child of the dummy node*/
class TBST
{
private:
    Node *root; // root pointer pointing to dummy node

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
    }

    // This function performs insertion in the Threaded Binary Search Tree
    void insert(int data)
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
                        // increase the count of number of elements in the left subtree of curr node
                        increaseCount(node);
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
                        // increase the count of number of elements in the right subtree of curr node
                        increaseCount(node);
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
                cout << x << " is already present";
                return;
            }
        }
    }

    /* utility function to increase the count of number of nodes present in left 
        and right subtree of curr node*/
    void increaseCount(Node *curr)
    {
        Node *temp = root->leftChild; // temp points to left child of dummy node i.e., root of actual TBST
        while (temp != curr)
        {
            if (curr->data < temp->data) // if curr node data is less than temp node data
            {
                temp->leftCount++;
                temp = temp->leftChild;
            }
            else // otherwise
            {
                temp->rightCount++;
                temp = temp->rightChild;
            }
        }
    }

    /* Fucntion searches for an element in the TBST*/
    Node *search(int key)
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

    /*Function deletes the node containing key from the tree*/
    void deleteNode(int key)
    {
        bool isFound = false;
        isFound = search(key);
        // if key is not present in the tree
        if (!isFound)
        {
            cout << key << " is not present in the tree";
            return;
        }

        // if key is present in the tree
        Node *curr = root->leftChild; // // curr points to left child of dummy node i.e., root of actual TBST
        Node *par = nullptr;          // points to the parent of curr node
        while (curr->data != key)     // until the key is found in the tree
        {
            par = curr;
            if (key < curr->data)
            {
                curr = curr->leftChild;
            }
            else
            {
                curr = curr->rightChild;
            }
        }

        // if the node to be deleted is a leaf node
        if (!curr->leftThread && !curr->rightThread)
        {
            deleteLeafNode(par, curr);
            decreaseCount(par); // decrease the count of nodes of left and right subtree of par node
        }
        // if the node to be deleted is a node with single child
        else if ((!curr->leftThread && curr->rightThread) || (curr->leftThread && !curr->rightThread))
        {
            deleteSingleChildNode(par, curr);
            decreaseCount(par); // decrease the count of nodes of left and right subtree of par node
        }
        else // if the node to be deleted has two children
        {
            deleteDoubleChildNode(par, curr);
        }
    }

    /*Utility Function to delete the leaf node. It takes curr pointer pointing to the 
    node to be deleted and par pointer pointing to the parent of curr node. It 
    returns the address of deleted node*/
    Node *deleteLeafNode(Node *par, Node *curr)
    {
        if (curr->data < par->data) // when the curr node is left child of par node
        {
            par->leftChild = curr->leftChild;
            par->leftThread = curr->leftThread;
            par->leftCount--;
        }
        else // when the curr node is right child of par node
        {
            par->rightChild = curr->rightChild;
            par->rightThread = curr->rightThread;
            par->rightCount--;
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
    Node *deleteSingleChildNode(Node *par, Node *curr)
    {
        Node *leftMost, *rightMost;
        if (curr->data < par->data) // if the curr node is left child of parent node
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
            par->leftCount--; // decreasing the count of nodes present in left sub tree of parent
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
            par->rightCount--; // decreasing the count of nodes present in the right sub tree of parent
        }

        curr->leftChild = nullptr;
        curr->rightChild = nullptr;
        curr->leftThread = false;
        curr->rightThread = false;
        return curr; // returning the deleted node
    }

    /*Utility Function to delete the node with two child nodes. It takes curr pointer pointing to the 
    node to be deleted and par pointer pointing to the parent of curr node. */
    void deleteDoubleChildNode(Node *par, Node *curr)
    {
        // finds the leftmost of right sub tree of current node (inorder successor of curr node)
        Node *leftMost = leftMostNode(curr->rightChild);
        // prev points to parent of temp node, temp points to right child of curr node
        Node *prev, *temp = curr->rightChild;

        // finding the parent of leftmost node and storing its address in prev
        while (temp->data != leftMost->data)
        {
            prev = temp;
            if (leftMost->data < temp->data)
                temp = temp->leftChild;
            else
                temp = temp->rightChild;
        }

        decreaseCount(prev); // // decrease the count of nodes of left and right subtree of prev node

        // deleting the leftmost node when it is a leaf node
        if (!leftMost->leftThread && !leftMost->rightThread)
        {
            temp = deleteLeafNode(prev, leftMost);
        }
        else // deleting the left most node when it is a node with single child
        {
            temp = deleteSingleChildNode(prev, leftMost);
        }
        //changing the data value of curr node to inorder successor data value
        curr->data = temp->data;
        free(temp);
    }

    /* utility function to decrease the count of number of nodes present in left 
        and right subtree of par node*/
    void decreaseCount(Node *par)
    {
        Node *temp = root->leftChild;
        while (temp != par)
        {
            if (par->data < temp->data)
            {
                temp->leftCount--;
                temp = temp->leftChild;
            }
            else
            {
                temp->rightCount--;
                temp = temp->rightChild;
            }
        }
    }

    /*Function performs the reverse inorder traversal and return  a linked list 
    containg the elements from max to min*/
    ListNode *reverseInorder()
    {
        LinkedList list;                             // linked list to store the elements
        Node *temp = rightMostNode(root->leftChild); //find the rightmost node of the tree

        while (temp != root) //till the temp points to dummy node
        {
            list.addNode(temp->data);                  // add a node to the list
            if (temp->leftThread)                      // if the temp node has left child
                temp = rightMostNode(temp->leftChild); //move to the rightmost node of the left subtree
            else
                temp = temp->leftChild; // move to the left child
        }
        list.display();
        return list.head; // return the head pointer of linked list
    }

    /*Inorder traversal of the threaded binary search tree*/
    void inorder(Node *curr)
    {
        while (curr->leftThread)
            curr = curr->leftChild;
        // Loop to traverse the tree
        while (curr != root)
        {
            cout << curr->data << " ";
            curr = inorderSuccessor(curr);
        }
    }

    /*Utility function to find the inorder successor of curr node*/
    Node *inorderSuccessor(Node *curr)
    {
        if (!curr->rightThread)      // if the curr node has no right child
            return curr->rightChild; // return the rightchild
        else                         // if it has right child
            curr = curr->rightChild; // point to right child

        return leftMostNode(curr); // return the left most node of curr right child
    }

    /*Function to find the successor of the curr node*/
    int successor(Node *curr)
    {
        return inorderSuccessor(curr)->data;
    }

    /*Utility function to find the leftmost child node of curr node*/
    Node *leftMostNode(Node *curr)
    {
        while (curr->leftThread)
            curr = curr->leftChild;
        return curr;
    }

    /*Utility function to find the rightmost child node of curr node*/
    Node *rightMostNode(Node *curr)
    {
        while (curr->rightThread)
            curr = curr->rightChild;
        return curr;
    }

    /*Utility Function o split the tree*/
    Node *splitUtil(Node *root, int k)
    {
        // Base case when the root is null
        if (root == NULL)
        {
            Node *node = new Node(INT_MAX);
            node->leftChild = nullptr;
            node->rightChild = nullptr;
            return node;
        }

        // if k exists in the left subtree
        if (root->data > k)
        {
            Node *temp = !root->leftThread ? nullptr : root->leftChild;
            Node *left = splitUtil(temp, k); // splitting the leftsubtree recursively
            root->leftChild = left->rightChild;
            Node *result = new Node(INT_MAX);
            result->leftChild = left->leftChild;
            result->rightChild = root;
            return result;
        }
        else //if the root exists in the right subtree
        {
            Node *temp = !root->rightThread ? nullptr : root->rightChild;
            Node *right = splitUtil(temp, k); // splitting the leftsubtree recursively
            root->rightChild = right->leftChild;
            Node *result = new Node(INT_MAX);
            result->leftChild = root;
            result->rightChild = right->rightChild;
            return result;
        }
    }

    /*Function perform splitting of tbst on value k */
    void split(int k)
    {
        Node *leftMost = leftMostNode(root->leftChild);
        Node *rightMost = rightMostNode(root->leftChild);
        leftMost->leftChild = nullptr;
        rightMost->rightChild = nullptr;
        Node *result = splitUtil(root->leftChild, k);
        inorder(result->leftChild);
        cout << "---------------------\n";
        inorder(result->rightChild);
    }

    /*Function to find all elements between k1 and k2*/
    ListNode *allElementsBetweenK1AndK2(int k1, int k2)
    {
        Node *temp = root->leftChild, *start = nullptr;
        while (temp != nullptr) //till temp is not nullptr and the data is not equal to k1
        {
            if (k1 <= temp->data) //if k1 is less than temp
            {
                if (start == nullptr || (start->data >= temp->data && temp->data >= k1))
                    start = temp;                                    //update start
                temp = temp->leftThread ? temp->leftChild : nullptr; //mode temp to left
            }
            else //otherwise
            {
                if ((start->data >= temp->data && temp->data >= k1))
                    start = temp;                                      //update end
                temp = temp->rightThread ? temp->rightChild : nullptr; //move temp to right
            }
        }

        LinkedList list;
        temp = start;
        while (temp->data <= k2)
        {
            list.addNode(temp->data);
            temp = inorderSuccessor(temp);
        }
        list.display();
        return list.head;
    }

    /*Function to find the kth largest element of the tree*/
    void kthElement(int k)
    {
        Node *curr = root->leftChild; // curr points to left child of dummy node i.e., root of actual TBST

        // if k is greater than total number of elements present in the tree
        if (k > (curr->leftCount + curr->rightCount + 1))
        {
            cout << k << "th largest does not exist";
            return;
        }

        // if k is less then total number of elements present in the tree
        while (true)
        {
            if ((curr->rightCount + 1) == k) // if k == (total number of elements in right subtree + 1)
            {
                cout << curr->data << "\t";
                return;
            }
            else if ((curr->rightCount + 1) < k) // if kth largest exists in the left sub tree
            {
                k = k - (curr->rightCount + 1);
                curr = curr->leftChild;
            }
            else // if kth largest exist in the right sub tree
            {
                curr = curr->rightChild;
            }
        }
    }

    /*Function to form the "graph.gv" file which helps to generate the
    picture of the tree*/
    void printTree()
    {
        // forming initial part of "graph.gv file"
        ofstream stream;
        string str = "grapgh.gv";
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
};

int main()
{
    TBST tbst;
    bool iteration = true;
    while (iteration)
    {
        int choice = 0;
        cout << "\n---------------------------------------";
        cout << "\n\tThreaded Binary Tree";
        cout << "\n---------------------------------------";
        cout << "\nWhat do you want to perform?";
        cout << "\n1. Insertion";
        cout << "\n2. Search";
        cout << "\n3. Deletion";
        cout << "\n4. Reverse Inorder Traversal";
        cout << "\n5. Successor";
        cout << "\n6. Split Tree on K";
        cout << "\n7. Find all elements between k1 and k2";
        cout << "\n8. Kth Largest Element";
        cout << "\n9. Print tree";
        cout << "\nEnter your choice: ";
        cin >> choice;
        int userData;

        switch (choice)
        {
        // Insertion operation
        case 1:
            cout << "\nEnter the number: ";
            cin >> userData;
            if (floor(userData) == userData) // checking whether the input data is a number or not
            {
                tbst.insert(userData);
                cout << "\nInsertion done successfully.";
            }
            else
                cout << "\n Enter a valid integer!!";
            break;

        // Search operation
        case 2:
            cout << "\nEnter the number: ";
            cin >> userData;
            if (floor(userData) == userData) // checking whether the input data is a number or not
            {
                try
                {
                    if (tbst.search(userData) != nullptr)
                        cout << "\nElement " << userData << " is present in the tree ";
                    else
                        throw userData;
                }
                catch (int x)
                {
                    cout << "\nElement " << userData << " is not present in the tree ";
                }
            }
            else
                cout << "\n Enter a valid integer!!";
            break;

        // Deletion operation
        case 3:
            cout << "\nEnter the number: ";
            cin >> userData;
            if (floor(userData) == userData) // checking whether the input data is a number or not
            {
                try
                {
                    if (tbst.search(userData) != nullptr)
                    {
                        tbst.deleteNode(userData);
                        cout << "\nDeletion done successfully";
                    }
                    else
                        throw userData;
                }
                catch (int x)
                {
                    cout << "\nElement " << x << " is not present in the tree ";
                }
            }
            else
                cout << "\n Enter a valid integer!!";
            break;

        // reverse inorder traversal
        case 4:
            if (tbst.reverseInorder() == nullptr)
                cout << "\nTree is empty!";
            break;

        // successor node
        case 5:
            cout << "\nEnter the number: ";
            cin >> userData;
            if (floor(userData) == userData) // checking whether the input data is a number or not
            {
                try
                {
                    Node *temp = tbst.search(userData); // searches the user data
                    if (temp != nullptr)
                    {
                        cout << "\nInorder successor of " << userData << " is " << tbst.successor(temp);
                    }
                    else
                        throw userData;
                }
                catch (int x)
                {
                    cout << "\nElement " << x << " is not present in the tree";
                    ;
                }
            }
            else
                cout << "\n Enter a valid integer!!";
            break;

        // split the tree
        case 6:
            cout << "\nEnter the number k: ";
            cin >> userData;
            if (floor(userData) == userData) // checking whether the input data is a number or not
            {
                tbst.split(userData);
            }
            else
                cout << "\n Enter a valid integer!!";
            break;

        // find elements between k1 and k2
        case 7:
            int k1, k2;
            cout << "\nEnter k1: ";
            cin >> k1;
            cout << "\nEnter k2: ";
            cin >> k2;
            if (tbst.allElementsBetweenK1AndK2(k1, k2) == nullptr)
                cout << "\nTree is empty!";
            break;

        // find kth largest element
        case 8:
            cout << "\nEnter the number: ";
            cin >> userData;
            if (floor(userData) == userData)
                tbst.kthElement(userData);
            else
                cout << "\n Enter a valid integer!!";
            break;

        // print the tree
        case 9:
            tbst.printTree();
            cout << "\ngraph.gv file generated.";
            cout << "\nRun the following command to generate the grapgh.png";
            cout << "\ndot -Tpng grapgh.gv -o graph.png";
            break;

        default:
            cout << "\nInvalid Choice";
        }

        char nextIteration = 'n';
        cout << "\nWant to perform more operations (y/n)?";
        cin >> nextIteration;
        if (nextIteration == 'y' || nextIteration == 'Y')
            iteration = true;
        else if (nextIteration == 'n' || nextIteration == 'N')
            iteration = false;
        else
        {
            iteration = false;
            cout << "\nInvalid input!";
        }
    }
    return 0;
}
