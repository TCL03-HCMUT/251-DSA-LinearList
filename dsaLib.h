#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <cstdlib>
using namespace std;

// TODO: Make a list

template <class T>
class LinearList
{ // abstract list design
public:
    virtual ~LinearList() {};
    virtual bool isEmpty() = 0;
    virtual int getSize() = 0;
    virtual void clear() = 0;
    virtual LinearList<T> *clone(int sidx = 0, int eidx = -1) = 0; // cannot return an object, end = -1 means copy to the end

    virtual T &operator[](int) = 0;
    // retrieve an element using the index,
    // should have a precondition before using or check and throw an exception
    // or you can be careful and exsure that you receive the correct index

    virtual bool insert(const T &, int) = 0; // precondition: index must be valid
    virtual bool append(const T &) = 0;
    virtual bool insert(T &&, int) = 0; // move an entire object
    virtual bool append(T &&) = 0;

    virtual bool removeIdx(int) = 0; // remove at an index
    virtual bool remove(const T &key, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; }) = 0; // remove an item by a comparison to a key

    virtual T *find(const T &key, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                    { return val == key; }) = 0;

    virtual int findIdx(const T &key, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; }) = 0;

    virtual LinearList<void *> *findAll(const T &key, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                                        { return val == key; }) = 0; // questionable design, may cause errors

    virtual void swap(int, int) = 0;
    virtual void reverse() = 0;
    virtual LinearList<T> *split(int) = 0; // split from some index, the returned list will be the second one while the current list will be reduced to the first
    virtual void merge(LinearList<T> *) = 0;

    virtual void reduceSize(int) = 0;
    virtual void shuffle() = 0;

    virtual void traverse(function<void(T &)>) = 0;
};

template <class T>
ostream &operator<<(ostream &os, LinearList<T> *array)
{
    array->traverse([&os](T &val)
                    { os << val << " "; });
    os << '\n';
    return os;
}

#define ARRAY_BLOCK_SIZE 32

// TODO: Array implementation of linear List
template <class T>
class ArrayList : public LinearList<T>
{
protected:
    T *pD;
    int nE, cap;

public:
    ArrayList() : nE(0), cap(0), pD(nullptr) {}
    ArrayList(int N) : nE(0), cap((N + ARRAY_BLOCK_SIZE - 1) / ARRAY_BLOCK_SIZE * ARRAY_BLOCK_SIZE)
    {
        pD = new T[cap];
    }
    ~ArrayList()
    {
        clear();
    }
    void resize(int N)
    {
        int newCap = (N + ARRAY_BLOCK_SIZE - 1) / ARRAY_BLOCK_SIZE * ARRAY_BLOCK_SIZE;
        T *newPD = new T[newCap];
        if (N < nE)
            nE = N;
        for (int i = 0; i < nE; i++)
        {
            newPD[i] = std::move(pD[i]);
        }
        delete[] pD;
        pD = newPD;
        cap = newCap;
    }

    virtual bool isEmpty() { return !nE; }
    virtual int getSize() { return nE; }
    virtual void clear()
    {
        if (pD)
        {
            delete[] pD;
            pD = nullptr;
            nE = 0;
        }
    }
    virtual LinearList<T> *clone(int sidx = 0, int eidx = -1)
    {

        // FIXME: missing constructor
        if (eidx < 0)
            eidx = nE;
        LinearList<T> *pL = new ArrayList<T>(eidx - sidx);
        for (int i = sidx; i < eidx; ++i)
        {
            pL->insert(pD[i], i - sidx);
        }
        return pL;
    }

    virtual T &operator[](int index) // precondition: index must be valid
    {
        // if (index < 0 || index >= nE)
        //     throw DSA_Exception{911, "ArrayList::operator[]: Invalid index"};
        return pD[index];
    }

    virtual bool insert(const T &value, int index)
    {
        if (nE == cap)
        {
            resize(nE + 1);
        }
        for (int i = nE; i > index; i--)
        {
            pD[i] = std::move(pD[i - 1]);
        }
        pD[index] = value;
        ++nE;
        return true;
    }
    virtual bool insert(T &&value, int index)
    {
        if (nE == cap)
        {
            resize(nE + 1);
        }
        for (int i = nE; i > index; i--)
        {
            pD[i] = std::move(pD[i - 1]);
        }
        pD[index] = std::move(value);
        ++nE;
        return true;
    } // move an entire object
    virtual bool append(const T &value)
    {
        return insert(value, nE);
    }
    virtual bool append(T &&value)
    {
        return insert(std::move(value), nE);
    }

