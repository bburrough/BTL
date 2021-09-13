#ifndef _RBTREE_H_
#define _RBTREE_H_

/*  red black tree 

    Bob Burrough, 2021

    Store items in a tree and retrieve them with O(log N) time complexity.
    Does not use recursion and does not allocate extra memory (e.g. stacks
    and queues) during iteration. The type stored in the tree must have
    a meaningful operator==() and operator<() to facilitate storage in and
    retrieval from the tree.

    Differs from the Morris traversal edition in that each node has a pointer to
    its parent, and the tree remains unmodified during traversal. To understand
    which tree to select for your project:

                                        Aborted    Complete          Memory
                        Insert  Search  Traversal  Traversal         Usage
                        ------  ------  ---------  ---------         -----
    RBTree<T>            fast    slow     fast       fast     3 ptrs + 1 enum per node
    RBTreeMorris<T>      slow    slow    slowest     slow             TBD
    AVLTree<T>           fast    fast    fastest     fast     3 ptrs + 1 int per node
    AVLTreeMorris<T>     slow    fast     slow       slow     2 ptrs + 1 int per node

    Note, if you are so memory constrained that you are considering using
    AVLTreeMorris<T>, make sure your compiler is configured for proper padding
    and alignment to benefit from AVLTreeMorris<T>'s smaller node sizes. */

template<class T>
class RBTree
{
public:
	RBTree();
	virtual ~RBTree();
    RBTree(RBTree<T>&& other); // move constructor

	// Retrieve item from the tree. Complexity os O(log N).
	bool Search(const T& item) const;

	// Place an item in the tree. Complexity is O(log N).
	void Insert(const T& item);

	// Remove item from the tree. Complexity is O(log N).
	void Remove(const T& item);

    void Clear();

    /* Create the intersection of this tree with another. 
       Complexity is O(N), where N is the number of elements
       in the smaller of the two trees. */
    template<typename U>
    RBTree<T> Intersect(const U& other) const;

	/* Consistency check. Returns true if the red black tree
       is internally consistent. Otherwise, false. */
	bool IsValid() const;
  
protected:
private:
	class Node
	{
		/* This is implemented as a nested class to avoid
		   presenting unnecessary interfaces to the calling
		   program. Node is purely an internal storage
		   concept of RBTree, and isn't used elsewhere.
		*/
	public:
		Node(const T& item);		
		virtual ~Node() = default;

		friend class RBTree<T>;

	protected:
	private:
		Node() = delete;
		enum class RBColor { Red, Black } color;
		T item;
		Node* right;
		Node* left;
		Node* parent;
	};

	Node* root;	
	void LeftRotate(Node* x);
	void RightRotate(Node* x);
	void InsertFixup(Node* z);
    void RemoveFixup(Node* x);

    // Iterator declarations
public:
    class ConstIterator // inorder iterator
    {
    public:
        ConstIterator();
        ConstIterator(Node* current);
        virtual ~ConstIterator();
        ConstIterator& operator++();
        bool operator!=(const ConstIterator& other) const;
        const T& operator*() const;

        friend class RBTree<T>; // For access to GetNode() member function below.

    protected:
    private:
        const Node* GetNode() const;
        Node* current; // cached copy of the current visitable element
    };
    ConstIterator begin() const; // inorder iterator is the default when doing range-based iteration of RBTree
    ConstIterator end() const;


    class ConstPostorder // adaptor for postorder iteration
    {
    public:
        ConstPostorder(const RBTree<T>& tree);
        virtual ~ConstPostorder();

        class Iterator
        {
        public:
            Iterator();
            Iterator(Node* current);
            virtual ~Iterator();
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            const T& operator*() const;

            friend class RBTree<T>; // For access to GetNode() member function below.

        protected:
        private:
            const Node* GetNode() const; // This is used for (eg) destruction of the tree.
            Node* current;
            Node* next; // So we can delete nodes during traversal, we first traverse via the next pointer and allow current to be mutated/deleted.
            bool downwardPhase; // Whether we're in the upward phase of tree traversal, or the downward phase.
        };
        Iterator begin() const;
        Iterator end() const;

    protected:
    private:
        ConstPostorder() = delete;
        const RBTree<T>& _tree;
    };
};


template<class T>
RBTree<T>::RBTree() 
    : root(nullptr) 
{}


