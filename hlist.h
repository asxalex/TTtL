/*
 * hlist.h
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HLIST_H
#define HLIST_H

#include <stdio.h>

typedef struct hlist_node_s hlist_node_t;
typedef struct hlist_head_s hlist_t;

struct hlist_node_s {
    struct hlist_node_s **pprev, *next;
};

struct hlist_head_s {
    struct hlist_node_s *first;
};

// init the hlist head
#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = { .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

#define LIST_POISON1 NULL
#define LIST_POISON2 NULL

// init the node
static inline void INIT_HLIST_NODE(hlist_node_t *n) {
    n->next = NULL;
    n->pprev = NULL;
}

static inline int hlist_hashed(const hlist_node_t *n) {
    return (n->pprev != NULL);
}

static inline int hlist_empty(const hlist_t *h) {
    return !h->first;
}

static inline void __hlist_del(hlist_node_t *n) {
    hlist_node_t *next = n->next;
    hlist_node_t **pprev = n->pprev;
    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
}

static inline void hlist_del(hlist_node_t *n) {
    __hlist_del(n);
    n->next = LIST_POISON1;
    n->pprev = LIST_POISON2;
}

static inline void hlist_add_head(hlist_node_t *n, hlist_t *h) {
    hlist_node_t *first = h->first;
    n->next = first;
    if(first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

static inline void hlist_add_before(hlist_node_t *n, hlist_node_t* next) {
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void hlist_add_after(hlist_node_t *n, hlist_node_t *next) {
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if(next->next) 
        next->next->pprev = &next->next;
}

static inline void hlist_move_list(hlist_t *old, hlist_t *n) {
    n->first = old->first;
    if(n->first) {
        n->first->pprev = &n->first;
    }
    old->first = NULL;
}

#define hlist_entry(ptr, type, member) \
    (type *)((unsigned char *)(ptr) - (unsigned char*)(&(((type*)0)->member)))

#define hlist_for_each(hlist, iter) \
    for ((iter) = hlist->first; (iter); (iter) = (iter)->next)

#define hlist_for_each_safe(hlist, iter, next) \
    for ((iter) = (hlist)->first, (next) = ((iter) ? (iter)->next : NULL); \
        (iter); \
        (iter) = (next != NULL ? next : NULL), (next) = ((next != NULL ? (next)->next : NULL)))


#endif /* !HLIST_H */
