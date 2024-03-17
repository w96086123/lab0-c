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

struct list_head *merge_list(struct list_head *first_list,
                             struct list_head *second_list,
                             bool descend)
{
    // Prepare empty list to store the result
    LIST_HEAD(result);
    INIT_LIST_HEAD(&result);
    struct list_head *tmp = &result;
    // Merge second list into first list until one is empty
    while (first_list && second_list) {
        element_t *first_element = list_entry(first_list, element_t, list);
        element_t *second_element = list_entry(second_list, element_t, list);

        if (cmp(first_element->value, second_element->value, descend) < 0) {
            tmp->next = first_list;
            first_list = first_list->next;
        } else {
            tmp->next = second_list;
            second_list = second_list->next;
        }
        tmp = tmp->next;
    }
    // Set tmp->next to either first_list or second_list depending on whether
    // first_list is not NULL
    tmp->next = first_list ? first_list : second_list;

    return result.next;
}

struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;

    // Use fast and slow pointer
    struct list_head *slow = head, *fast = head->next, *new_head;

    // Find to middle node
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    // Split
    new_head = slow->next;
    slow->next = NULL;
    // Merge
    head = merge_sort(head, descend);
    new_head = merge_sort(new_head, descend);

    return merge_list(head, new_head, descend);
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    struct list_head *list = head->next, *pre, *node;
    head->prev->next = NULL;
    head->next = merge_sort(list, descend);

    for (pre = head, node = head->next; node->next != NULL;
         pre = node, node = node->next) {
        node->prev = pre;
    }
    node->next = head;
    head->prev = node;
}

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
 * order */    /* code */

int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    struct list_head *list_cur = head->next;
    struct list_head *list_next = list_cur->next;

    queue_contex_t *contex_cur = list_entry(list_cur, queue_contex_t, chain);

    while (list_next != head) {
        queue_contex_t *contex_next =
            list_entry(list_next, queue_contex_t, chain);
        contex_next = list_entry(list_next, queue_contex_t, chain);
        merge_list(contex_cur->q, contex_next->q, descend);
        list_next = list_next->next;
    }

    int contex_size = q_size(contex_cur->q);
    if (descend) {
        q_reverse(list_cur);
    }
    return contex_size;
}
