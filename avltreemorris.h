#ifndef _AVLTREE_MORRIS_H_
#define _AVLTREE_MORRIS_H_

/*  AVL tree, Morris traversal edition

    Bob Burrough, 2021

    Height balanced binary search tree. Provides O(log N) insertion,
    search, and delete. Does not use recursion and does not allocate
    memory during iteration. The type stored in the tree must have a
    meaningful operator<() to facilitate balancing of the tree.

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

#if 0

// This is disabled because this implementation is incomplete.

template<class T>
class AVLTreeMorris
{
public:
    AVLTreeMorris();
    ~AVLTreeMorris();
    AVLTreeMorris(AVLTreeMorris<T>&& other); // move constructor
        /* TODO: AVLTree currently violates rule of 5. It has a custom destructor and move constructor, but does
           not implement copy, copy-assignment, or move-assignment.
           https://en.cppreference.com/w/cpp/language/rule_of_three#:~:text=Rule%20of%20five */

    // Place an item in the tree. Complexity is O(log N).
    void Insert(const T& item);

    // Remove item from the tree. Complexity is O(log N).
    void Remove(const T& item);

    // Retrieve item from the tree. Complexity is O(log N).
    bool Search(const T& item) const;

    void Clear();

    template<typename U>
    AVLTreeMorris<T> Intersect(const U& other) const;

    /* Consistency check. Returns true if the tree
       is internally consistent. Otherwise, false. */
    bool IsValid() const;

protected:
private:
    class Node
    {
    public:
        Node(const T& item);
        virtual ~Node() = default;
        friend class AVLTreeMorris<T>;

    protected:
    private:
        Node() = delete; // item must be provided
        T item;
        int balanceFactor;
        Node* left;
        Node* right;

        Node* RightRotate(); 
        Node* LeftRotate(); 
        Node* DoubleRightRotate();
        Node* DoubleLeftRotate(); 
        Node* Balance(); // Rebalances the node such that the balanceFactor becomes -1, 0, or +1.
        unsigned int CalculateHeight(); // For validation only.

        // These are provided simply to avoid including additional headers.
        template<class U> static const U& max(const U& a, const U& b) { return a < b ? b : a; }
        template<class U> static const U& min(const U& a, const U& b) { return a < b ? a : b; }
    };


    Node* root;

    
    // Iterator declarations
public:
    class ConstIterator  // inorder iterator
    {
    public:
        ConstIterator(const AVLTreeMorris<T>& tree_);
        ConstIterator(const AVLTreeMorris<T>& tree_, bool end); // This constructor creates an empty iterator which is equal to end().
        virtual ~ConstIterator();
        /* TODO: ConstIterator currently violates rule of 5 becaues it has custom destructor but
           no custom copy, copy-assignment, or move-assignment operators. */

        ConstIterator& operator++();
        bool operator!=(const ConstIterator& other) const;
        const T& operator*() const;

        friend class AVLTreeMorris<T>; // For access to GetNode() member function below.

    protected:
    private:
        ConstIterator() = delete;
        void Reset(); // causes traversal to unwind back to beginning state
        const Node* GetNode() const;
        const AVLTreeMorris<T>& tree;
        Node* current;
        Node* previous;
        unsigned int traversalPointerCount;
        bool abort;
        bool continuation;
    };    
    ConstIterator begin() const; 
    ConstIterator end() const;


    class ConstPostorder // adaptor for postorder iteration
    {
    public:
        ConstPostorder(const AVLTreeMorris<T>& tree);
        virtual ~ConstPostorder();

        class Iterator
        {
        public:
            Iterator(const AVLTreeMorris& tree_);
            Iterator(const AVLTreeMorris& tree_, bool end);
            virtual ~Iterator();
            /* TODO: ConstIterator currently violates rule of 5 becaues it has custom destructor but
               no custom copy, move, copy-assignment, or move-assignment operators. */

            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            const T& operator*() const;

            friend class AVLTreeMorris<T>; // For access to GetNode() member function below.

        protected:
        private:
            Iterator() = delete;
            void Reset();
            const Node* GetNode() const; // This is used for (eg) destruction of the tree.
            const AVLTreeMorris<T>& tree;
            Node* current;
            Node* previous;
            unsigned int traversalPointerCount;
            bool abort;
            bool continuation;
            Node fakeRoot;
            Node* first;
            Node* middle;
            Node* last;
            Node* capture;
        };
        Iterator begin() const;
        Iterator end() const;

    protected:
    private:
        ConstPostorder() = delete;
        const AVLTreeMorris<T>& tree;
    };
};


