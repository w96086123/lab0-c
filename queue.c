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
    struct list_head *current, *next;  // Pointers for traversing the list

    // Traverse the list safely
    list_for_each_safe (current, next, head) {
        // Delete the node from the list
        list_del(current);

        // Free the memory allocated for the current
        free(current);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    // Create a new element
    element_t *new_element = malloc(sizeof(element_t));

    // Check
    if (new_element == NULL) {
        // Memory allocated failed
        free(new_element);
        return false;
    }


    // Copy s to new element's value
    new_element->value = strdup(s);

    // Check
    if (new_element->value == NULL) {
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
    if (!head)
        return false;

    // Create a new element
    element_t *new_element = (element_t *) malloc(sizeof(element_t));

    // Check
    if (new_element == NULL) {
        // Memory allocated failed
        return false;
    }

    // Copy s to new element's value
    new_element->value = strdup(s);

    // Check
    if (new_element->value == NULL) {
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

    while (fast->next != head && fast->next->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

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
    if (!head || !head->next)
        return true;

    struct list_head *current = head->next;
    struct list_head *prev = head;

    // If the queue have duplicate value, this value will become true
    bool dup = false;

    while (current) {
        element_t *current_node = list_entry(current, element_t, list);
        element_t *prev_node = list_entry(prev, element_t, list);

        if (current_node->value == prev_node->value) {
            dup = true;
            // Delete current node and prev->next point to current->next
            struct list_head *temp = current;
            current = current->next;
            prev->next = current;
            list_del(temp);
            free(temp);
            free(current_node);
        } else {
            prev = current;
            current = current->next;
        }
    }

    return dup;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (!head || !head->next)
        return;

    struct list_head *current = head->next;
    struct list_head *pre = head;

    while (current && current->next) {
        struct list_head *next_pair = current->next->next;

        pre->next = current->next;
        current->next->prev = pre;

        current->next = next_pair;
        if (next_pair)
            next_pair->prev = current;

        pre->prev = current;
        current->prev = NULL;

        pre = current;
        current = next_pair;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *front = head->prev, *cur = head, *next = NULL;
    while (next != head) {
        next = cur->next;
        cur->next = front;
        cur->prev = next;
        front = cur;
        cur = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
