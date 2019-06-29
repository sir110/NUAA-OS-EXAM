#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int len=0;

void sort(void *p)
{
    int *x = (int*)p;
    int i, j, k;
    for(i=0; i<len/2-1; i++)
    {
        k = i;
        for(j=i+1; j<len/2; j++)
        {
            if(x[j] > x[k])
                k = j;
        }
        if(k != i)
        {
            x[i] = x[i]^x[k];
            x[k] = x[i]^x[k];
            x[i] = x[i]^x[k];
        }
    }
}

void merge(int *a, int start, int mid, int end)
{
    int *tmp = (int *)malloc((end-start+1)*sizeof(int));
    int i = start;
    int j = mid+1;
    int k = 0;
    while(i <= mid && j<=end)
    {
        if(a[i] >= a[j])
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }
    while(i <= mid)
        tmp[k++] = a[i++];
    while(j <= end)
        tmp[k++] = a[j++];
    for(i=0; i<k; i++)
    {
        a[start+i] = tmp[i];
    }
    free(tmp);
}

int main()
{
    int array[100]={100,123,456,789,963,852,741,1,56,789456,123,555,9}; 
    int i=0;
    while(array[i])
    {
        len++;
        i++;
    }
    printf("%d\n", len);
    pthread_t tid;
    sort(array);
    pthread_create(&tid, NULL, (void*)&sort, (void*)(array+len/2));
    pthread_join(tid, NULL);
    
    i = 0;
    while(i < len)
    {
        printf("%d ", array[i++]);
    }
    printf("\n");
    
    if(len%2 != 0)
    {
        merge(array, 0, len/2-1, len-2);
        merge(array, 0, len-2, len-1);
    }
    else
        merge(array, 0, len/2-1, len-1);
    i = 0;
    while(i < len)
        printf("%d ", array[i++]);
    printf("\n");
    return 0;
}
