#include "stdio.h"
#include "../generic_queue.h"

generic_queue_t q;

#define n 10000

typedef struct{
    long int v;
    char prova[5];
}  test;

int main()
{
    GENQ_init(&q, sizeof(test));

    test v[n];
    char prova[] = "asdaa";
    for(int i=0;i<n;i++)
    {
        v[i].v = i+1;
        memcpy(v[i].prova, prova, sizeof(char)*5);
    }
    int tot = GENQ_SIZE / sizeof(test);
    int cnt = 0;
    for(int i=0;i<n;i++)
    {
        if(!GENQ_push(&q, v+i))
        {
            //printf("errore push\n");
            break;
        }
        else
            cnt++;
    }
    int check = (cnt == tot);
    //printf("pushed %d elements\n", cnt);
    for(int i=0;i<10;i++)
    {
        test tmp;
        if(!GENQ_pop(&q, &tmp))
        {
            printf("error\n");
            return 11;
        }
        cnt--;
    }
    for(int i=0;i<n;i++)
    {
        if(!GENQ_push(&q, v+i))
        {
            //printf("errore push\n");
            break;
        }
        else
            cnt++;
    }
    check = check || (cnt == tot);
    cnt = 0;
    for(int i=0;i<n;i++)
    {
        test tmp;
        if(GENQ_pop(&q, &tmp))
        {
            //printf("%d %s\n", tmp.v, tmp.prova);
            cnt++;
        }
        else
        {
            //printf("errore pop\n");
            break;
        }
    }
    check = check || (cnt == 0);
    printf("maximum elements: %d\n", tot);
    if(check) printf("test passed\n");
    else printf("test not passed\n");
    //printf("popped %d elements\n", cnt);
}