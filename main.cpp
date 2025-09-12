#include "dsaLib.h"

int main()
{
    LinearList<int> *pL = new ArrayList<int>(10);
    for (int i = 0; i < 20; i++)
        pL->append(rand() % 10);

    cout << pL;

    pL->remove(5);
    cout << pL;
    delete pL;
    return 0;
}