template<class T>
AVLTreeMorris<T>::AVLTreeMorris()
    : root(nullptr)
{}


template<class T>
AVLTreeMorris<T>::~AVLTreeMorris()
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
}


template<class T>
void AVLTreeMorris<T>::Clear()
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
    root = nullptr;
}


template<class T>
AVLTreeMorris<T>::AVLTreeMorris(AVLTreeMorris&& other)
{
    root = other.root;
    other.root = nullptr;
}


template<class T>
void AVLTreeMorris<T>::Insert(const T& item)
{
    //defaultIterator.Reset(); // make sure the iterator doesn't have any traversal pointers outstanding
    // TODO: defaultIterator has been eliminated. We still should be able to use some sort of a flag to ensure that no traversal pointers remain outstanding.
    Node* node = new Node(item);
    Node* current(root);
    Node* previous(nullptr);
    Node* balancePoint(nullptr);
    Node* balancePointPredecessor(nullptr);
    Node* balanceFactorUpdateHead(root);
    if (current == nullptr)
    {
        root = node;
        return;
    }

    while (1) // TODO: This is ugly.
    {
        if (item < current->item)
        {
            if(current->balanceFactor > 0)
                balanceFactorUpdateHead = current;

            if (current->left == nullptr)
            {
                current->left = node;
                break;
            }
            else
            {
                /* We're stepping down the tree. If the current node is perfectly balanced,
                   then no rotation is required, because it will still be +/- 1 levels.

                   If we step down a shorter path, no balance is required. Nothing we can do
                   will unbalance the tree. We can forget about any previously tracked
                   balancePoint.

                   If we step down a longer path, then rotation might be needed. Also, we
                   can forget all prior nodes. */

                if (current->balanceFactor < 0)
                {
                    // We're going down the longer side.
                    balancePointPredecessor = previous;
                    balancePoint = current;
                }
                else if (current->balanceFactor > 0)
                {
                    // We're going down the shorter side.
                    balancePointPredecessor = nullptr;
                    balancePoint = nullptr;
                }
                previous = current;
                current = current->left;
            }
        }
        else if (item > current->item)
        {
            if (current->balanceFactor < 0)
                balanceFactorUpdateHead = current;

            if (current->right == nullptr)
            {
                current->right = node;
                break;
            }
            else
            {
                if (current->balanceFactor > 0)
                {
                    // We're going down the longer side.
                    balancePointPredecessor = previous;
                    balancePoint = current;
                }
                else if (current->balanceFactor < 0)
                {
                    // We're going down the shorter side.
                    balancePointPredecessor = nullptr;
                    balancePoint = nullptr;
                }
                previous = current;
                current = current->right;
            }
        }
        else
        {
            // They're equal.
            balancePoint = nullptr;
            balanceFactorUpdateHead = nullptr;
            delete node;
            return;
        }
    }

    /* If balancePoint is set, we do not want to update balanceFactor's
       *above* that node, because we would have to pull those updates
       back out after Balance() takes place. */
    if (balancePoint != nullptr)
        balanceFactorUpdateHead = balancePoint;

    if (balanceFactorUpdateHead != nullptr)
    {
        // Update balance factors.
        do
        {
            if (item < balanceFactorUpdateHead->item)
            {
                balanceFactorUpdateHead->balanceFactor--;
                balanceFactorUpdateHead = balanceFactorUpdateHead->left;
            }
            else
            {
                balanceFactorUpdateHead->balanceFactor++;
                balanceFactorUpdateHead = balanceFactorUpdateHead->right;
            }
        } while (balanceFactorUpdateHead != node);
    }

    if (balancePoint != nullptr) // Balance factors are correct at this point.
    {
        Node* substituteNode = balancePoint->Balance(); // However, balancing is messing them up again.
        if (root == balancePoint)
            root = substituteNode;
        if (balancePointPredecessor != nullptr)
        {
            if (balancePointPredecessor->left == balancePoint)
                balancePointPredecessor->left = substituteNode;
            else
                balancePointPredecessor->right = substituteNode;
        }
    }
}
 

/* A precondition for Balance is that the balanceFactor of this node
   and its immediate descendants must be accurate (obviously). Also,
   this node must have a balanceFactor of 2 or -2. */
