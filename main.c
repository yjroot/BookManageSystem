#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//마감시간 12월 18일 오후 6시
//email 제출 tegoshiyu@naver.com

#define error(MESSAGE) fprintf(stderr, "[%s:%d] %s\n", __FILE__, __LINE__, MESSAGE)
#define stralloc(str) strcpy((char*)malloc(strlen(str)), str)

unsigned int hash(char *pKey) {
    unsigned int ret = 0;
    while(*pKey != '\0') {
        ret = (ret * 31) + (*pKey);
        pKey++;
    }
    return ret;
}

typedef struct BSTElement {
    unsigned int hash;
    char* key;
    void* value;
    struct BSTElement *left;
    struct BSTElement *right;
} bstElement;

typedef struct HashTable {
    int bucketSize;
    int currentElementCount;
    struct BSTElement *element[];
} hTable;

hTable* hash_createTable(unsigned int bucketSize) {
    size_t size = sizeof(hTable) + (sizeof(bstElement*)*bucketSize);
    hTable* hReturn = (hTable*)malloc(size);
    if (hReturn != NULL) {
        memset(hReturn, 0, size);
        hReturn->bucketSize = bucketSize;
    }else {
        error("malloc(), hash_createTable()");
        return NULL;
    }
    return hReturn;
}

void hash_add(hTable* table, char* key, void* value) {
    unsigned int keyHash = hash(key);

    bstElement* element = (bstElement*)malloc(sizeof(bstElement));
    if (element != NULL) {
        element->hash = keyHash;
        element->key = stralloc(key);
        element->value = value;
        element->left = NULL;
        element->right = NULL;
    }else {
        error("malloc(), hash_createTable()");
    }
    
    unsigned int bucketNo = keyHash % table->bucketSize;
    bstElement** pElement = &(table->element[bucketNo]);
    while((*pElement)!=0){
        if((*pElement)->hash > keyHash){
            pElement = &((*pElement)->left);
        }else{
            pElement = &((*pElement)->right);
        }
    }
    *pElement = element;
}

void hash_elementFree(bstElement* element) {
    if(element->left) {
        hash_elementFree(element->left);
    }
    if(element->right) {
        hash_elementFree(element->right);
    }
    free(element->key);
    free(element);
}

void* hash_search(hTable* table, char* key) {
    unsigned int keyHash = hash(key);
    
    unsigned int bucketNo = keyHash % table->bucketSize;
    bstElement** pElement = &(table->element[bucketNo]);
    while((*pElement)!=0){
        if((*pElement)->hash > keyHash){
            pElement = &((*pElement)->left);
        }else if((*pElement)->hash < keyHash){
            pElement = &((*pElement)->right);
        }else{
            return (*pElement)->value;
        }
    }
    return NULL;
}

void hash_delete(hTable* table, char* key) {
    unsigned int keyHash = hash(key);
    
    unsigned int bucketNo = keyHash % table->bucketSize;
    bstElement** pElement = &(table->element[bucketNo]);
    while((*pElement)!=0){
        if((*pElement)->hash > keyHash){
            pElement = &((*pElement)->left);
        }else if((*pElement)->hash < keyHash){
            pElement = &((*pElement)->right);
        }else{
            bstElement* target = *pElement;
            if(target->left==NULL && target->right==NULL){
                *pElement = NULL;
                hash_elementFree(target);
            }else if(target->left==NULL){
                *pElement = target->right;
                target->right = NULL;
                hash_elementFree(target);
            }else if(target->right==NULL){
                *pElement = target->left;
                target->left = NULL;
                hash_elementFree(target);
            }else {
                bstElement** maximum = &(target->left);
                while((*maximum)->right){
                    maximum = &((*maximum)->right);
                }
    
                bstElement* change = *maximum;
                *maximum = change->left;
                change->left = target->left;
                change->right = target->right;
                *pElement = change;

                target->left = NULL;
                target->right = NULL;
                hash_elementFree(target);
            }
            break;
        }
    }
}

int main(int argc, char **argv) {
    hTable* table = hash_createTable(10);
    hash_add(table, "a", 1);
    hash_add(table, "b", 2);
    hash_add(table, "c", 3);
    printf("a = %d\n", hash_search(table, "a"));
    printf("b = %d\n", hash_search(table, "b"));
    printf("c = %d\n", hash_search(table, "c"));

    return 0;
}
