#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *n = malloc(sizeof(struct list_head));
    if(!n){
        return NULL;
    }
    INIT_LIST_HEAD(n);
    return n;  
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if(!l){
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, l, list){
        q_release_element(entry);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *n = malloc(sizeof(element_t));
    n->value = malloc((strlen(s)+1)*sizeof(char));
    if (!n || !(n->value))
        return false;
    //memset(n->value, 0, (strlen(s)+1)*sizeof(char));
    strncpy(n->value, s, strlen(s)+1);
    list_add(&n->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *n = malloc(sizeof(element_t));
    n->value = malloc((strlen(s)+1)*sizeof(char));
    if (!n || !(n->value))
        return false;
    //memset(n->value, 0, (strlen(s)+1)*sizeof(char));
    strncpy(n->value, s, strlen(s)+1);
    list_add_tail(&n->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head) || !head)
        return NULL;
    element_t *e = list_first_entry(head, element_t, list);
    list_del(head);
    if (sp){
        memset(sp, 0, bufsize);
        strncpy(sp, e->value, bufsize-1);
    }
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head) || !head)
        return NULL;
    element_t *e = list_last_entry(head, element_t, list);
    list_del(&e->list);
    if(sp){
        memset(sp, 0, bufsize);
        strncpy(sp, e->value, bufsize-1);
    }
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (list_empty(head) || !head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (list_empty(head) || !head)
        return false;
    int mid = q_size(head)/2;
    struct list_head *tmp = head;
    for (int i = 0; i < mid; i++){
        tmp = tmp->next;
    }
    list_del(tmp);
    q_release_element(list_entry(tmp, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (list_empty(head) || !head)
        return false;
    q_sort(head);
    struct list_head *node;
    list_for_each(node, head){
        element_t *now = list_entry(node, element_t, list);
        element_t *next = list_entry(node->next, element_t, list);
        if(now->value == next->value){
            struct list_head *tmp = node->next;
            do
            {
                tmp = tmp->next;
                list_del(node->next);
                q_release_element(next);
                next = list_entry(tmp, element_t, list);
            } while (now->value == next->value);
            node = tmp;
            list_del(tmp->prev);
            q_release_element(list_entry(tmp->prev, element_t, list));
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (list_empty(head) || !head)
        return;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (q_size(head) < 2 || !head)
        return;
    struct list_head *fptr = head, *lptr;
    list_for_each(lptr,head){}
    do{
        element_t *first = list_entry(fptr, element_t, list), *last = list_entry(lptr, element_t, list);
        char *tmp = first->value;
        first->value = last ->value;
        last->value = tmp;
        fptr = fptr->next;
        lptr = lptr->prev;
    }while(fptr != lptr && fptr != lptr->next);
}

//merge two sorted list
struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    if(!l2 || list_empty(l2))
        return l1;
    else if(!l1 || list_empty(l1))
        return l2;
    element_t *e1 = list_entry(l1, element_t, list);
    element_t *e2 = list_entry(l2, element_t, list);
    struct list_head *head = NULL, **tmp = &head;
    while (l1 && l2){
        if(strcmp(e1->value, e2->value) < 0){
            *tmp = l1;
            l1 = l1->next;
        }else{
            *tmp = l2;
            l2 = l2->next;
        }
        tmp = &((*tmp)->next);
    }

    if(list_entry(l1, element_t, list))
        *tmp = l1;
    if(list_entry(l2, element_t, list))
        *tmp = l2;
    return  head;  
}

//sort list segments
struct list_head *lsort(struct list_head *head, int qSize)
{
    if(!head || qSize < 2)
        return head;
    struct list_head *l1=head, *l2=head;
    for (int i = 0; i < qSize/2; i++)
        l2 = l2->next;
    l1 = lsort(l1, qSize/2);
    if(qSize-(qSize/2)*2)
        l2 = lsort(l2, qSize/2);
    else
        l2 = lsort(l2, qSize/2+1);
    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (q_size(head) < 2 || !head)
        return;
    head = lsort(head, q_size(head));
}