template<class T>
typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::Node::Balance()
{
    Node *w(nullptr), *p(nullptr);
    if (balanceFactor == -2) 
    {
        w = left;
        if (w->balanceFactor == 1)
            p = DoubleRightRotate();
        else
            p = RightRotate();
    }
    else if (balanceFactor == 2)
    {
        w = right;
        if (w->balanceFactor == -1)
            p = DoubleLeftRotate();
        else
            p = LeftRotate();
    }
    return p;
}


template<class T>
void AVLTreeMorris<T>::Remove(const T& item)
{
#if 1
    Node* current(root);
    Node* balancePoint(nullptr);
    Node* previous(nullptr);
    Node* balanceUpdateHead(root);

    while (current != nullptr)
    {
        if (current->item == item)
            break;

        if (current->balanceFactor == 0) // shortening of the below here will not cause shortening above here
            balanceUpdateHead = current;

        previous = current;
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
        else if (previous->left == current)
        {
            balancePoint = previous;
            previous->balanceFactor++;
            previous->left = nullptr;
        }
        else
        {
            balancePoint = previous;
            previous->balanceFactor--;
            previous->right = nullptr;
        }
        if (previous != nullptr && previous->balanceFactor == 0)
        {         
            while (balanceUpdateHead != previous)
            {
                if (item < balanceUpdateHead->item)
                {
                    balanceUpdateHead->balanceFactor++;
                    balanceUpdateHead = balanceUpdateHead->left;
                }
                else
                {
                    balanceUpdateHead->balanceFactor--;
                    balanceUpdateHead = balanceUpdateHead->right;
                }
            }
#if 0
            // current's tree is shorter
            Node* predecessor = previous;
            do
            {
                /* Because we can't access parent here, we will have to find a balanceUpdateHead
                   as we traverse down through the tree (above).  In the original version,
                   as we travel up through the tree, we increment the balanceFactor if predecessor
                   is a left sibling, and decrement the balanceFactor if predecessor is a right
                   sibling. If the resulting increment is 1 for increment, or -1 for decrement,
                   then we're done.
                   
                   Therefore, as we descend the tree if the item is in the right, and we assume
                   the right tree will be shorter, we can determine that the current balanceFactor
                   will become -1, thereby overriding any previous balanceUpdateHead we've seen. */
                if (predecessor->parent == nullptr)
                    break;

                if (predecessor->parent->left == predecessor)
                {
                    predecessor->parent->balanceFactor++;
                    if (predecessor->parent->balanceFactor == 1)
                        break;
                }
                else
                {
                    predecessor->parent->balanceFactor--;
                    if (predecessor->parent->balanceFactor == -1)
                        break;
                }
                predecessor = predecessor->parent;
            } while (1);
#endif
        }
    }
    else if (current->left != nullptr && current->right == nullptr)
    {
        // The current node has only one child, which is on the left.
        if (current == root)
        {
            root = current->left;
        }
        else if (previous->left == current)
        {
            previous->left = current->left;
            balancePoint = previous;
            previous->balanceFactor++;
        }
        else
        {
            previous->right = current->left;
            balancePoint = previous;
            previous->balanceFactor--;
        }
        if (previous != nullptr && previous->balanceFactor == 0)
        {
            while (balanceUpdateHead != previous)
            {
                if (item < balanceUpdateHead->item)
                {
                    balanceUpdateHead->balanceFactor++;
                    balanceUpdateHead = balanceUpdateHead->left;
                }
                else
                {
                    balanceUpdateHead->balanceFactor--;
                    balanceUpdateHead = balanceUpdateHead->right;
                }
            }
#if 0
            // current's tree is shorter
            Node* predecessor = previous;
            do
            {
                if (predecessor->parent == nullptr)
                    break;

                if (predecessor->parent->left == predecessor)
                {
                    predecessor->parent->balanceFactor++;
                    if (predecessor->parent->balanceFactor == 1)
                        break;
                }
                else
                {
                    predecessor->parent->balanceFactor--;
                    if (predecessor->parent->balanceFactor == -1)
                        break;
                }
                predecessor = predecessor->parent;
            } while (1);
#endif
        }
    }
    else if (current->left == nullptr && current->right != nullptr)
    {
        // The current node has only one child, which is on the right.
        if (current == root)
        {
            root = current->right;
        }
        else if (previous->left == current)
        {
            previous->left = current->right;
            balancePoint = previous;
            previous->balanceFactor++;
        }
        else
        {
            previous->right = current->right;
            balancePoint = previous;
            previous->balanceFactor--;
        }
        if (previous != nullptr && previous->balanceFactor == 0)
        {
            while (balanceUpdateHead != previous)
            {
                if (item < balanceUpdateHead->item)
                {
                    balanceUpdateHead->balanceFactor++;
                    balanceUpdateHead = balanceUpdateHead->left;
                }
                else
                {
                    balanceUpdateHead->balanceFactor--;
                    balanceUpdateHead = balanceUpdateHead->right;
                }
            }
#if 0
            // current's tree is shorter
            Node* predecessor = previous;
            do
            {
                if (predecessor->parent == nullptr)
                    break;

                if (predecessor->parent->left == predecessor)
                {
                    predecessor->parent->balanceFactor++;
                    if (predecessor->parent->balanceFactor == 1)
                        break;
                }
                else
                {
                    predecessor->parent->balanceFactor--;
                    if (predecessor->parent->balanceFactor == -1)
                        break;
                }
                predecessor = predecessor->parent;
            } while (1);
#endif
        }
    }
    else
    {
        // current has both left and right children
        Node* replacement = current->right;
        Node* replacementPrevious = current;

        bool replacementIncludesBottomMost = true;
        // At this point, it is definitionally true that replacement includes bottom most.
        while (replacement->left != nullptr)   // traverse to the minimum value in the right subtree
        {
            // We're going downward to the left.
            if (replacement->balanceFactor >= 0)
                replacementIncludesBottomMost = false;

            replacementPrevious = replacement;
            replacement = replacement->left;
        }

        if (replacementIncludesBottomMost)
        {
            current->balanceFactor--;
            /* At this point, this shortening could theoretically
               cascade all the way up the tree to the root.
               I suspect this is also true during the balancing
               procedure below.

               Is it definitionally true that if current's balanceFactor
               has changed, that every balanceFactor above me should
               also change? Answer: no. Because if current->balanceFactor
               became -1, then current's height will not have changed.
               However, we can be certain that if current's balanceFactor
               became zero, that current's height has changed.

               Actually...I suspect this should happen at replacement instead
               of at current. */

            if (current->balanceFactor >= 0)
            {
                while (balanceUpdateHead != current)
                {
                    if (item < balanceUpdateHead->item)
                    {
                        balanceUpdateHead->balanceFactor++;
                        balanceUpdateHead = balanceUpdateHead->left;
                    }
                    else
                    {
                        balanceUpdateHead->balanceFactor--;
                        balanceUpdateHead = balanceUpdateHead->right;
                    }
                }
#if 0
                // current's tree is shorter
                Node* predecessor = current;
                do
                {
                    if (predecessor->parent == nullptr)
                        break;

                    if (predecessor->parent->left == predecessor)
                    {
                        predecessor->parent->balanceFactor++;
                        if (predecessor->parent->balanceFactor == 1)
                            break;
                    }
                    else
                    {
                        predecessor->parent->balanceFactor--;
                        if (predecessor->parent->balanceFactor == -1)
                            break;
                    }
                    predecessor = predecessor->parent;
                } while (1);
#endif
            }
        }

        replacement->left = current->left;
        replacement->balanceFactor = current->balanceFactor;

        if(replacementPrevious != current)
        {
            replacementPrevious->left = replacement->right;
            replacementPrevious->balanceFactor++;
            balancePoint = replacementPrevious;
            replacement->right = current->right;
        }

        if (current == root)
        {
            root = replacement;
        }
        else if (previous->left == current)
        {
            previous->left = replacement;
        }
        else
        {
            previous->right = replacement;
        }
    }

    delete current;

    while (balancePoint != nullptr)
    {
        if (balancePoint->balanceFactor == -2 || balancePoint->balanceFactor == 2)
        {
            Node* balancePointPredecessor = balancePoint->parent;

            Node* substituteNode = balancePoint->Balance();
            if (root == balancePoint)
                root = substituteNode;

            if (balancePointPredecessor != nullptr)
            {
                substituteNode->parent = balancePointPredecessor;
                if (balancePointPredecessor->left == balancePoint)
                {
                    balancePointPredecessor->left = substituteNode;
                    balancePointPredecessor->balanceFactor++;
                }
                else
                {
                    balancePointPredecessor->right = substituteNode;
                    balancePointPredecessor->balanceFactor--;
                }

                if (balancePointPredecessor->balanceFactor == 0)
                {
                    // current's tree is shorter
                    Node* predecessor = balancePointPredecessor;
                    do
                    {
                        if (predecessor->parent == nullptr)
                            break;

                        if (predecessor->parent->left == predecessor)
                        {
                            predecessor->parent->balanceFactor++;
                            if (predecessor->parent->balanceFactor == 1)
                                break;
                        }
                        else
                        {
                            predecessor->parent->balanceFactor--;
                            if (predecessor->parent->balanceFactor == -1)
                                break;
                        }
                        predecessor = predecessor->parent;
                    } while (1);
                }
            }

            balancePoint = balancePointPredecessor;
        }
        else
            break;
    }

#endif
}




