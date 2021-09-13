#ifndef _AVLTREE_H_
#define _AVLTREE_H_


/*  AVL tree

    Bob Burrough, 2021

    Single-header implementation of height-balanced binary search tree. Provides
    O(log N) insertion, search, and delete. Does not use recursion and does not
    allocate extra memory during iteration. The type stored in the tree must have
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
class AVLTree
{
public:
    AVLTree();
    virtual ~AVLTree(); // custom destructor (rule of 5)
    //AVLTree(const AVLTree<T>& other); // copy constructor (rule of 5)
    AVLTree(AVLTree<T>&& other); // move constructor (rule of 5)
    //AVLTree<T>& operator=(const AVLTree<T>& other); // copy assignment operator (rule of 5)
    //AVLTree<T>& operator=(AVLTree<T>&& other); // move assignment operator (rule of 5)

        /* TODO: AVLTree currently violates rule of 5. It has a custom destructor and move constructor, but does
           not implement copy, copy-assignment, or move-assignment.
           https://en.cppreference.com/w/cpp/language/rule_of_three#:~:text=Rule%20of%20five */

    // Place an item in the tree. Complexity is O(log N).
    void Insert(const T& item);

    // Remove item from the tree. Complexity is O(log N).
    void Remove(const T& item);

    // Retrieve item from the tree. Complexity os O(log N).
    bool Search(const T& item) const;

    void Clear();

    template<typename U>
    AVLTree<T> Intersect(const U& other) const;

    /* Consistency check. Returns true if the tree
       is internally consistent. Otherwise, false. */
    bool IsValid() const;

protected:
private:
    class Node
    {
    public:
        Node(const T& item_);
        virtual ~Node() = default;

        friend class AVLTree<T>;

    protected:
    private:
        Node() = delete; // item must be provided
        T item;
        int balanceFactor;
        Node* left;
        Node* right;
        Node* parent;

        Node* RightRotate(); 
        Node* LeftRotate(); 
        Node* DoubleRightRotate();
        Node* DoubleLeftRotate(); 
        Node* Balance(); // Rebalances the node such that the balanceFactor becomes -1, 0, or +1.
        unsigned int CalculateHeight() const; // For validation only.

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
        ConstIterator(const AVLTree<T>& tree_);
        ConstIterator(const AVLTree<T>& tree_, bool end); // This constructor creates an empty iterator which is equal to end().
        virtual ~ConstIterator();
        /* TODO: ConstIterator currently violates rule of 5 becaues it has custom destructor but
           no custom copy, copy-assignment, or move-assignment operators. */

        ConstIterator& operator++();
        bool operator!=(const ConstIterator& other) const;
        const T& operator*() const;

        friend class AVLTree<T>; // For access to GetNode() member function below.

    protected:
    private:
        ConstIterator() = delete;
        const Node* GetNode() const;
        const AVLTree<T>& tree;
        Node* current;
    };    
    ConstIterator begin() const; 
    ConstIterator end() const;


    class ConstPostorder // adaptor for postorder iteration
    {
    public:
        ConstPostorder(const AVLTree<T>& tree);
        virtual ~ConstPostorder();

        class Iterator
        {
        public:
            Iterator(const AVLTree& tree_);
            Iterator(const AVLTree& tree_, bool end);
            virtual ~Iterator();
            /* TODO: ConstIterator currently violates rule of 5 becaues it has custom destructor but
               no custom copy, move, copy-assignment, or move-assignment operators. */

            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            const T& operator*() const;

            friend class AVLTree<T>; // For access to GetNode() member function below.

        protected:
        private:
            Iterator() = delete;
            const Node* GetNode() const; // This is used for (eg) destruction of the tree.
            const AVLTree<T>& tree;
            Node* current;
            Node* next;
            bool downwardPhase;
        };
        Iterator begin() const;
        Iterator end() const;

    protected:
    private:
        ConstPostorder() = delete;
        const AVLTree<T>& tree;
    };
};



template<class T>
AVLTree<T>::AVLTree()
    : root(nullptr)
{}


template<class T>
AVLTree<T>::~AVLTree() // custom destructor (rule of 5)
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
}


template<class T>
void AVLTree<T>::Clear()
{
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
    root = nullptr;
}