template<class T>
RBTree<T>::~RBTree()
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
}


template<class T>
void RBTree<T>::Clear()
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
    root = nullptr;
}


template<class T>
RBTree<T>::RBTree(RBTree<T>&& other)
{
    root = other.root;
    other.root = nullptr;
}


template<class T>
bool RBTree<T>::Search(const T& item) const
{
    Node* current(root);
    while (current != nullptr)
    {
        if (current->item == item)
            return true;

        if (item < current->item)
            current = current->left;
        else
            current = current->right;
    }
	return false;
}


template<class T>
void RBTree<T>::Insert(const T& item)
{
	Node* current(root);
	Node* previous(nullptr);
	Node* node = new Node(item);
	while(current != nullptr)
	{
		previous = current;
		if(item < current->item)
			current = current->left;
		else if(item > current->item)
			current = current->right;			
        else
        {
            // They're equal.
            delete node;
            return;
        }
	}

	node->parent = previous;
	if(previous == nullptr)
		root = node;
	else if(item < previous->item)
		previous->left = node;
	else
		previous->right = node;

	InsertFixup(node);
}


template<class T>
void RBTree<T>::Remove(const T& item)
{
    Node* current(root);
    Node* balancePoint(nullptr);

    while (current != nullptr)
    {
        if (current->item == item)
            break;

        if (item < current->item)
            current = current->left;
        else
            current = current->right;
    }
    if (current == nullptr)
        return; // The tree doesn't contain the specified item.

    if (current->left == nullptr && current->right == nullptr)
    {
        // We're removing a bottom-most node.
        if (current == root)
            root = nullptr;
        else if (current->parent->left == current)
        {
            current->parent->left = nullptr;
        }
        else
        {
            current->parent->right = nullptr;
        }
    }
    else if (current->left != nullptr && current->right == nullptr)
    {
        // The current node has only one child, which is on the left.
        if (current == root)
        {
            root = current->left;
            current->left->parent = nullptr;
            balancePoint = root;
        }
        else if (current->parent->left == current)
        {
            current->parent->left = current->left;
            balancePoint = current->left;
            current->left->parent = current->parent;
        }
        else
        {
            current->parent->right = current->left;
            balancePoint = current->left;
            current->left->parent = current->parent;
        }
    }
    else if (current->left == nullptr && current->right != nullptr)
    {
        // The current node has only one child, which is on the right.
        if (current == root)
        {
            root = current->right;
            current->right->parent = nullptr;
            balancePoint = root;
        }
        else if (current->parent->left == current)
        {
            current->parent->left = current->right;
            balancePoint = current->right;
            current->right->parent = current->parent;
        }
        else
        {
            current->parent->right = current->right;
            balancePoint = current->right;
            current->right->parent = current->parent;
        }
    }
    else
    {
        // current has both left and right children
        Node* replacement = current->right;
        while (replacement->left != nullptr)   // traverse to the minimum value in the right subtree
            replacement = replacement->left;

        replacement->left = current->left;
        current->left->parent = replacement;
        typename Node::RBColor temp = replacement->color;
        replacement->color = current->color;
        current->color = temp;
        balancePoint = replacement->right;

        if (replacement->parent != current)
        {
            replacement->parent->left = replacement->right;
            replacement->right = current->right;
            current->right->parent = replacement;
        }

        if (current == root)
        {
            root = replacement;
            replacement->parent = nullptr;
        }
        else if (current->parent->left == current)
        {
            current->parent->left = replacement;
            replacement->parent = current->parent;
        }
        else
        {
            current->parent->right = replacement;
            replacement->parent = current->parent;
        }
    }

    if (balancePoint != nullptr && current->color == Node::RBColor::Black)
        RemoveFixup(balancePoint);

    delete current;
}


