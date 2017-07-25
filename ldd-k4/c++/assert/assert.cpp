#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
struct ITEM
{
    int key;
    int value;
};
/*add item to list,make sure list is not null*/
void additem(struct ITEM* itemptr)
{
    assert(itemptr!=NULL);
    /*additemtolist*/
}
int main(void)
{
    additem(NULL);
    return 0;
}
