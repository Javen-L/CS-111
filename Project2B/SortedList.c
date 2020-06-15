//NAME: Dhruv Singhania
//EMAIL: singhania_dhruv@yahoo.com
//ID: 105125631

#include "SortedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  if(list == NULL || element == NULL || list->key != NULL || element->key == NULL) { //invalid list or element
    return;
  }
  if(list->next == NULL) { //empty list
    if(opt_yield & INSERT_YIELD) {
      sched_yield();
    }
    list->next = element;
    list->prev = element;
    element->prev = list;
    element->next = NULL;
  }
  SortedListElement_t *n = list->next;
  while(n != list && strcmp(element->key, n->key) >= 0) { //find element's place in list
    n = n->next;
  }
  SortedListElement_t *p = n->prev;
  if(opt_yield & INSERT_YIELD) {
    sched_yield();
  }
  p->next = element;
  n->prev = element;
  element->prev = p;
  element->next = n;
  return;
}

int SortedList_delete(SortedListElement_t *element) {
  if(element == NULL || element->key == NULL || element->prev->next != element || element->next->prev != element) { //invalid list or element
    return 1;
  }
  if(opt_yield & DELETE_YIELD) {
    sched_yield();
  }
  element->prev->next = element->next;
  element->next->prev = element->prev;
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  if(list == NULL || list->key != NULL) { //invalid list
    return NULL;
  }
  if(key == NULL) { //null key
    if(opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    return list;
  }
  SortedListElement_t *temp = list->next;
  while(temp != list) { //not a null key
    if(opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    if(strcmp(key, temp->key) == 0) {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

int SortedList_length(SortedList_t *list) {
  if(list == NULL || list->key != NULL) { //invalid list
    return -1;
  }
  int counter = 0;
  SortedListElement_t *temp = list->next;
  while(temp != list) {
    if(opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    counter++;
    temp = temp->next;
  }
  return counter;
}