template<class T>
AVLTree<T>::AVLTree(AVLTree&& other) // move constructor (rule of 5)
{
    root = other.root;
    other.root = nullptr;
}


#if 0
template<class T>
AVLTree<T>::AVLTree(const AVLTree<T>& other) // copy constructor (rule of 5)
{}





template<class T>
AVLTree<T>& AVLTree<T>::operator=(const AVLTree<T>& other) // copy assignment operator (rule of 5)
{
    // Deconstruct any existing parts of the tree.
    for (typename ConstPostorder::Iterator itr = ConstPostorder(*this).begin(); itr != ConstPostorder(*this).end(); ++itr)
        delete itr.GetNode();
    
    // Construct new elements which are copies of the elements in other.
    // Don't call Insert(). Insert does way more than is necessary. We just need to duplicate the structure
    // that exists in other.
    Node* current = other.root;
    while (current != nullptr)
    {
        Node* node = new Node(current->item);
        if (current == other.root)
            root = node;


    }
} 

template<class T>
AVLTree<T>& AVLTree<T>::operator=(AVLTree<T>&& other) // move assignment operator (rule of 5)
{
    // Deconstruct any existing parts of the three.
    // Steal other's parts.
}
#endif


template<class T>
void AVLTree<T>::Insert(const T& item)
{
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
            if (current->balanceFactor > 0)
            {
                balanceFactorUpdateHead = current;
                if (balancePoint != nullptr)
                    balancePoint = current;
            }

            if (current->left == nullptr)
            {
                current->left = node;
                node->parent = current;
                break;
            }
            else
            {
            /*  We're stepping down the tree. If the current node is perfectly balanced,
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
            {
                balanceFactorUpdateHead = current;
                if(balancePoint != nullptr)
                    balancePoint = current;
            }

            if (current->right == nullptr)
            {
                current->right = node;
                node->parent = current;
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
            break;
        }
    }

    /* If balancePoint is set, we do not want to update balanceFactor's
       *above* that node, because we would have to pull those updates
       back out after Balance() takes place. */
    if (balancePoint != nullptr)
        balanceFactorUpdateHead = balancePoint;
        /* This is incorrectly pulling the balanceFactorUpdateHead *upward*.
           However, I believe the problem is that balancePoint should *not*
           be 0.1f. In fact, there should be no balancePoint at all. */
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

    if (balancePoint != nullptr) 
    {
        Node* substituteNode = balancePoint->Balance(); 
        if (root == balancePoint)
            root = substituteNode;            

        if (balancePointPredecessor != nullptr)
        {
            substituteNode->parent = balancePointPredecessor;
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
typename AVLTree<T>::Node* AVLTree<T>::Node::Balance()
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


/* For validation only. Do not use for any other purpose.
   Calculates the height of a tree by walking all descendant
   nodes. */
template<class T>
unsigned int AVLTree<T>::Node::CalculateHeight() const
{
    const Node* current = this;
    unsigned int currentHeight = 0;
    unsigned int maxHeight = 0;

    while (current->left != nullptr) // descend leftward as far as possible
    {
        currentHeight++;
        current = current->left;
    }

    while (current != nullptr && current != parent)
    {
        if (currentHeight > maxHeight)
            maxHeight = currentHeight;

        if (current->right != nullptr) // descend rightward one step
        {
            current = current->right;
            currentHeight++;

            while (current->left != nullptr) // descend leftward as far as possible
            {
                current = current->left;
                currentHeight++;
            }
        }
        else
        {
            // ascend, skipping any previously visited nodes
            while (current->parent != nullptr && current->parent->right == current && current != this)
            {
                current = current->parent;
                currentHeight--;
            }

            current = current->parent;
            currentHeight--;
        }
    }
    return maxHeight;
}


template<class T>
void AVLTree<T>::Remove(const T& item)
{
    Node* current(root);
    Node* balancePoint(nullptr);
    Node* balanceUpdateHead(root);

    while (current != nullptr)
    {
        if (current->item == item)
            break;

        if (current->balanceFactor == 0) // shortening of the below here will not cause shortening above here
            balanceUpdateHead = current;

        if (item < current->item)
        {

            current = current->left;
        }
        else
        {
            current = current->right;
        }
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
            balancePoint = current->parent;
            current->parent->balanceFactor++;
            current->parent->left = nullptr;
        }
        else
        {
            balancePoint = current->parent;
            current->parent->balanceFactor--;
            current->parent->right = nullptr;
        }
        if (current->parent != nullptr && current->parent->balanceFactor == 0)
        {
            // current's tree is shorter
            Node* predecessor = current->parent;
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
    else if (current->left != nullptr && current->right == nullptr)
    {
        // The current node has only one child, which is on the left.
        if (current == root)
        {
            root = current->left;
            current->left->parent = nullptr;
        }
        else if (current->parent->left == current)
        {
            current->parent->left = current->left;
            balancePoint = current->parent;
            current->parent->balanceFactor++;
            current->left->parent = current->parent;
        }
        else
        {
            current->parent->right = current->left;
            balancePoint = current->parent;
            current->parent->balanceFactor--;
            current->left->parent = current->parent;
        }
        if (current->parent != nullptr && current->parent->balanceFactor == 0)
        {
            // current's tree is shorter
            Node* predecessor = current->parent;
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
    else if (current->left == nullptr && current->right != nullptr)
    {
        // The current node has only one child, which is on the right.
        if (current == root)
        {
            root = current->right;
            current->right->parent = nullptr;
        }
        else if (current->parent->left == current)
        {
            current->parent->left = current->right;
            balancePoint = current->parent;
            current->parent->balanceFactor++;
            current->right->parent = current->parent;
        }
        else
        {
            current->parent->right = current->right;
            balancePoint = current->parent;
            current->parent->balanceFactor--;
            current->right->parent = current->parent;
        }
        if (current->parent != nullptr && current->parent->balanceFactor == 0)
        {
            // current's tree is shorter
            Node* predecessor = current->parent;
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
    else
    {
        // current has both left and right children
        Node* replacement = current->right;
        bool replacementIncludesBottomMost = true;
        // At this point, it is definitionally true that replacement includes bottom most.
        while (replacement->left != nullptr)   // traverse to the minimum value in the right subtree
        {
            // We're going downward to the left.
            if (replacement->balanceFactor >= 0)
                replacementIncludesBottomMost = false;

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
            }
        }

        replacement->left = current->left;
        current->left->parent = replacement;
        replacement->balanceFactor = current->balanceFactor;
        balancePoint = replacement;

        if (replacement->parent != current)
        {
            replacement->parent->left = replacement->right;
            replacement->parent->balanceFactor++;
            balancePoint = replacement->parent;
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
}


template<class T>
bool AVLTree<T>::Search(const T& item) const
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
AVLTree<T>::Node::Node(const T& item_)
    : item(item_), balanceFactor(0), left(nullptr), right(nullptr), parent(nullptr)
{}


template<class T>
typename AVLTree<T>::Node* AVLTree<T>::Node::RightRotate()
{
    Node* q(left);
    if (q == nullptr) // Shouldn't happen, but we guard against it here anyway.
        return nullptr;     
    this->left = q->right;
    if(q->right != nullptr)
        q->right->parent = this;
    q->right = this;
    q->parent = this->parent;
    parent = q;
    int newBalanceThis = balanceFactor + 1 - min(q->balanceFactor, 0);
    int newBalanceQ = q->balanceFactor + 1 + max(newBalanceThis, 0);
    this->balanceFactor = newBalanceThis;
    q->balanceFactor = newBalanceQ;
    return q;
}


template<class T>
typename AVLTree<T>::Node* AVLTree<T>::Node::LeftRotate()
{
    Node* q(right);
    if (q == nullptr) // Shouldn't happen, but we guard against it here anyway.
        return nullptr;
    this->right = q->left;
    if(q->left != nullptr)
        q->left->parent = this;
    q->left = this;
    q->parent = this->parent;
    parent = q;
    int newBalanceThis = balanceFactor - 1 - max(q->balanceFactor, 0);
    int newBalanceQ = q->balanceFactor - 1 + min(newBalanceThis, 0);
    this->balanceFactor = newBalanceThis;
    q->balanceFactor = newBalanceQ;
    return q;
}


template<class T>
typename AVLTree<T>::Node* AVLTree<T>::Node::DoubleRightRotate()
{
    if (left == nullptr)  // Shouldn't happen, but we guard against it here anyway.
        return nullptr;
    left = left->LeftRotate();
    return RightRotate();
}


template<class T>
typename AVLTree<T>::Node* AVLTree<T>::Node::DoubleLeftRotate()
{
    if (right == nullptr)
        return nullptr;
    right = right->RightRotate();
    return LeftRotate();
}


template<class T>
bool AVLTree<T>::IsValid() const
{
    /* - Balance factors of all nodes are -1, 0, or 1.
       - Verify balance factors by calculating heights at all nodes.
    */
    for (ConstIterator itr = begin(); itr != end(); ++itr)
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
AVLTree<T>::ConstIterator::ConstIterator(const AVLTree<T>& tree_)
    : tree(tree_), current(tree_.root)
{
    if (tree_.root == nullptr)
        return;

    while (current->left != nullptr)
        current = current->left;
}


template<class T>
AVLTree<T>::ConstIterator::ConstIterator(const AVLTree<T>& tree_, bool end)
    : tree(tree_), current(nullptr)
{
    end = true; // TODO: Reconsider the constructors for end() iterators.
}


template<class T>
AVLTree<T>::ConstIterator::~ConstIterator()
{}


template<class T>
typename AVLTree<T>::ConstIterator& AVLTree<T>::ConstIterator::operator++()
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
bool AVLTree<T>::ConstIterator::operator!=(const ConstIterator& other) const
{
    return current != other.current;
}


template<class T>
const T& AVLTree<T>::ConstIterator::operator*() const
{
    return current->item;
}


template<class T>
const typename AVLTree<T>::Node* AVLTree<T>::ConstIterator::GetNode() const
{
    return current;
}


template<class T>
typename AVLTree<T>::ConstIterator AVLTree<T>::begin() const
{    
    return ConstIterator(*this);
}


template<class T>
typename AVLTree<T>::ConstIterator AVLTree<T>::end() const
{
    return ConstIterator(*this, true);
}



template<class T>
AVLTree<T>::ConstPostorder::ConstPostorder(const AVLTree<T>& tree_)
    : tree(tree_)
{}


template<class T>
AVLTree<T>::ConstPostorder::~ConstPostorder() {}


template<class T>
AVLTree<T>::ConstPostorder::Iterator::~Iterator()
{}


template<class T>
AVLTree<T>::ConstPostorder::Iterator::Iterator(const AVLTree& tree_)
    : tree(tree_), current(nullptr), next(tree_.root), downwardPhase(true)
{
    if (next == nullptr)
        return;

    while (next->left != nullptr) // descend leftward as far as possible
        next = next->left;

    operator++();
}


template<class T>
AVLTree<T>::ConstPostorder::Iterator::Iterator(const AVLTree& tree_, bool end)
    : tree(tree_), current(nullptr), next(nullptr), downwardPhase(true)
{
    end = true; // TODO: Reconsider the constructors for end() iterators.
}


template<class T>
typename AVLTree<T>::ConstPostorder::Iterator& AVLTree<T>::ConstPostorder::Iterator::operator++()
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
bool AVLTree<T>::ConstPostorder::Iterator::operator!=(const Iterator& other) const
{
    return !(current == other.current && next == other.next && downwardPhase == other.downwardPhase);
}


template<class T>
const T& AVLTree<T>::ConstPostorder::Iterator::operator*() const
{
    return current->item;
}


template<class T>
const typename AVLTree<T>::Node* AVLTree<T>::ConstPostorder::Iterator::GetNode() const
{
    return current;
}


template<class T>
typename AVLTree<T>::ConstPostorder::Iterator AVLTree<T>::ConstPostorder::begin() const
{
    return Iterator(tree);
}


template<class T>
typename AVLTree<T>::ConstPostorder::Iterator AVLTree<T>::ConstPostorder::end() const
{
    return Iterator(tree, true);
}


template<typename T>
template<typename U>
AVLTree<T> AVLTree<T>::Intersect(const U& other) const
{
    ConstIterator left = begin();
    typename U::ConstIterator right = other.begin();
    AVLTree<T> intersectionTree;
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


template class AVLTree<int>; // To force compilation of the template, for compile-time validation.

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