    virtual bool removeIdx(int index)
    {
        if (!nE)
            return false;

        for (int i = index; i < nE - 1; ++i)
        {
            pD[i] = std::move(pD[i + 1]);
        }
        --nE;
        return true;
    }
    virtual bool remove(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; })
    {
        int idxNew = 0;
        int idxOld = 0;
        for (; idxOld < nE; ++idxOld, ++idxNew)
        {
            while (eq(pD[idxOld], k) && idxOld < nE)
                ++idxOld;
            if (idxOld != idxNew && idxOld < nE)
            {
                pD[idxNew] = std::move(pD[idxOld]);
            }
        }
        nE = idxNew;
        return true;
    } // remove an item by a comparison to a key

    virtual T *find(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                    { return val == key; })
    {
        for (T *p = pD, *pE = pD + nE; p != pE; p++)
        {
            if (eq(*p, k))
                return p;
        }
        return nullptr;
    }

    virtual int findIdx(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; })
    {
        for (T *p = pD, *pE = pD + nE; p != pE; p++)
        {
            if (eq(*p, k))
                return (p - pD);
        }
        return -1;
    }

    virtual LinearList<void *> *findAll(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                                        { return val == key; })
    {
        LinearList<void *> *pL = new ArrayList<void *>;
        traverse([&pL, &k, &eq](T &val)
                 { if (eq(val, k)) pL->append((void*)&val); });
        return pL;
    }

    virtual void swap(int idxA, int idxB)
    {
        T temp(std::move(pD[idxA]));
        pD[idxA] = std::move(pD[idxB]);
        pD[idxB] = std::move(temp);
    }
    virtual void reverse()
    {
        for (int left = 0, right = nE - 1; left < right; ++left, --right)
        {
            T temp(std::move(pD[left]));
            pD[left] = std::move(pD[right]);
            pD[right] = std::move(temp);
        }
    }
    virtual LinearList<T> *split(int index)
    {
        LinearList<T> *pL = new ArrayList<T>(nE - index);
        for (int i = index; i < nE; i++)
        {
            pL->append(pD[i]);
        }
        nE = index;

        return pL;
    } // split from some index, the returned list will be the second one while the current list will be reduced to the first
    virtual void merge(LinearList<T> *pL)
    {
        resize(nE + pL->getSize());
        pL->traverse([&pL, this](T &val)
                     { this->append(val); });
    }

    virtual void reduceSize(int newCap)
    {
        resize(newCap);
    }
    virtual void shuffle()
    {
    }
    virtual void traverse(function<void(T &)> action)
    {
        for (T *p = pD, *pE = pD + nE; p != pE; p++)
        {
            action(*p);
        }
    }
};

// TODO: Linked List implementation of LinearList
template <class T>
class L1List : public LinearList<T>
{
protected:
    struct Node
    {
        T data;
        Node *pNext;
        Node(const T &val, Node *p = nullptr) : data(val), pNext(p) {}
        Node(T &&val, Node *p = nullptr) : data(std::move(val)), pNext(p) {}
    };

    Node *pHead, *pTail;
    int nE;

public:
    L1List() : nE(0), pHead(nullptr), pTail(nullptr) {}
    ~L1List() { clear(); }

    void resize(int N)
    {
        return;
    }

    virtual bool isEmpty() { return !nE; }
    virtual int getSize() { return nE; }
    virtual void clear()
    {
        while (!pHead)
        {
            Node *p = pHead;
            pHead = pHead->pNext;
            delete p;
        }
        nE = 0;
        pTail = nullptr;
    }
    virtual LinearList<T> *clone(int sidx = 0, int eidx = -1)
    {
        if (eidx < 0)
            eidx = nE;
        LinearList<T> *pL = new L1List<T>;
        Node *p = this->pHead, **pNL = &((L1List<T>*)pL->pHead);
        eidx -= sidx;
        pL->nE = eidx;
        while (sidx != 0 && p != nullptr)
        {
            p = p->next;
            --sidx;
        }

        while (p && eidx)
        {
            --eidx;
            *pNL = new Node(p->data);
            // pL->pTail = *pNL;
            pNL = &(*pNL->pNext);
            p = p->next;
        }

        // Get the tail
        // risky option
        pL->pTail = (Node *)((char *)pNL - sizeof(T)); //  uses data alignment

        // // safe option
        // Node *pT = pL->pHead;
        // while (pT->pNext) pT = pT->pNext;
        // pL->pTail = pT;

        return pL;
    }

    virtual T &operator[](int index) // precondition: index must be valid
    {
        // if (index < 0 || index >= nE)
        //     throw DSA_Exception{911, "ArrayList::operator[]: Invalid index"};
        // return pD[index];
        Node *pT = pHead;
        while (pT && index)
        {
            pT = pT->pNext;
            --index;
        }
        if (pT)
            return pT->data;
        else
            throw out_of_range("Invalid index");
    }