template<class T>
bool AVLTreeMorris<T>::Search(const T& item) const
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
AVLTreeMorris<T>::Node::Node(const T& item)
    : item(item), balanceFactor(0), left(nullptr), right(nullptr)
{}


#if 0
/* 
   This is kept here for archival purposes only. It is an implementation
   of an inorder traversal algorithm which has since been replaced by an
   iterator implementation. The iterator should be functionally identical
   to this.

   This uses Morris Traversal. Morris Traversal modifies the nodes (therefore
   is somewhat risky), but recursion and stacks are worse. I don't believe
   there's a method that traverses the tree without modifying the elements and
   without growing memory (either on the heap or on the stack). Therefore, we
   use the memory allocated for the nodes to facilitate traversal (it stores a
   traversal pointer in unused leaf pointers). This is conceptually similar to
   how aligned malloc works (i.e. hiding data in the structure itself). This
   code is *not* reentrant. Do not invoke it in a nested fashion.

   The provided functor must return true to continue iteration, or false to
   stop iteration. Because this uses Morris Traversal, iteration is not
   guaranteed to stop immediately. Iteration will continue until all of the
   traversal pointers have been set back to their original, null values. */
template<class T>
template<typename Functor>
void AVLTree<T>::ForEachNode(Functor orderedVisitor) const
{
    AVLTree<T>::Node* current(root);
    AVLTree<T>::Node* previous(nullptr);
    bool abort(false);
    unsigned int traversalPointerCount(0);

    while (current != nullptr)
    {
        if (current->left == nullptr)
        {
            if (!abort)
                abort = !orderedVisitor(current);
            if (abort && traversalPointerCount == 0)
                break;

            current = current->right;
        }
        else
        {
            previous = current->left;
            while (previous->right != nullptr && previous->right != current)
                previous = previous->right;

            if (previous->right == nullptr && !abort)
            {
                previous->right = current;
                traversalPointerCount++;
                current = current->left;
            }
            else
            {
                if (!abort)
                    abort = !orderedVisitor(current);

                if (previous->right != nullptr) // If previous->right is already null, it's because we're aborting.
                {
                    previous->right = nullptr;
                    traversalPointerCount--;
                }
                if (abort && traversalPointerCount == 0)
                    break;
                current = current->right;
            }
        }
    }
}


