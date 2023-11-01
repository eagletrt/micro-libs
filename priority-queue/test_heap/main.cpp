#include <queue>
#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include "../priority_queue_heap.h"

using namespace std;

const int n = 100;
struct prova{
    int id;
    char name[50];
};
prova *tmp = new prova[n];
vector<int> priority(n);

PQ *pq2;

class Compare {
public:
    bool operator()(prova a, prova b)
    {
        return priority[a.id] >= priority[b.id];
    }
};

bool compare(int a, int b)
{
    return a >= b;
}
priority_queue<prova, vector<prova>, Compare> pq1;

void push()
{
    static int last_push = 0;
    for(;last_push<n;last_push++)
    {
        if(PQ_insert(pq2, priority[last_push], (uint8_t*)&tmp[last_push]) == 0) 
            break;
        pq1.push(tmp[last_push]);
    }
}
void pop()
{
    int cnt = 0;
    while(!PQ_is_empty(pq2))
    {
        if(cnt++ > n/3) break;
        prova tmp1, tmp2;
        int p;
        PQ_pop(pq2, &p, (uint8_t*)&tmp1);
        tmp2 = pq1.top();
        pq1.pop();
        cout<<tmp1.name<<" | "<<tmp2.name<<endl;
        assert(priority[tmp2.id] == p);
        assert(strcmp(tmp1.name, tmp2.name) == 0);
    }
}

int main()
{
    pq2 = (PQ*)malloc(sizeof(PQ));
    for(int i=0;i<n;i++)
    {
        priority[i] = i;
        tmp[i].id = i;
        string str = "test"+to_string(i);
        cout<<str<<" "<<priority[i]<<endl;
        memcpy(tmp[i].name, str.c_str(), str.size());
    }
    int size = 30*(sizeof(prova));
    auto mem = (uint8_t*)malloc(size);
    int *pmem = (int*)malloc(4*30);
    PQ_init(pq2, size, sizeof(prova), mem, pmem, PQ_GE);
    random_shuffle(priority.begin(), priority.end());
    push();
    cout<<"OK push\n";
    pop();
    cout<<"OK pop\n";
    push();
    pop();

    cout<<"Test passed"<<endl;

    delete[] tmp;
    free(pq2);
    free(mem);
    free(pmem);

    return 0;
}