    virtual bool insert(const T &value, int index)
    {
        Node **pp = &pHead;
        while (*pp && index)
        {
            --index;
            pp = &((*pp)->pNext);
        }
        (*pp) = new Node(value, *pp);
        ++nE;
        if (pTail->pNext)
            pTail = pTail->pNext;
        
        return true;
    }
    virtual bool insert(T &&value, int index)
    {
        Node **pp = &pHead;
        while (*pp && index)
        {
            --index;
            pp = &((*pp)->pNext);
        }
        (*pp) = new Node(std::move(value), *pp);
        ++nE;
        if (pTail->pNext)
            pTail = pTail->pNext;
        return true;
    }
    // move an entire object
    virtual bool append(const T &value)
    {
        if (pHead)
        {
            pTail->pNext = new Node(value);
            pTail = pTail->pNext;
        }
        else
        {
            pHead = pTail = new Node(value);
        }
        ++nE;
        return true;
    }
    virtual bool append(T &&value)
    {
        if (pHead)
        {
            pTail->pNext = new Node(std::move(value));
            pTail = pTail->pNext;
        }
        else
        {
            pHead = pTail = new Node(std::move(value));
        }
        ++nE;
        return true;
    }

    virtual bool removeIdx(int index)
    {
        if (!nE)
            return false;
        Node **pp = &pHead;
        while (*pp && index)
        {
            --index;
            pp = &((*pp)->pNext);
        }
        if (*pp)
        {
            --nE;
            Node *p = *pp;
            if (p->pNext == nullptr)
            {
                pTail = (Node *)((char *)pp - sizeof(T)); // FIXME: VERY RISKY
            }
            *pp = (*pp)->pNext;
            delete p;
        }
        else
            return false;

        return true;
    }
    virtual bool remove(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; })
    {
        if (!nE)
            return false;
        Node **pp = &pHead;
        while (*pp)
        {
            while (eq((*pp)->data, k)) // remove it
            {
                Node *p = *pp;
                *pp = (*pp)->pNext;
                delete p;
                --nE;
            }
            if ((*pp) != nullptr)
                pp = &((*pp)->pNext);
            else
                pTail = (Node *)((char *)pp - sizeof(T));
        }
        return true;
    } // remove an item by a comparison to a key

    virtual T *find(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                    { return val == key; })
    {
        for (Node *p = pHead; p != nullptr; p = p->next)
        {
            if (eq(p->data, k))
                return p->data;
        }
        return nullptr;
    }

    virtual int findIdx(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                        { return val == key; })
    {
        int index = 0;
        for (Node *p = pHead; p != nullptr; p = p->pNext)
        {
            if (eq(p->data, k)) return index;
            index++; 
        }
        return -1;
    }

    virtual LinearList<void *> *findAll(const T &k, function<bool(const T &, const T &)> eq = [](const T &val, const T &key) -> bool
                                        { return val == key; })
    {
        LinearList<void *> *pL = new ArrayList<void *>;
        // traverse([&pL, &k, &eq](T &val)
        //          { if (eq(val, k)) pL->append((void*)&val); });
        return pL;
    }

    virtual void swap(int idxA, int idxB) // suppose indices are valid idxA < idxB
    {
        Node **pp1 = &pHead, **pp2 = nullptr;
        while (idxA)
        {
            --idxA;
            --idxB;
            pp1 = &((*pp1)->pNext);
        }
        if (idxB == 1) // swapping two elements next to each other
        {
            Node *pA = *pp1;
            Node *pB = pA->pNext;
            Node *pC = pB->pNext;

            *pp1 = pB;
            pB->pNext = pA;
            pA->pNext = pC;
            return;
        }
        else
        {
            pp2 = &((*pp1)->pNext);
            while (idxB > 1)
            {
                --idxB;
                pp2 = &((*pp2)->pNext);
            }
            Node *pA = *pp1;
            Node *pA1 = pA->pNext;
            Node *pB = *pp2;
            Node *pB1 = pB->pNext;

            (*pp1) = pB;
            pB->pNext = pA1;

            (*pp2) = pA;
            pA->pNext = pB1;
        }
    }
    virtual void reverse()
    {
        Node *pNL = nullptr;
        while (pHead)
        {
            Node *p = pHead;
            pHead = pHead->pNext;

            p->pNext = pNL;
            pNL = p;
        }
        pHead = pNL;
    }
    virtual LinearList<T> *split(int index)
    {
        LinearList<T> *pL = new L1List<T>;
        Node **pp = &pHead;
        (L1List<T>*)(pL)->nE = this->nE - index;
        nE = index;
        pL->pTail = this->pTail;

        while (index)
        {
            --index;
            pp = &((*pp)->pNext);

        }
        
        pL->pHead = *pp;
        *pp = nullptr;
        this->pTail = (Node *)((char *)pp - sizeof(T));

        return pL;
    } // split from some index, the returned list will be the second one while the current list will be reduced to the first
    virtual void merge(LinearList<T> *pL)
    {
        LinearList<T> *p = this;
        pL->traverse([p](T &val)
                     { p->append(val); });
    }

    virtual void reduceSize(int newCap)
    {
        return;
    }
    virtual void shuffle()
    {
    }
    virtual void traverse(function<void(T &)> action)
    {
        for (Node *p = pHead; p != nullptr; p = p->pNext)
        {
            action(p->data);
        }
    }
};

#endif