
#include <iostream>
#include <string>

using namespace std;

#include "rbtree.h"
#include "avltree.h"
#include "avltreemorris.h"
#include "list.h"
#include "pair.h"


bool SequencesMatch(const List<int>& lhs, const List<int>& rhs)
{
    if (lhs.Size() != rhs.Size())
        return false;

    List<int>::ConstIterator lhs_itr = lhs.begin();
    List<int>::ConstIterator rhs_itr = rhs.begin();
    for (; lhs_itr != lhs.end() && rhs_itr != rhs.end();)
    {
        if (*lhs_itr != *rhs_itr)
            return false;

        ++lhs_itr;
        ++rhs_itr;
    }

    return true;
}


#define VALUES { 2, 13, 10, 5, 12, 7, 17, 18, 37, 29, 11, 14, 15, 16 }
#define REVERSE_VALUES { 16, 15, 14, 11, 29, 37, 18, 17, 7, 12, 5, 10, 13, 2 }
#define SORTED_VALUES { 2, 5, 7, 10, 11, 12, 13, 14, 15, 16, 17, 18, 29, 37 }
#define EMPTY_VALUES {}

template<typename T>
void IntegerListTest()
{
    T integerList(VALUES);
    List<int> resultantSequence;
    for (const int& x : integerList)
        resultantSequence.Append(x);
    cout << (integerList.IsValid() && SequencesMatch(resultantSequence, VALUES) ? "passed" : "failed") << "...initializer_list test " << endl;
    resultantSequence.Clear();

    integerList.Clear();
    for (const int& x : integerList)
        resultantSequence.Append(x);
    cout << (integerList.IsValid() && SequencesMatch(resultantSequence, EMPTY_VALUES) ? "passed" : "failed") << "...clear test " << endl;
    resultantSequence.Clear();

    for (const int& x : VALUES)
        integerList.Insert(x); // Insert causes the order of VALUES to be reversed.
    for (const int& x : integerList)
        resultantSequence.Append(x);
    cout << (integerList.IsValid() && SequencesMatch(resultantSequence, REVERSE_VALUES) ? "passed" : "failed") << "...insert test " << endl;

    resultantSequence.Reverse();
    cout << (integerList.IsValid() && SequencesMatch(resultantSequence, VALUES) ? "passed" : "failed") << "...reversal test " << endl;
    resultantSequence.Clear();

    integerList.Clear();
    for (const int& x : VALUES)
        integerList.Append(x);
    for (const int& x : integerList)
        resultantSequence.Append(x);
    cout << (integerList.IsValid() && SequencesMatch(resultantSequence, VALUES) ? "passed" : "failed") << "...append test " << endl;
    resultantSequence.Clear();
}