template<class T>
template<typename Functor>
void AVLTree<T>::Node::ForEachNodePostorder(Functor bottomUpVisitor)
{
    unsigned int traversalPointerCount(0);
    bool abort(false);
    Node dummy({});
    dummy.left = this;
    AVLTree<T>::Node* current(&dummy);
    AVLTree<T>::Node* previous(nullptr);

    while (current != nullptr)
    {
        if (current->left == nullptr)
        {
            current = current->right;
        }
        else
        {
            previous = current->left;
            while (previous->right != nullptr && previous->right != current)
                previous = previous->right;

            if (previous->right == nullptr)
            {
                previous->right = current;
                traversalPointerCount++;
                current = current->left;
            }
            else
            {
                // This section is the key difference for post order Morris traversal.

                /* This fails during abort because it's expecting previous->right to represent
                   one end of the traversal, but we have preempted the setting of previous->right
                   due to the abort condition. 
                   
                   middle->right eventually arrives */
                if (!abort)
                {
                    AVLTree<T>::Node* first(current);
                    AVLTree<T>::Node* middle(current->left);
                    AVLTree<T>::Node* last(nullptr);

                    while (middle != current)
                    {
                        last = middle->right;
                        middle->right = first;
                        first = middle;
                        middle = last;
                    }

                    Node* capture(nullptr);
                    first = current;
                    middle = previous;
                    while (middle != current)
                    {
                        capture = middle;

                        last = middle->right;
                        middle->right = first;
                        if (middle->right == current) // Workaround to make inorder traversal safe from within postorder. This is duplicative of previous->right = nullptr below.
                            middle->right = nullptr;
                        first = middle;
                        middle = last;

                        if(!abort)
                            abort = !bottomUpVisitor(capture);
                    }
                }
                previous->right = nullptr;
                traversalPointerCount--;
                if (abort && traversalPointerCount == 0)
                    return;
                current = current->right;
            }
        }
    }
}
#endif


