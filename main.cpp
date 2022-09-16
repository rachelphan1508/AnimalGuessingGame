#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>

using namespace std;

#pragma region Node
class Node
{
public:
    string question;
    Node *yesAnswer;
    Node *noAnswer;
    Node(string question)
    {
        this->question = question;
        yesAnswer = NULL;
        noAnswer = NULL;
    }
    Node()
    {
    }
};
#pragma endregion Node

void PrintQuestion(Node *node)
{
    cout << node->question << endl;
}

Node *MoveToYes(Node *cur)
{
    return cur->yesAnswer;
}
Node *MoveToNo(Node *cur)
{
    return cur->noAnswer;
}
void AddQuestionToNode(Node *&cur, string question, string yesAnswer, string noAnswer)
{
    Node *node = new Node(question);
    node->yesAnswer = new Node(yesAnswer);
    node->noAnswer = new Node(noAnswer);
    cur = node;
}

#pragma region TreeTraversal
/// @brief get the Inorder Traversal of the tree
/// @param node
/// @param inorderList
void GetInorderList(Node *node, vector<string> &inorderList)
{
    if (node == NULL)
        return;

    /* first recur on left child */
    GetInorderList(node->yesAnswer, inorderList);

    /* then get the data of node */
    inorderList.push_back(node->question);

    /* now recur on right child */
    GetInorderList(node->noAnswer, inorderList);
};

/// @brief get the Preorder Traversal of the tree
/// @param node
/// @param preorderList
void GetPreOrderList(Node *node, vector<string> &preorderList)
{
    if (node == NULL)
        return;

    /* get the data of node */
    preorderList.push_back(node->question);

    /* recur on left child */
    GetPreOrderList(node->yesAnswer, preorderList);

    /* now recur on right child */
    GetPreOrderList(node->noAnswer, preorderList);
};
#pragma endregion TreeTraversal

#pragma region BuildTree

// Build the binary tree from inorder and preorder traversal
Node *
BuildTreeHelper(vector<string> preOrderList, vector<string> inOrderList, int startPre, int startIn, int endIn)
{
    if (startIn == endIn)
        return NULL;
    Node *head = new Node(preOrderList[startPre]);
    int split;
    for (int i = startIn; i < endIn; i++)
    {
        if (inOrderList[i] == preOrderList[startPre])
        {
            split = i;
        }
    }
    split = split - startIn;
    head->yesAnswer = BuildTreeHelper(preOrderList, inOrderList, startPre + 1, startIn, startIn + split);
    head->noAnswer = BuildTreeHelper(preOrderList, inOrderList, startPre + 1 + split, startIn + split + 1, endIn);
    return head;
}

Node *BuildTree(vector<string> preOrderList, vector<string> inOrderList)
{
    if (preOrderList.size() != inOrderList.size())
        return NULL;
    int n = preOrderList.size();
    return BuildTreeHelper(preOrderList, inOrderList, 0, 0, n);
}
#pragma endregion BuildTree

#pragma region ReadAndWriteToFile

void WriteVectorToFile(vector<string> v, string filename)
{
    ofstream myfile(filename);
    myfile.clear();
    for (const auto &e : v)
    {
        myfile << e << '\n';
    }
    myfile.close();
}
void WriteDataToFile(Node *root)
{
    Node *head = root;
    vector<string> inorderList;
    vector<string> preorderList;
    GetInorderList(head, inorderList);
    GetPreOrderList(head, preorderList);

    WriteVectorToFile(inorderList, "inorder.txt");
    WriteVectorToFile(preorderList, "preorder.txt");
}

vector<string> getListFromFile(string filename)
{
    vector<string> res;
    ifstream myfile(filename);
    if (myfile.is_open())
    {
        string curline;
        while (getline(myfile, curline))
        {
            res.push_back(curline);
        }
    }
    myfile.close();
    return res;
}
Node *BuildTreeFromFile()
{
    vector<string> inorderList = getListFromFile("inorder.txt");
    vector<string> preorderList = getListFromFile("preorder.txt");
    return BuildTree(preorderList, inorderList);
}

#pragma endregion ReadAndWriteToFile
int main()
{
    string playAgain = "Yes";
    string userAnswer = "";

    while (playAgain == "Yes" || playAgain == "y")
    {
        cout << "Do you want to use previous data (1) or you want to train me from the beginning (2)?" << endl;
        int gameMode;
        cin >> gameMode;
        while (gameMode != 1 && gameMode != 2)
        {
            cout << "Please enter 1 to use previous data or 2 to train me from the beginning." << endl;
            cin >> gameMode;
        }
        bool gameFinished = false;
        Node *root = new Node();
        cout << "Okay! Let's get started. \nThink of an animal and answer my questions with 'y' for Yes and 'n' for No." << endl;
        if (gameMode == 1)
        {
            // Read data from file and build Tree
            root = BuildTreeFromFile();
        }
        else if (gameMode == 2)
        {
            AddQuestionToNode(root, "Does it swim underwater?", "Is this a shark?", "Is this a horse?");
        }
        Node *cur = root;

        // pause for 3 seconds before starting -- let user think about the animal
        sleep(3);

        while (!gameFinished)
        {
            PrintQuestion(cur);
            cin >> userAnswer;

            // // if user answer Yes and we are at the leaf, game is over and the guess was right
            if (userAnswer == "y" && !cur->yesAnswer)
            {
                cout << "Yayyyy. I got it right!" << endl;
                gameFinished = true;
            }
            else if (userAnswer == "y")
            {
                cur = MoveToYes(cur);
            }
            else if (userAnswer == "n" && cur->noAnswer)
            {
                cur = MoveToNo(cur);
            }
            // if the guess is wrong
            else
            {
                // get the animal that was guessed
                int lastSpace = cur->question.find_last_of(' ');
                string guessedAnimal = cur->question.substr(lastSpace + 1, cur->question.size() - lastSpace - 2);

                // get the animal that user were thinking of
                cout << "oh. I guessed it wrong! Which animal were you thinking of?" << endl;
                string userAnimal;
                cin >> userAnimal;

                // ask user to add a question
                cout << "Please give me a question to differiate " << userAnimal << " with " << guessedAnimal << "." << endl;
                string userQuestion;
                cin.ignore();
                getline(cin, userQuestion);

                // ask user which answer (y/n) for the question they gave
                cout << "Should a " << userAnimal << " be a y/n to that question?" << endl;
                string userAnimalAnswer;
                cin >> userAnimalAnswer;

                string curQuestion = cur->question;

                // make the animal that user added become a question.
                string userAnimalQuestion = "Is it a " + userAnimal + "?";

                // replace current question with that user question.
                cur->question = userQuestion;

                if (userAnimalAnswer == "y")
                {
                    // make cur->yesAnswer the userAnimal
                    cur->yesAnswer = new Node(userAnimalQuestion);
                    cur->noAnswer = new Node(curQuestion);
                }
                else
                {
                    // make cur->noAnswer the userAnimal
                    cur->noAnswer = new Node(userAnimalQuestion);
                    cur->yesAnswer = new Node(curQuestion);
                }
                cout << "Thank you! Test how I learnt from you by playing again :D " << endl;

                gameFinished = true;
            }
        }

        // after the game, data from this game will be saved in files. So previous data will be replaced.
        WriteDataToFile(root);

        cout << "Do you want to play again? (y/n)" << endl;
        cin >> playAgain;
    }

    cout << "Hope you had fun! Thanks for playing along with me!" << endl;
    return 0;
}