template<typename T>
void IntegerTreeTest()
{
    T integerTree;
    const int values[] = VALUES;
    List<int> resultantSequence;
    for (const int &x : values)
        integerTree.Insert(x);

    for (typename T::ConstIterator itr = integerTree.begin(); itr != integerTree.end(); ++itr)
        resultantSequence.Append(*itr);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, SORTED_VALUES) ? "passed" : "failed") << "...insert test " << endl;
    resultantSequence.Clear();

    integerTree.Insert(2);
    integerTree.Insert(2);
    integerTree.Insert(2);
    for (typename T::ConstIterator itr = integerTree.begin(); itr != integerTree.end(); ++itr)
        resultantSequence.Append(*itr);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, SORTED_VALUES) ? "passed" : "failed") << "...duplicate insertion test " << endl;
    resultantSequence.Clear();

    for (typename T::ConstIterator itr = integerTree.begin(); itr != integerTree.end(); ++itr)
    {
        if (*itr == 5)
            break;  // intentionally stop in the middle

        resultantSequence.Append(*itr);
    }
    cout <<  (integerTree.IsValid() && SequencesMatch(resultantSequence, { 2 }) ? "passed" : "failed") << "...interrupted iteration test " << endl;
    resultantSequence.Clear();

    // At this point, there should be no traversal pointers in the tree.

    integerTree.Insert(4);
    integerTree.Insert(6);
    integerTree.Insert(42);

    for (typename T::ConstIterator itr = integerTree.begin(); itr != integerTree.end(); ++itr)
        resultantSequence.Append(*itr);
    cout <<  (integerTree.IsValid() && SequencesMatch(resultantSequence, { 2,4,5,6,7,10,11,12,13,14,15,16,17,18,29,37,42 }) ? "passed" : "failed") << "...insert after interruption test " << endl;
    resultantSequence.Clear();

    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 2,4,5,6,7,10,11,12,13,14,15,16,17,18,29,37,42 }) ? "passed" : "failed") << "...range-based iteration test " << endl;
    resultantSequence.Clear();

    for (typename T::ConstPostorder::Iterator itr = typename T::ConstPostorder(integerTree).begin(); itr != typename T::ConstPostorder(integerTree).end(); ++itr)
        resultantSequence.Append(*itr);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 4,2,6,7,5,11,12,10,14,16,17,15,29,42,37,18,13 }) ? "passed" : "failed") << "...first postorder iteration test " << endl;
    resultantSequence.Clear();

    for (auto &x : typename T::ConstPostorder(integerTree))
        resultantSequence.Append(x);
    cout <<  (integerTree.IsValid() && SequencesMatch(resultantSequence, { 4,2,6,7,5,11,12,10,14,16,17,15,29,42,37,18,13 }) ? "passed" : "failed") << "...second postorder iteration test " << endl;
    resultantSequence.Clear();

    for (auto &x : typename T::ConstPostorder(integerTree))
    {
        if (x == 37)
            break;

        resultantSequence.Append(x);
    }
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 4,2,6,7,5,11,12,10,14,16,17,15,29,42 }) ? "passed" : "failed") << "...interrupted postorder iteration test " << endl;
    resultantSequence.Clear();

    for (auto &x : typename T::ConstPostorder(integerTree))
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 4,2,6,7,5,11,12,10,14,16,17,15,29,42,37,18,13 }) ? "passed" : "failed") << "...iteration after postorder iteration test " << endl;
    resultantSequence.Clear();

    cout << (integerTree.IsValid() && integerTree.Search(29) ? "passed" : "failed") << "...search found test" << endl;

    cout << (integerTree.IsValid() && integerTree.Search(30) == false ? "passed" : "failed") << "...search not found test" << endl;

    T secondTree;
    int alternateValues[] = { -1, 800, 12, 10, 3, 42, 7, 16, 15 }; // -1, 800, 3 do not intersect. 12, 10, 42, 7, 16, 15 do intersect.
    for (auto &x : alternateValues)
        secondTree.Insert(x);

    T intersectionTree = integerTree.Intersect(secondTree);
    for (auto &x : intersectionTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && secondTree.IsValid() && intersectionTree.IsValid() && SequencesMatch(resultantSequence, { 7, 10, 12, 15, 16, 42 }) ? "passed" : "failed") << "...intersection test" << endl;
    resultantSequence.Clear();

    AVLTree<float> floatTree;
    float floatValues[] = { 0.1f, 3.14159f, -1.0f, 2.71828f, 7.0f, 10.0f, 12.0f };
    for (auto &x : floatValues)
    {
        floatTree.Insert(x);
        if (!floatTree.IsValid())
            cout << "not valid" << endl;
    }
    
    T intersectionTree2 = integerTree.Intersect(floatTree);
    for (auto &x : intersectionTree2)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && floatTree.IsValid() && intersectionTree2.IsValid() && SequencesMatch(resultantSequence, { 7, 10, 12 }) ? "passed" : "failed") << "...float intersection test" << endl;
    resultantSequence.Clear();

    // removal tests
    integerTree.Clear();
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, EMPTY_VALUES) ? "passed" : "failed") << "...clear all items" << endl;
    resultantSequence.Clear();

    integerTree.Insert(1);
    integerTree.Remove(0); // TC1 - attempt to remove nonexistent item. Nothing should happen.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 1 }) ? "passed" : "failed") << "...remove nonexistent item" << endl;
    resultantSequence.Clear();

    integerTree.Remove(1); // TC2 - remove root node which has no children. Result should be empty tree.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, EMPTY_VALUES) ? "passed" : "failed") << "...remove root with no children" << endl;
    resultantSequence.Clear();

    integerTree.Insert(1); // 1 is root
    integerTree.Insert(2); // 2 is root's right child
    integerTree.Remove(1); // TC3 - remove root node which has a right child. Result should be item 2 is root.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 2 }) ? "passed" : "failed") << "...remove root with right child" << endl;
    resultantSequence.Clear();

    integerTree.Insert(1); // 1 is now root's left child. 2 is root.
    integerTree.Remove(2); // TC4 - remove root node which has a left child. Result should be item 1 is root.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 1 }) ? "passed" : "failed") << "...remove root with left child" << endl;
    resultantSequence.Clear();

    integerTree.Insert(0);
    integerTree.Insert(2);
    integerTree.Remove(1); // TC5 - remove root node which has two children. Result should be 2 is now root.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 0, 2 }) ? "passed" : "failed") << "...remove root with two children" << endl;
    resultantSequence.Clear();

    integerTree.Insert(5);
    integerTree.Remove(5); // TC6 - remove non-root node with no children
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 0, 2 }) ? "passed" : "failed") << "...remove non-root with no children" << endl;
    resultantSequence.Clear();

    integerTree.Insert(5);
    integerTree.Insert(4);
    integerTree.Remove(5); // TC7 - remove non-root node with a left child
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 0, 2, 4 }) ? "passed" : "failed") << "...remove non-root with left child" << endl;
    resultantSequence.Clear();

    integerTree.Insert(5);
    integerTree.Remove(4); // TC8 - remove non-root node with a right child.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 0, 2, 5 }) ? "passed" : "failed") << "...remove non-root with right child" << endl;
    resultantSequence.Clear();

    integerTree.Insert(4);
    integerTree.Insert(6);
    integerTree.Remove(5); // TC9 - remove non-root node which has two children.
    for (auto &x : integerTree)
        resultantSequence.Append(x);
    cout << (integerTree.IsValid() && SequencesMatch(resultantSequence, { 0, 2, 4, 6 }) ? "passed" : "failed") << "...remove non-root with two children" << endl;
    resultantSequence.Clear();

    // Forward removal iteration test
    bool removalIterationsPassed = true;
    const int numRemovalIterations = 1000;
    for (int i = 0; i < numRemovalIterations; i++)
    {
        integerTree.Clear();
        List<int> expectedResult;
        for (int j = 0; j < numRemovalIterations; j++)
        {
            integerTree.Insert(j); // put all values in integerTree.
            if (j == i)
                continue;
            expectedResult.Append(j); // put all values but i in expectedResult
        }
        integerTree.Remove(i);
        resultantSequence.Clear();
        for (auto &x : integerTree)
            resultantSequence.Append(x);
        removalIterationsPassed &= integerTree.IsValid() && SequencesMatch(resultantSequence, expectedResult);
        if (!removalIterationsPassed)
        {
            cout << "failed...remove " << i << " from " << numRemovalIterations << " element tree test" << endl;
            break;
        }
    }
    if(removalIterationsPassed)
        cout << "passed...remove n from " << numRemovalIterations << " element tree test" << endl;

    // Backward removal iteration test
    removalIterationsPassed = true;
    for (int i = 0; i < numRemovalIterations; i++)
    {
        integerTree.Clear();
        List<int> expectedResult;
        for (int j = 0; j < numRemovalIterations; j++)
        {
            int value = numRemovalIterations - (j + 1);

            integerTree.Insert(value); // put all values in integerTree.
            if (value == i)
                continue;
            expectedResult.Insert(value); // put all values but i in expectedResult
        }
        integerTree.Remove(i);
        resultantSequence.Clear();
        for (auto &x : integerTree)
            resultantSequence.Append(x);
        removalIterationsPassed &= integerTree.IsValid() && SequencesMatch(resultantSequence, expectedResult);
        if (!removalIterationsPassed)
        {
            cout << "failed...remove " << i << " from " << numRemovalIterations << " element tree (reverse insertion) test" << endl;
            break;
        }
    }
    if (removalIterationsPassed)
        cout << "passed...remove n from " << numRemovalIterations << " element tree (reverse insertion) test" << endl;

}


int main()
{
    cout << "\n\nTesting AVLTree<int>...\n\n";
    IntegerTreeTest<AVLTree<int>>();
    cout << "\n\nTesting RBTree<int>...\n\n";
    IntegerTreeTest<RBTree<int>>();

    // These are disabled becuase AVLTreeMorris<T> implementation is incomplete.
    //cout << "\n\nTesting AVLTreeMorris<int>...\n\n";
    //IntegerTreeTest<AVLTreeMorris<int>>();

    cout << "\n\nTesting List<int>...\n\n";
    IntegerListTest<List<int>>();


    cout << "\n\nTestling List<string>...\n\n";
    List<string> names;
    names.Insert("Charlie");
    names.Insert("Bob");
    names.Insert("Aaron");
    names.Append("Xavier");
    names.Append("Yolanda");
    names.Append("Zoe");

    cout << "forward...\n";
    for (const string& name : names)
        cout << "name: " << name << "\n";

    cout << "\nreverse...\n";
    names.Reverse();
    for (const string& name : names)
        cout << "name: " << name << "\n";

    return 0;
} 

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