#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    // Add your code below
    if (root ==NULL) {return true;}
    else if ((root->left ==NULL) && (equalPathsHelper(root->right)==1)){return true;}
    else if ((equalPathsHelper(root->left)==1) && (root->right ==NULL)){return true;}
    //if there's either a left or right child
    int leftPath = equalPathsHelper(root->left);
    int rightPath= equalPathsHelper(root->right);
    if(leftPath==rightPath){return true;}
    return false;
}

int equalPathsHelper(Node* value){
    //finds longest path down each subtree,
    //is value is NULL
    if (value == NULL){
        return 0;
    }
    //left child has longer or equal path to right child
    if(equalPathsHelper(value->left) >= equalPathsHelper(value->right)){
        return 1+equalPathsHelper(value->left);
    }
    //if right child has longer path
    else{
        return 1+equalPathsHelper(value->right);
    }
}