/*  CalculateHeight()

    Returns the maximum number of edges from this node to all descendant
    nodes.

    Uses Morris Traversal. See above comment about Morris Traversal. Also,
    since this is Node member function, it cannot be const because it's
    modifying the nodes.  To run this while iterating a tree, use postorder
    iteration.
*/
template<class T>
unsigned int AVLTreeMorris<T>::Node::CalculateHeight()
{
    Node* current(this);
    Node* previous(nullptr);
    int maxHeight(-1);

    int currentHeight = -1;
    while (current != nullptr)
    {
        if (current->left == nullptr)
        {
            // visit
            currentHeight++;
            if (currentHeight > maxHeight)
                maxHeight = currentHeight;

            current = current->right;
        }
        else
        {
            previous = current->left; // this descends one level
            int interimCount = 1; // count one for the left-step
            while (previous->right != nullptr && previous->right != current)
            {
                previous = previous->right;
                interimCount++; // and one for each right-step
            }

            if (previous->right == nullptr)
            {
                previous->right = current;
                current = current->left;
                currentHeight++;
            }
            else
            {
                previous->right = nullptr;

                // At this point, previous is definitely a bottom-up node, which also means its balance is zero.

                // visit
                currentHeight -= interimCount;
                if (currentHeight > maxHeight)
                    maxHeight = currentHeight;

                current = current->right;
            }
        }
    }
    return maxHeight;
}



template<class T>
typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::Node::RightRotate()
{
    Node* q(left);
    if (q == nullptr) // Shouldn't happen, but we guard against it here anyway.
        return nullptr;     
    this->left = q->right;
    q->right = this;
    int newBalanceThis = balanceFactor + 1 - min(q->balanceFactor, 0);
    int newBalanceQ = q->balanceFactor + 1 + max(newBalanceThis, 0);
    this->balanceFactor = newBalanceThis;
    q->balanceFactor = newBalanceQ;
    return q;
}


template<class T>
typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::Node::LeftRotate()
{
    Node* q(right);
    if (q == nullptr) // Shouldn't happen, but we guard against it here anyway.
        return nullptr;
    this->right = q->left;
    q->left = this;
    int newBalanceThis = balanceFactor - 1 - max(q->balanceFactor, 0);
    int newBalanceQ = q->balanceFactor - 1 + min(newBalanceThis, 0);
    this->balanceFactor = newBalanceThis;
    q->balanceFactor = newBalanceQ;
    return q;
}


template<class T>
typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::Node::DoubleRightRotate()
{
    if (left == nullptr)  // Shouldn't happen, but we guard against it here anyway.
        return nullptr;
    left = left->LeftRotate();
    return RightRotate();
}


template<class T>
typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::Node::DoubleLeftRotate()
{
    if (right == nullptr)
        return nullptr;
    right = right->RightRotate();
    return LeftRotate();
}


template<class T>
bool AVLTreeMorris<T>::IsValid() const
{
    /* - Balance factors of all nodes are -1, 0, or 1.
       - Verify balance factors by calculating heights at all nodes.
    */
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
    {
        const Node* n = itr.GetNode();
        if (n->balanceFactor != -1 && n->balanceFactor != 0 && n->balanceFactor != 1)
            return false;

        unsigned int leftHeight = n->left != nullptr ? (1 + n->left->CalculateHeight()) : 0;
        unsigned int rightHeight = n->right != nullptr ? (1 + n->right->CalculateHeight()) : 0;
        int calculatedBalanceFactor = rightHeight - leftHeight;
        if (n->balanceFactor != calculatedBalanceFactor)
            return false;
    }
    return true;
}


template<class T>
AVLTreeMorris<T>::ConstIterator::ConstIterator(const AVLTreeMorris<T>& tree_)
    : tree(tree_), current(tree_.root), previous(nullptr), traversalPointerCount(0), abort(false), continuation(false)
{
    if (tree_.root == nullptr)
        return;

    operator++();
}


template<class T>
AVLTreeMorris<T>::ConstIterator::ConstIterator(const AVLTreeMorris<T>& tree_, bool end)
    : tree(tree_), current(nullptr), previous(nullptr), traversalPointerCount(0), abort(false), continuation(false)
{
    end = true; // TODO: Reconsider the constructors for end() iterators.
}