template<class T>
void RBTree<T>::InsertFixup(Node* z)
{
	while(z != root && z->parent->color == Node::RBColor::Red)
		/* Parameter z is set by the caller, and since Insert(const T& item)
		   is the only caller, we are guaranteed it is set. There is no
		   guarantee that z->parent is set (i.e. z is the root node), or
		   that z->parent->parent so we check it here. */
	{
		if(z->parent == z->parent->parent->left) // Is our parent on the left of its grandparent?
		{
			Node* y = z->parent->parent->right; // z->parent->parent->right is "uncle."
			if(y != nullptr && y->color == Node::RBColor::Red)
			{
				z->parent->color = Node::RBColor::Black;
				y->color = Node::RBColor::Black;
				z->parent->parent->color = Node::RBColor::Red;
				z = z->parent->parent;
			}
			else 
			{
				if(z == z->parent->right) 
				{
					z = z->parent;
					LeftRotate(z);
				}
				z->parent->color = Node::RBColor::Black;
				z->parent->parent->color = Node::RBColor::Red;
				RightRotate(z->parent->parent);				
			}
		}
		else
		{
			Node* y = z->parent->parent->left; // z->parent->parent->left is "uncle."
			if(y != nullptr && y->color == Node::RBColor::Red)
			{
				z->parent->color = Node::RBColor::Black;
				y->color = Node::RBColor::Black;
				z->parent->parent->color = Node::RBColor::Red;
				z = z->parent->parent;
			}
			else 
			{
				if(z == z->parent->left)
				{
					z = z->parent;
					RightRotate(z);
				}	
				z->parent->color = Node::RBColor::Black;
				z->parent->parent->color = Node::RBColor::Red;
				LeftRotate(z->parent->parent);				
			}			
		}
	}
	root->color = Node::RBColor::Black;
}


template<class T>
void RBTree<T>::RemoveFixup(Node* x)
{
    while (x != root && x->color == Node::RBColor::Black)
    {
        if (x == x->parent->left)
        {
            Node* y = x->parent->right;
            if (y->color == Node::RBColor::Red)
            {
                // Case 1
                y->color = Node::RBColor::Black;
                x->parent->color = Node::RBColor::Red;
                LeftRotate(x->parent);
                y = x->parent->right; // TODO: Confirm this is not a no-op because LeftRotate() changed the relationship of the nodes.
            }
            
            if ((y->left == nullptr || y->left->color == Node::RBColor::Black) &&
                (y->right == nullptr || y->right->color == Node::RBColor::Black))
            {
                // Case 2
                y->color = Node::RBColor::Red;
                x = x->parent;
            }
            else
            {
                if (y->right == nullptr || y->right->color == Node::RBColor::Black)
                {
                    // Case 3
                    if (y->left != nullptr)
                        y->left->color = Node::RBColor::Black;
                    y->color = Node::RBColor::Red;
                    RightRotate(y);
                    y = x->parent->right;  // TODO: Confirm this is not a no-op because RightRotate() changed the relationship of the nodes.
                }

                // Case 4
                y->color = x->parent->color;
                if(x->parent->parent != nullptr)
                    x->parent->parent->color = Node::RBColor::Black;
                y->right->color = Node::RBColor::Black;
                LeftRotate(x->parent);
                root = x;
            }
        }
        else
        {
            // left-right symmetry here
            Node* y = x->parent->left;
            if (y->color == Node::RBColor::Red)
            {
                // Case 1
                y->color = Node::RBColor::Black;
                x->parent->color = Node::RBColor::Red;
                RightRotate(x->parent);
                y = x->parent->left; // TODO: Confirm this is not a no-op because LeftRotate() changed the relationship of the nodes.
            }
            
            if ((y->right == nullptr || y->right->color == Node::RBColor::Black) &&
                (y->left == nullptr || y->left->color == Node::RBColor::Black))
            {
                // Case 2
                y->color = Node::RBColor::Red;
                x = x->parent;
            }
            else
            {
                if (y->left == nullptr || y->left->color == Node::RBColor::Black)
                {
                    // Case 3
                    if (y->right != nullptr)
                        y->right->color = Node::RBColor::Black;
                    y->color = Node::RBColor::Red;
                    LeftRotate(y);
                    y = x->parent->left;  // TODO: Confirm this is not a no-op because RightRotate() changed the relationship of the nodes.
                }

                // Case 4
                y->color = x->parent->color;
                if (x->parent->parent != nullptr)
                    x->parent->parent->color = Node::RBColor::Black;
                y->left->color = Node::RBColor::Black;
                RightRotate(x->parent);
                root = x;
            }
        }
    }
    x->color = Node::RBColor::Black;
}


