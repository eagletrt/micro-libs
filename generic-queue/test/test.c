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
    int tot = 30;

    uint8_t *mem = (uint8_t*)malloc(sizeof(test)*tot);
    GENQ_init(&q, sizeof(test)*tot, sizeof(test), mem);

    test v[n];
    char prova[] = "asdaa";
    for(int i=0;i<n;i++)
    {
        v[i].v = i+1;
        memcpy(v[i].prova, prova, sizeof(char)*5);
    }
    int cnt = 0;
    for(int i=0;i<n;i++)
    {
        if(!GENQ_push(&q, (uint8_t*)(v+i)))
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
        if(!GENQ_pop(&q, (uint8_t*)(&tmp)) && tmp.v == i+1)
        {
            printf("error\n");
            return 11;
        }
        cnt--;
    }
    for(int i=0;i<n;i++)
    {
        if(!GENQ_push(&q, (uint8_t*)(v+i)))
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
        if(GENQ_pop(&q, (uint8_t*)(&tmp)) && tmp.v == i+11)
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

    free(mem);
    //printf("popped %d elements\n", cnt);
}