template<class T>
AVLTreeMorris<T>::ConstIterator::~ConstIterator()
{
    Reset();
}


template<class T>
typename AVLTreeMorris<T>::ConstIterator& AVLTreeMorris<T>::ConstIterator::operator++()
{
    while (current != nullptr)
    {
        if (current->left == nullptr)
        {
            if (!abort && !continuation)
            {
                continuation = true;
                break;
            }
            continuation = false;

            if (abort && traversalPointerCount == 0)
                break;

            current = current->right;
        }
        else
        {
            previous = current->left;
            while (previous->right != nullptr && previous->right != current)
                previous = previous->right;

            if (previous->right == nullptr && !abort)
            {
                previous->right = current;
                traversalPointerCount++;
                current = current->left;
            }
            else
            {
                if (!abort && !continuation)
                {
                    continuation = true;
                    break;
                }
                continuation = false;


                if (previous->right != nullptr) // If previous->right is already null, it's because we're aborting.
                {
                    previous->right = nullptr;
                    traversalPointerCount--;
                }
                if (abort && traversalPointerCount == 0)
                    break;
                current = current->right;
            }
        }
    }
    if ((abort && traversalPointerCount == 0) || current == nullptr)
    {
        // Need to be equal to end().
        current = nullptr;
        previous = nullptr;
        traversalPointerCount = 0;
        abort = false;
        continuation = false;
    }

    return *this;
}


template<class T>
bool AVLTreeMorris<T>::ConstIterator::operator!=(const ConstIterator& other) const
{
    return !(current == other.current &&
        previous == other.previous &&
        traversalPointerCount == other.traversalPointerCount &&
        abort == other.abort &&
        continuation == other.continuation);
}


template<class T>
const T& AVLTreeMorris<T>::ConstIterator::operator*() const
{
    return current->item;
}


template<class T>
const typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::ConstIterator::GetNode() const
{
    return current;
}


template<class T>
void AVLTreeMorris<T>::ConstIterator::Reset()
{
    if (traversalPointerCount > 0)
    {
        abort = true;
        operator++();
    }
    continuation = false;
    current = tree.root;
}


template<class T>
typename AVLTreeMorris<T>::ConstIterator AVLTreeMorris<T>::begin() const
{    
    return ConstIterator(*this);
}


template<class T>
typename AVLTreeMorris<T>::ConstIterator AVLTreeMorris<T>::end() const
{
    return ConstIterator(*this, true);
}



template<class T>
AVLTreeMorris<T>::ConstPostorder::ConstPostorder(const AVLTreeMorris<T>& tree_)
    : tree(tree_)
{}


template<class T>
AVLTreeMorris<T>::ConstPostorder::~ConstPostorder() {}


template<class T>
AVLTreeMorris<T>::ConstPostorder::Iterator::~Iterator()
{
    Reset();
}


template<class T>
AVLTreeMorris<T>::ConstPostorder::Iterator::Iterator(const AVLTreeMorris& tree_)
    : tree(tree_), current(nullptr), previous(nullptr), traversalPointerCount(0), abort(false), continuation(false), fakeRoot({}), first(nullptr), middle(nullptr), last(nullptr), capture(nullptr)
{
    current = &fakeRoot;
    fakeRoot.left = tree.root;
    operator++();
}


template<class T>
AVLTreeMorris<T>::ConstPostorder::Iterator::Iterator(const AVLTreeMorris& tree_, bool end)
    : tree(tree_), current(nullptr), previous(nullptr), traversalPointerCount(0), abort(false), continuation(false), fakeRoot({}), first(nullptr), middle(nullptr), last(nullptr), capture(nullptr)
{
    end = true; // TODO: Reconsider the constructors for end() iterators.
}


#if 0
template<class T>
AVLTree<T>::ConstPostorder::Iterator::Iterator(Iterator&& other)
    : tree(other.tree), current(other.current), previous(other.previous), traversalPointerCount(other.traversalPointerCount), abort(other.abort), continuation(other.continuation), dummy(other.dummy), capture(other.capture)
{
    /* TODO: ConstPostorder::Iterator violates rule of 5. It implements a move constructor,
       but doesn't need a custom destructor because there's no dynamically allocated memory
       within the class. Does it really need a move constructor then?
       https://en.cppreference.com/w/cpp/language/rule_of_three#:~:text=Rule%20of%20five */
    other.dummy.left = nullptr;
    other.current = nullptr;
    other.previous = nullptr;
    other.traversalPointerCount = 0;
    other.abort = false;
    other.continuation = false;
    other.capture = nullptr;
}
#endif

