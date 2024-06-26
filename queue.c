#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    /* Allocate memory dynamically for new list_head */
    struct list_head *new_head = malloc(sizeof(struct list_head));

    /* Check if memory allocation was successful */
    if (!new_head) {
        /* Allocation failed, return NULL */
        free(new_head);
        return NULL;
    }

    /* Initialize the new list_head to point to itself, indicating empty queue
     */
    INIT_LIST_HEAD(new_head);
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *current, *next;  // Pointers for traversing the list

    // Traverse the list safely
    list_for_each_entry_safe (current, next, head, list) {
        q_release_element(current);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    // Create a new element
    element_t *new_element = malloc(sizeof(element_t));

    if (!new_element) {
        return false;
    }

    // Copy s to new element's value
    new_element->value = strdup(s);

    // Check
    if (!new_element->value) {
        // String copied failed
        free(new_element);
        return false;
    }

    // New element insert to list head
    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    // Create a new element
    element_t *new_element = malloc(sizeof(element_t));

    if (!new_element) {
        return false;
    }

    // Copy s to new element's value
    new_element->value = strdup(s);

    // Check
    if (!new_element->value) {
        // String copied failed
        free(new_element);
        return false;
    }
    // New element insert to list tail
    list_add_tail(&new_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // Is this queue empty
    if (list_empty(head)) {
        return NULL;
    }


    // Get first element from the queue
    element_t *remove_element = list_first_entry(head, element_t, list);

    if (remove_element) {
        // Copy the value to sp
        memcpy(sp, remove_element->value, bufsize);
        sp[bufsize - 1] = '\0';

        // Remove the element from the queue
        list_del(&remove_element->list);
    }

    return remove_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // Is this queue empty
    if (list_empty(head)) {
        return NULL;
    }

    // Get first element from the queue
    element_t *remove_element = list_last_entry(head, element_t, list);

    if (remove_element) {
        // Copy the value to sp
        memcpy(sp, remove_element->value, bufsize);
        sp[bufsize - 1] = '\0';

        // Remove the element from the queue
        list_del(&remove_element->list);
    }

    return remove_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    // Is this a empty queue
    if (!head)
        return false;

    // Use fast and slow pointer
    struct list_head *slow = head, *fast = head;

    // Find to middle node
    while (fast->next != head && fast->next->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    // Entry middle node
    element_t *mid = list_entry(slow->next, element_t, list);

    // Check middle node is exist
    if (!mid)
        return false;

    // Delete middle node from the queue
    list_del(slow->next);
    q_release_element(mid);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    // If queue is empty or one node, we can return true
    if (!head)
        return false;
    if (list_empty(head) || list_is_singular(head))
        return true;

    // If the queue have duplicate value, this value will become true
    bool dup = false;
    element_t *cur, *next;
    list_for_each_entry_safe (cur, next, head, list) {
        if (&next->list != head && strcmp(cur->value, next->value) == 0) {
            dup = true;
            list_del(&cur->list);
            free(cur->value);
            free(cur);
        } else if (dup) {
            dup = false;
            list_del(&cur->list);
            free(cur->value);
            free(cur);
        }
    }


    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (!head || !head->next)
        return;

    struct list_head *current, *pre;
    // Initialization
    for (pre = head->next, current = pre->next; pre != head && current != head;
         pre = pre->next, current = pre->next) {
        // Move pre to current
        list_move(pre, current);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *current = NULL;
    struct list_head *next = NULL;

    list_for_each_safe (current, next, head) {
        current->next = current->prev;
        current->prev = next;
    }

    current->next = current->prev;
    current->prev = next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    if (!head || list_empty(head))
        return;
    struct list_head *current = NULL;
    struct list_head *next = NULL;
    struct list_head *reverseHead = head;

    // Get number of group
    int num = q_size(head) / k;
    int count = k - 1;

    for (; num != 0; num--) {
        current = reverseHead->next;
        next = current->next;

        for (; count != 0; count--) {
            list_del(next);
            list_add(next, reverseHead);
            next = current->next;
        }
        reverseHead = current;
        count = k - 1;
    }
}

int cmp(const char *a, const char *b, bool descend)
{
    if (descend) {
        return (strcmp(b, a));
    } else {
        return (strcmp(a, b));
    }
}

void merge_list(struct list_head *result,
                struct list_head *first_list,
                struct list_head *second_list,
                bool descend)
{
    // Merge second list into first list until one is empty
    while (!list_empty(first_list) && !list_empty(second_list)) {
        element_t *first_element =
            list_entry(first_list->next, element_t, list);
        element_t *second_element =
            list_entry(second_list->next, element_t, list);
        if (cmp(first_element->value, second_element->value, descend) < 0) {
            list_move_tail(first_list->next, result);
        } else {
            list_move_tail(second_list->next, result);
        }
    }
    // Set tmp->next to either first_list or second_list depending on whether
    // first_list is not NULL
    if (!list_empty(first_list)) {
        list_splice_tail_init(first_list, result);
    } else {
        list_splice_tail_init(second_list, result);
    }
}

void merge_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // Use fast and slow pointer
    struct list_head *slow, *fast;
    for (slow = head->next, fast = slow; fast != head && fast->next != head;
         fast = fast->next->next) {
        slow = slow->next;
    }
    // Split
    LIST_HEAD(left_head);
    LIST_HEAD(mid_head);
    list_cut_position(&left_head, head, slow->prev);
    list_splice(head, &mid_head);
    INIT_LIST_HEAD(head);

    merge_sort(&left_head, descend);
    merge_sort(&mid_head, descend);
    // Merge
    merge_list(head, &left_head, &mid_head, descend);
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    merge_sort(head, descend);
}

int q_descend_or_ascend(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    int num = 0;
    struct list_head *cur = head->prev;
    char *max = list_entry(cur, element_t, list)->value;
    cur = cur->prev;
    while (cur != head) {
        num++;
        element_t *cur_element = list_entry(cur, element_t, list);
        bool del_prev = cmp(max, cur_element->value, descend) < 0;
        if (del_prev) {
            struct list_head *tmp = cur;
            cur = cur->prev;
            list_del(tmp);
            element_t *tmp_ele = list_entry(tmp, element_t, list);
            free(tmp_ele->value);
            free(tmp_ele);
            num--;
        } else {
            max = cur_element->value;
            cur = cur->prev;
        }
    }
    num++;
    return num;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_descend_or_ascend(head, false);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_descend_or_ascend(head, true);
}



/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */    /* code */
// https://github.com/ShawnXuanc/lab0-c/blob/master/queue.c
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    LIST_HEAD(result);
    INIT_LIST_HEAD(&result);

    struct list_head *list_next = head->next->next;
    struct list_head *first = head->next;
    while (list_next != head) {
        queue_contex_t *first_context =
            list_entry(first, queue_contex_t, chain);
        queue_contex_t *contex_next =
            list_entry(list_next, queue_contex_t, chain);
        size += contex_next->size;

        merge_list(&result, first_context->q, contex_next->q, descend);
        list_splice_init(&result, first_context->q);
        list_next = list_next->next;
    }

    return size;
}
