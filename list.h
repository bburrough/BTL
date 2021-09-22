#ifndef _LIST_H_
#define _LIST_H_

/* List

   Bob Burrough, 2021

   Implementation of a singly-linked list. */


template<class T>
class List
{
public:
    List();
    List(std::initializer_list<T> l); // initialize List with a static array of values
    virtual ~List();
    List(const List& other); // copy constructor
    List(List<T>&& other); // move constructor
        // TODO: Currently violates rule of five. To satisfy rule of 5, need copy assignment and move assignment operators.

    // Insert an item at the front of the list. O(1)
    void Insert(const T& item);

    // Append an item to the end of the list. O(1)
    void Append(const T& item);

    // Reverses the list. O(n)
    void Reverse();

    // returns true if the list contains no elements
    bool IsEmpty() const;

    // returns the number of elements contained in the list
    size_t Size() const;

    // empties the list of all elements
    void Clear();

    /* Consistency check. Returns true if the tree
       is internally consistent. Otherwise, false. */
    bool IsValid() const;

protected:
private:
    class Node
    {
    public:
        Node(const T& item_);

        friend class List<T>;

    protected:
    private:
        Node() = delete; // An item is required to instantiate a node.
        Node* next;
        T item;
    };

    Node* head;
    Node* tail;
    size_t size;

    // Iterator declarations
public:
    class ConstIterator
    {
    public:
        ConstIterator();
        ConstIterator(Node* start);
        ConstIterator& operator++();
        bool operator==(const ConstIterator& other) const;
        bool operator!=(const ConstIterator& other) const;
        const T& operator*() const;

        friend class List<T>; // For access to GetNode() member function below.

    protected:
    private:
        const Node* GetNode() const;

        Node* current;
        Node* next; // This is necessary so we can delete nodes during iteration.
    };

    ConstIterator begin() const;
    ConstIterator end() const;
};


template<class T>
List<T>::List()
    : head(nullptr), tail(nullptr), size(0)
{}


template<class T>
List<T>::List(std::initializer_list<T> l)
    : head(nullptr), tail(nullptr), size(0)
{
    for(const auto& x : l)
        Append(x);
}


template<class T>
List<T>::~List()
{
    for (ConstIterator itr = begin(); itr != end(); ++itr)
        delete itr.GetNode();
}


template<class T>
void List<T>::Clear()
{
    for (ConstIterator itr = begin(); itr != end(); ++itr)
        delete itr.GetNode();
    head = nullptr;
    tail = nullptr;
    size = 0;
}


template<class T>
List<T>::List(const List<T>& other) // copy constructor
    : head(nullptr), tail(nullptr), size(other.size)
{
    Node* otherCurrent = other.head;
    Node* previous(nullptr);
    Node* node(nullptr);
    while (otherCurrent != nullptr)
    {
        node = new Node(otherCurrent->item);
        if (head == nullptr)
            head = node;
        if (previous != nullptr)
            previous->next = node;
        previous = node;
        otherCurrent = otherCurrent->next;
        if (otherCurrent == nullptr)
            tail = node;
    }
}


template<class T>
List<T>::List(List<T>&& other) // move constructor
    : head(other.head), tail(other.tail), size(other.size)
{
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
}


template<class T>
void List<T>::Insert(const T& item)
{
    Node* current = new Node(item);
    current->next = head;
    head = current;
    if (tail == nullptr)
        tail = current;
    size++;
}


template<class T>
void List<T>::Append(const T& item)
{
    Node* current = new Node(item);
    if (head == nullptr)
        head = current;
    if (tail != nullptr)
        tail->next = current;
    tail = current;
    size++;
}


template<class T>
void List<T>::Reverse()
{
    Node* current = head;
    Node* previous = nullptr;
    Node* next = nullptr;
    while (current != nullptr)
    {        
        next = current->next;
        current->next = previous;

        previous = current;
        current = next;
    }
    Node* temp = head;
    head = tail;
    tail = temp;
}


template<class T>
bool List<T>::IsEmpty() const
{
    return size == 0;
}


template<class T>
size_t List<T>::Size() const
{
    return size;
}


template<class T>
List<T>::Node::Node(const T& item_)
    : next(nullptr), item(item_)
{}


template<class T>
List<T>::ConstIterator::ConstIterator() // This also happens to be equivalent to List<T>::end()
    : current(nullptr), next(nullptr)
{}


template<class T>
List<T>::ConstIterator::ConstIterator(Node* start)
    : current(start), next(nullptr)
{
    if (current != nullptr && current->next != nullptr)
        next = current->next;
}


template<class T>
typename List<T>::ConstIterator& List<T>::ConstIterator::operator++()
{
    current = next;
    if (current != nullptr)
        next = current->next;
    else
        next = nullptr;
    return *this;
}


template<class T>
bool List<T>::ConstIterator::operator==(const List<T>::ConstIterator& other) const
{
    return (current == other.current && next == other.next);
}


template<class T>
bool List<T>::ConstIterator::operator!=(const List<T>::ConstIterator& other) const
{
    return !(current == other.current && next == other.next);
}


template<class T>
const T& List<T>::ConstIterator::operator*() const
{
    return current->item;
}


template<class T>
const typename List<T>::Node* List<T>::ConstIterator::GetNode() const
{
    return current;
}


template<class T>
typename List<T>::ConstIterator List<T>::begin() const
{
    return ConstIterator(head);
}


template<class T>
typename List<T>::ConstIterator List<T>::end() const
{
    return ConstIterator(nullptr);
}


template<class T>
bool List<T>::IsValid() const
{
    if (head != nullptr && tail == nullptr)
        return false;
    if (head == nullptr && tail != nullptr)
        return false;

    size_t elementCount(0);
    Node* current = head;
    while (current != nullptr)
    {
        elementCount++;
        if (current->next == nullptr && tail != current)
            return false;
        current = current->next;
    }
    if (Size() != elementCount)
        return false;

    return true;
}


template class List<int>; // To force compilation of the template, for compile-time validation.

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