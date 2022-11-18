#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"


struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    virtual void insertFix(AVLNode<Key,Value>*p, AVLNode<Key,Value>* n);
    virtual int balanceCalc(AVLNode<Key, Value>* value);
    virtual int balanceHelper(AVLNode<Key, Value>* value);
    virtual void leftRotate(AVLNode<Key,Value>* p,AVLNode<Key,Value>* n);
    virtual void rightRotate(AVLNode<Key,Value>* p,AVLNode<Key,Value>* n);
    virtual void removeFix(AVLNode<Key,Value>* n , int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  //bool isLeftChild = false;
  AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
  if (temp == NULL){
      AVLNode<Key, Value>* nodeToInsert = new AVLNode<Key, Value> (new_item.first,new_item.second, NULL);
      this ->root_=nodeToInsert;
      return;}
  if (BinarySearchTree<Key, Value>::internalFind(new_item.first)!=NULL){ //if key already exists, no need to change balance
      BinarySearchTree<Key, Value>::internalFind(new_item.first)->setValue(new_item.second);
      return;
  }
  
  AVLNode<Key, Value>* nodeToInsert = new AVLNode<Key, Value> (new_item.first,new_item.second, NULL);

    while (temp !=NULL){
        //moves the "iterator" (temp) until there is a suitable place for insertion;
        if (nodeToInsert->getKey() < temp->getKey()){
          if (temp->getLeft() ==NULL){
            nodeToInsert->setParent(temp);
            temp->setLeft(nodeToInsert);
            break;}
          temp = temp->getLeft();
        }
        else if (nodeToInsert->getKey() > temp->getKey()){
          if (temp->getRight() == NULL){
          
            nodeToInsert->setParent(temp);
            temp ->setRight(nodeToInsert);
            break;
          }
          temp=temp->getRight();
        }
    }
    AVLNode<Key, Value>* insertedNodeParent = nodeToInsert->getParent();
    if (insertedNodeParent->getBalance() ==-1 || insertedNodeParent->getBalance() ==1){
      insertedNodeParent->setBalance(0);
    }
    //if parent's balance was -1 or 1, we change to 0 and return, else we call insertFix
    else{ //if parent's balance ==0;
      int val = (insertedNodeParent->getLeft() == nodeToInsert) ? -1 : 1;
      insertedNodeParent->updateBalance(val); 
      insertFix(insertedNodeParent, nodeToInsert);
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n ){
  // where p is our inserted node's parent, and g is its grandparent
  if (p ==NULL){return;}
  if (p->getParent() == NULL){return;}
  AVLNode<Key,Value>* g = p->getParent();

  if (g->getLeft() ==p){ //if p is left child of g
    g->updateBalance(-1);
    if(g->getBalance()==0){return;}
    else if( g->getBalance()==-1) {insertFix(g,p);}

    else{ //if g's balance ==-2
      if (p->getLeft() ==n){//zig-zig
        rightRotate(g,p);
        g->setBalance(0);
        p->setBalance(0);
      }
      else if(p->getRight() ==n){  //zig-zag
        leftRotate(p,n);
        rightRotate(g,n); 
        //update balances here
        int nBalance = n->getBalance(); 
        if (nBalance ==0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if(nBalance==-1){
          p->setBalance(0);
          g->setBalance(1);
          n->setBalance(0);
        }
        else if (nBalance==1){
          p->setBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
  else{ //p is right child of g
    g->updateBalance(1);
    if(g->getBalance()==0){return;}
    else if (g->getBalance()==1){insertFix(g,p);}
    else{  
      if(p->getRight() ==n){ //zig-zig
        leftRotate(g,p);
        g->setBalance(0);
        p->setBalance(0);
      }

      else if(p->getLeft() ==n){
        rightRotate(p,n);
        leftRotate(g,n);
        //update balances here
        int nBalance = n->getBalance(); 
        if (nBalance ==0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if(nBalance==1){
          p->setBalance(0);
          g->setBalance(-1);
          n->setBalance(0);
        }
        else if (nBalance==-1){
          p->setBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key,Value>* p,AVLNode<Key,Value>* n){
  //where p is the node that gets moved down.
  AVLNode<Key,Value>* g = p->getParent();
  p->setRight(n->getLeft());
  if(n->getLeft()!=NULL){(n->getLeft()) ->setParent(p);}

  n->setLeft(p);
  n->setParent(g);
  p->setParent(n);
  if (g==NULL){ //means p was root.
    this->root_= n;
  }
  else if (g->getRight() ==p){
    g->setRight(n);
  }
  else{
    g->setLeft(n);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key,Value>* p,AVLNode<Key,Value>* n){
  //where p is the node that gets moved down.
  //p->setRight(n->getRight());
  AVLNode<Key,Value>* g = p->getParent();
  p->setLeft(n->getRight());
  if (n->getRight() !=NULL){(n->getRight())->setParent(p); }

  n->setRight(p);
  n->setParent(g);
  p->setParent(n);
  if (g==NULL){ //means p was root.
    this->root_= n;
  }
  else if (g->getRight() ==p){
    g->setRight(n);
  }
  else{
    g->setLeft(n);
  }
  
}
template<typename Key, typename Value>
int AVLTree<Key, Value>::balanceCalc(AVLNode<Key, Value>* value){
  if (value == NULL){
        //balanceCheck = true;
        return 0;
    }
    int left = balanceHelper(value->getLeft());
    int right = balanceHelper(value->getRight());
    int diff = (right-left);
    return diff;

}
template<typename Key, typename Value>
int AVLTree<Key, Value>::balanceHelper(AVLNode<Key, Value>* value)
{
    // copied from bst.h

    //finds longest path down each subtree,
    //is value is NULL
    if (value ==nullptr){
        //balanceCheck = true;
        return 0;
    }

    //left child has longer or equal path to right child
    if(balanceHelper(value->getLeft()) >= balanceHelper(value->getRight())){
        return 1+balanceHelper(value->getLeft());
    }
    //if right child has longer path
    else{
        return 1+balanceHelper(value->getRight());
    }
    
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    
    AVLNode<Key, Value>* location = static_cast<AVLNode<Key, Value>*>(this->internalFind(key)); 
    bool isLeftChild= false; 
    int diff=0;
    if (location ==NULL){return;}
    if(location->getRight()!=NULL&&location->getLeft()!=NULL){ //2 child case
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(location));
      nodeSwap(pred,location);
      //since location now points to where pred was, the below code will "fix" for if it has 0 or 1 children.
    }

    AVLNode<Key, Value>* locationParent = location->getParent();
    //checks if location was left or right child. 
    //if node has no children
    if (location->getRight() == NULL && location ->getLeft() ==NULL){
      if (location ==this->root_){
        delete location;
        this->root_ =NULL;
        return;
      }
    if (locationParent->getLeft()== location){isLeftChild =true;}
      
    //check if it is left or right child. 
    if(locationParent->getLeft() == location){
      locationParent->setLeft(NULL);
    }
    else{
      locationParent->setRight(NULL);
    }
    delete location;
    //return;
    }

    else{ //node has 1 child
      AVLNode<Key,Value>* c;
      if(location->getRight()!= NULL){ //right child exists
        c = location->getRight();
      }
      else{
        c = location ->getLeft();
      }
      c->setParent(locationParent);

      if (location ==this->root_){
        //delete this->root_;
        this->root_=c;
        //return;
      }
      else{
        if(locationParent->getLeft()==location){ 
          isLeftChild=true;
          //if removed node was a left child
          locationParent->setLeft(c);
        }
        else{
          //if removed node was right child
          locationParent->setRight(c);

        }
      }
    
      delete location; 
    }
    if(locationParent!=NULL){
      if(isLeftChild){
        diff=1;
      }
      else{
        diff = -1;
      }
    }
    removeFix((locationParent), diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>:: removeFix(AVLNode<Key,Value>* n , int diff)
{

  
  if (n==NULL){return;}
  AVLNode<Key,Value>* p = n->getParent();
  //if (p==NULL){return;} 
  int ndiff=0;
  if (p!=NULL){
    if (p->getLeft() == n){ndiff =1;}
    else{ndiff=-1;}
  }

  

  int nBalance = n->getBalance(); 
  if (diff ==-1){ 
    AVLNode<Key,Value>* c = n->getLeft();
    int cBalance;
    if (c!=NULL){ 
      cBalance = c->getBalance();
      if (nBalance+diff ==-2){
        if (cBalance == -1){ //zig-zig
          rightRotate(n,c);
          n->setBalance(0);
          c->setBalance(0);
          removeFix(p,ndiff); // 
        }
        else if (cBalance==0){ // zig-zig 
          rightRotate(n,c);
          n->setBalance(-1);
          c->setBalance(1);
        }
        else if (cBalance==1){
          AVLNode<Key,Value>* g = c->getRight();
          int gBalance = g->getBalance();
          leftRotate(c,g);
          rightRotate(n,g);
          if (gBalance ==1){
            n->setBalance(0);
            c->setBalance(-1);
            g->setBalance(0);
          }
          else if(gBalance==0){
            n->setBalance(0);
            c->setBalance(0);
            g->setBalance(0);
          }
          else if(gBalance==-1){
            n->setBalance(1);
            c->setBalance(0);
            g->setBalance(0);
          }
          removeFix(p,ndiff);
        }
      }
    }
    if (nBalance+diff == -1){n->setBalance(-1);}
    if (nBalance+diff == 0){
      n->setBalance(0);
      removeFix(p,ndiff);
    }
  }

  else if (diff == 1){
      AVLNode<Key,Value>* c = n->getRight();
      int cBalance; 
      if (c!=NULL){
        cBalance = c->getBalance();
        if (nBalance+diff == 2){
          if (cBalance == 1){ //zig-zig
            leftRotate(n,c);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p,ndiff); // 
          }
          else if (cBalance==0){ // zig-zig 
            leftRotate(n,c);
            n->setBalance(1);
            c->setBalance(-1);
          }
          else if (cBalance==-1){
            AVLNode<Key,Value>* g = c->getLeft();
            int gBalance = g->getBalance();
            rightRotate(c,g);
            leftRotate(n,g);
            if (gBalance ==-1){
              n->setBalance(0);
              c->setBalance(1);
              g->setBalance(0);
            }
            else if(gBalance==0){
              n->setBalance(0);
              c->setBalance(0);
              g->setBalance(0);
            }
            else if(gBalance==1){
              n->setBalance(-1);
              c->setBalance(0);
              g->setBalance(0);
            }
            removeFix(p,ndiff);
          }
        }
      }
      if (nBalance+diff==1){n->setBalance(1);}
      if (nBalance+diff ==0){
        n->setBalance(0);
        removeFix(p,ndiff);
      }
  }
}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