template<class T>
typename AVLTreeMorris<T>::ConstPostorder::Iterator& AVLTreeMorris<T>::ConstPostorder::Iterator::operator++()
{
    int test = 1;
    test++;
    while (current != nullptr)
    {
        if (current->left == nullptr)
        {
            current = current->right;
        }
        else
        {
            if (!continuation)
            {
                previous = current->left;
                while (previous->right != nullptr && previous->right != current)
                    previous = previous->right;
            }

            if (!continuation && previous->right == nullptr)
            {
                previous->right = current;
                traversalPointerCount++;
                current = current->left;
            }
            else
            {
                /* This section is the key difference for post order Morris traversal. It treats this
                   branch as a linked list, reverses it, and then visits each node, un-reversing it
                   after visitation has occured. */
                if (!abort || continuation)
                {
                    if (!continuation)
                    {
                        first = current;
                        middle = current->left;
                        last = nullptr;

                        while (middle != current)
                        {
                            last = middle->right;
                            middle->right = first;
                            first = middle;
                            middle = last;
                        }

                        first = current;
                        middle = previous;
                    }

                    bool breakout = false;
                    while (middle != current)
                    {
                        capture = middle;

                        if (capture == previous)
                        {
                            int test = 0; // When this is true during destruction, previous->right = nullptr; below is accessing deleted memory.
                            test++;
                            previous = nullptr;
                        }

                        last = middle->right;
                        middle->right = first;
                        if (middle->right == current) // Workaround to make inorder traversal safe from within postorder. This is duplicative of previous->right = nullptr below.
                            middle->right = nullptr;
                        first = middle;
                        middle = last;

                        if (!abort)
                        {
                            breakout = true;
                            continuation = true;
                            break;
                        }
                    }
                    if (breakout) // break occurred
                        break;

                    continuation = false;
                }

                if(previous != nullptr)
                    previous->right = nullptr;
                traversalPointerCount--;
                if (abort && traversalPointerCount == 0)
                    break;
                current = current->right;
            }
        }
    }
    if ((abort && traversalPointerCount == 0) || current == nullptr)
    {
        // Need to be equal to end().
        current = nullptr;
        previous = nullptr;
        traversalPointerCount = 0;
        abort = false;
        continuation = false;
        capture = nullptr;
        fakeRoot.left = nullptr;
    }
    return *this;
}


template<class T>
void AVLTreeMorris<T>::ConstPostorder::Iterator::Reset()
{
    if (traversalPointerCount > 0)
    {
        abort = true;
        operator++();
    }
    continuation = false;
    current = tree.root;
    fakeRoot.left = tree.root;
}


template<class T>
bool AVLTreeMorris<T>::ConstPostorder::Iterator::operator!=(const Iterator& other) const
{
    return !(current == other.current && 
        previous == other.previous && 
        abort == other.abort && 
        continuation == other.continuation && 
        traversalPointerCount == other.traversalPointerCount && 
        fakeRoot.left == other.fakeRoot.left && 
        capture == other.capture);
}


template<class T>
const T& AVLTreeMorris<T>::ConstPostorder::Iterator::operator*() const
{
    return capture->item;
}


template<class T>
const typename AVLTreeMorris<T>::Node* AVLTreeMorris<T>::ConstPostorder::Iterator::GetNode() const
{
    return capture;
}


template<class T>
typename AVLTreeMorris<T>::ConstPostorder::Iterator AVLTreeMorris<T>::ConstPostorder::begin() const
{
    return Iterator(tree);
}


template<class T>
typename AVLTreeMorris<T>::ConstPostorder::Iterator AVLTreeMorris<T>::ConstPostorder::end() const
{
    return Iterator(tree, true);
}


template<typename T>
template<typename U>
AVLTreeMorris<T> AVLTreeMorris<T>::Intersect(const U& other) const
{
    ConstIterator left = begin();
    typename U::ConstIterator right = other.begin();
    AVLTreeMorris<T> intersectionTree;
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
/* There's a design problem with the Intersect() method. Currently it accepts
   type U, which we assume is another tree type (either AVLTree or RBTree).
   However, a more flexible design would accept intersections with other
   container types such as array, vector, or list.  The way this algorithm
   is written, it assumes that the containers are sorted. In the case of a
   tree, this is true. However, if this code were to be given an array
   or list, that might not be true. */


template class AVLTreeMorris<int>; // To force compilation of the template, for compile-time validation.

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

#endif