/**
 * @file linkNode.h
 * @author Bryce Deary (github@bd2357.org)
 * @brief generic link node base class
 * 
 * Copyright 2020, bdPrime
 * 
 */
#ifndef LINKNODE_H
#define LINKNODE_H
#include <config.h>

// a node base class
typedef struct LinkBase_t
{
    struct LinkBase_t * next;
} LinkBase_t;

/**
 * @brief function to push a node onto a stack
 * 
 * @param head - the address of stack head which is a pointer to a node
 * @param node - the address of the node to push
 */
void StackPush(LinkBase_t **head, LinkBase_t *node);

/**
 * @brief function to pop a node off a stack
 * 
 * @param head - the address of stack head which is a pointer to a node
 * @return LinkBase_t* - returns pointer to the popped node or NULL if empty
 */
LinkBase_t *StackPop(LinkBase_t **head);

#endif // LINKNODE_H