/*
	x     left rotate ->     y
	 \                      /
	  y   <- right rotate  x
*/
template <class T>
void RBTree<T>::LeftRotate(Node* x)
{
	Node* y = x->right;
	x->right = y->left;
	if(y->left != nullptr)
		y->left->parent = x;
	y->parent = x->parent;
	if(x->parent == nullptr)
	{
		root = y;
	}
	else if(x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}


/*
	x     left rotate ->     y
	 \                      /
	  y   <- right rotate  x
*/
template <class T>
void RBTree<T>::RightRotate(Node* x)
{
	Node* y = x->left;
	x->left = y->right;
	if(y->right != nullptr)
		y->right->parent = x;
	y->parent = x->parent;
	if(x->parent == nullptr)
	{
		root = y;	
	}
	else if(x == x->parent->right)
	{
		x->parent->right = y;
	}
	else
	{
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}


template<class T>
RBTree<T>::Node::Node(const T& item)
    : color(RBColor::Red), item(item), right(nullptr), left(nullptr), parent(nullptr)
{}


template<class T>
bool RBTree<T>::IsValid() const
{
	/* 1. Every node has color red or black.
       2. The root is always black.
       3. Every leaf node is black (nullptr is treated as black).
       4. If a node is red, then both its children are black.
       5. For each node, all paths from the node to the descendant leaves contains
          the same number of black nodes.
    */

    // #1 is definitionally true.

    // Check #2, that root is black.
    if(root != nullptr && root->color != Node::RBColor::Black)
		return false;

	// #3 is definitionally true. The leaf nodes are nullptr's and cannot be checked, but are treated as being black.

	// Check #4, that all red nodes have black children.    
    for (ConstIterator itr = begin(); itr != end(); ++itr)
    {
        const Node* current = itr.GetNode();
        // Check whether the current color is red and also has any red children.
        if (current->color == Node::RBColor::Red &&
            ((current->left != nullptr && current->left->color == Node::RBColor::Red) ||
            (current->right != nullptr && current->right->color == Node::RBColor::Red)
                ))
        {
            return false;
        }
    }

    // Check #5, For each node, all paths from the node to the descendant leaves contains the same number of black nodes.
    int mainCount = -1;
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
    {
        const Node* current = itr.GetNode();
        if (current->left != nullptr || current->right != nullptr) // The below validation only occurs at leaf nodes (i.e. bottom-most nodes).
            continue;

        int blackNodeCount = 0;
        const Node* ancestor = current;
        while (ancestor != nullptr) // Ascend the tree counting black nodes.
        {
            if (ancestor->color == Node::RBColor::Black)
                blackNodeCount++;
            ancestor = ancestor->parent;
        }
        if (mainCount == -1)
            mainCount = blackNodeCount;
        else if (blackNodeCount != mainCount)
            return false;     // iteration stops
    }
	return true;
}


template<class T>
template<typename U>
RBTree<T> RBTree<T>::Intersect(const U& other) const
{
    ConstIterator left = begin();
    typename U::ConstIterator right = other.begin();
    RBTree<T> intersectionTree;
    while (left != end() && right != other.end())
    {
        if (*left == *right)
        {
            intersectionTree.Insert(*left);
            ++left;
            ++right;
        }
        else if (*left < *right)
            ++left;
        else
            ++right;
    }
    return intersectionTree;
}


template<class T>
RBTree<T>::ConstIterator::ConstIterator() : current(nullptr)
{}


template<class T>
RBTree<T>::ConstIterator::ConstIterator(Node* current_) 
    : current(current_)
{
    if (current_ == nullptr)
        return;

    while (current->left != nullptr)
        current = current->left;
}


template<class T>
RBTree<T>::ConstIterator::~ConstIterator()
{}


template<class T>
typename RBTree<T>::ConstIterator& RBTree<T>::ConstIterator::operator++()
{
    if (current->right != nullptr)
    {
        current = current->right;
        while (current->left != nullptr)
            current = current->left;
    }
    else
    {
        while (current->parent != nullptr && current->parent->right == current)
            current = current->parent;

        current = current->parent;
    }
        
    return *this;
}


template<class T>
bool RBTree<T>::ConstIterator::operator!=(const ConstIterator& other) const
{
    return current != other.current;
}


template<class T>
const T& RBTree<T>::ConstIterator::operator*() const
{
    return current->item;
}


template<class T>
const typename RBTree<T>::Node* RBTree<T>::ConstIterator::GetNode() const
{
    return current;
}


template<class T>
typename RBTree<T>::ConstIterator RBTree<T>::begin() const
{
    return ConstIterator(root);
}


template<class T>
typename RBTree<T>::ConstIterator RBTree<T>::end() const
{    
    return ConstIterator(nullptr);
}


template<class T>
RBTree<T>::ConstPostorder::ConstPostorder(const RBTree<T>& tree)
    : _tree(tree) 
{}

template<class T>
RBTree<T>::ConstPostorder::~ConstPostorder() {}


template<class T>
RBTree<T>::ConstPostorder::Iterator::Iterator()
    : current(nullptr), next(nullptr), downwardPhase(true)
{}


template<class T>
RBTree<T>::ConstPostorder::Iterator::~Iterator()
{}


template<class T>
RBTree<T>::ConstPostorder::Iterator::Iterator(Node* current_)
    : current(nullptr), next(current_), downwardPhase(true)
{
    if (current_ == nullptr)
        return;

    while (next->left != nullptr) // descend leftward as far as possible
        next = next->left;

    operator++();
}


template<class T>
typename RBTree<T>::ConstPostorder::Iterator& RBTree<T>::ConstPostorder::Iterator::operator++()
{
    if (next == nullptr)        
        current = nullptr; // We're at the end.

    while (next != nullptr)
    {
        if (downwardPhase)
        {
            if (next->right != nullptr)
            {
                next = next->right;
                while (next->left != nullptr)
                    next = next->left;
            }
            else
            {
                downwardPhase = false;
            }
        }
        else
        {
            if (next->parent != nullptr && next->parent->right == next)
            {
                // stay in the upward phase
            }
            else
            {
                downwardPhase = true;
            }

            // visit
            current = next;
            next = next->parent;
            break;
        }
    }
    return *this;
}


template<class T>
bool RBTree<T>::ConstPostorder::Iterator::operator!=(const Iterator& other) const
{
    return !(current == other.current && next == other.next && downwardPhase == other.downwardPhase);
}


template<class T>
const T& RBTree<T>::ConstPostorder::Iterator::operator*() const
{
    return current->item;
}


template<class T>
const typename RBTree<T>::Node* RBTree<T>::ConstPostorder::Iterator::GetNode() const
{
    return current;
}


template<class T>
typename RBTree<T>::ConstPostorder::Iterator RBTree<T>::ConstPostorder::begin() const
{
    return Iterator(_tree.root);
}


template<class T>
typename RBTree<T>::ConstPostorder::Iterator RBTree<T>::ConstPostorder::end() const
{
    return Iterator(nullptr);
}


#if 0
/* This is an archival reprsentation of the traversal algorithm. It is provided
   purely as a reference for comparison with the iterator implementations. The
   implementations should be functionally identical.

   Recursion and stack are not allowed. Recursion is forbidden to preclude
   the possibility of a stack smash, and the stack is forbidden for the sake
   of memory efficiency. */
template<class T>
template<typename FunctorA, typename FunctorB>
void RBTree<T>::ForEachNode(FunctorA sortOrderVisitor, FunctorB bottomUpVisitor) const
{
    Node* current = root;
    if (current == nullptr)
        return;

    while (current->left != nullptr) // descend leftward as far as possible
        current = current->left;

    while (current != nullptr)
    {
        if (!sortOrderVisitor(current))
            return;

        if (current->right != nullptr) // descend rightward one step
        {
            current = current->right;
            while (current->left != nullptr) // descend leftward as far as possible
                current = current->left;
        }
        else
        {
            // ascend, skipping any previously visited nodes
            //Node* bottomUpNode = current;

            while (current->parent != nullptr && current->parent->right == current)
            {
                if (!bottomUpVisitor(current))
                    return;
                current = current->parent;
                //if (!bottomUpVisitor(current))
                //    return;
                //bottomUpNode = current;
            }
            if (!bottomUpVisitor(current))
                return;

            current = current->parent;
            //if (!bottomUpVisitor(bottomUpNode))
            //    return;
        }
    }
}
#endif


template class RBTree<int>; // To force compilation of the template, for validation.

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2021 Bobby G. Burrough
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

#endif