#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
typedef struct LinkedPair
{
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable
{
  int capacity;
  LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL)
  {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char *u_str = (unsigned char *)str;

  while ((c = *u_str++))
  {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
  HashTable *ht = malloc(sizeof(HashTable));

  ht->capacity = capacity;
  ht->storage = calloc(capacity, sizeof(LinkedPair *));

  return ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in th existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
  int h_i = hash(key, ht->capacity);

  // if index is open - create new node
  if (ht->storage[h_i] == NULL)
    ht->storage[h_i] = create_pair(key, value);

  // if hashed index is not open
  else
  {
    LinkedPair *node = ht->storage[h_i];
    // scan nodes for matching key
    while (node->next != NULL)
    {
      if (strcmp(node->key, key) == 0)
      {
        node->value = value;
        break;
      }
      node = node->next;
    }

    // last node
    if (strcmp(node->key, key) == 0)
      node->value = value;
    else
      node->next = create_pair(key, value);
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{

  int h_i = hash(key, ht->capacity);
  LinkedPair *node = ht->storage[h_i];

  // accounts for first node match
  while (strcmp(node->key, key) == 0)
  {
    if (node->next == NULL)
    {
      destroy_pair(node);
      ht->storage[h_i] = NULL;
      break;
    }
    else
    {
      LinkedPair *temp = node;
      node = node->next;
      destroy_pair(temp);
    }
  }

  // check index for match
  if (ht->storage[h_i])
  {
    while (node->next != NULL)
    {
      LinkedPair *next_node = node->next;
      if (strcmp(next_node->key, key) == 0)
      {
        // match found
        node->next = next_node->next;
        destroy_pair(next_node);
      }
      else
        node = node->next;
    }
    // last node
    if (strcmp(node->next->key, key) == 0)
    {
      // match found
      destroy_pair(node->next);
      node->next = NULL;
    }
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  int h_i = hash(key, ht->capacity);
  LinkedPair *node = ht->storage[h_i];

  while (node->next != NULL)
  {
    if (strcmp(node->key, key) == 0)
      return node->value;
    else
      node = node->next;
  }

  if (strcmp(node->key, key) == 0)
    return node->value;
  else
    return NULL;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
  for (int i = 0; i < ht->capacity; i++)
  {
    LinkedPair *node = ht->storage[i];
    if (node)
    {
      while (node->next != NULL)
      {
        LinkedPair *temp = node;
        node = node->next;
        destroy_pair(temp);
      }
      destroy_pair(node);
    }
  }
  free(ht->storage);
  free(ht);
}

/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
  HashTable *new_ht = create_hash_table(ht->capacity * 2);

  for (int i = 0; i < ht->capacity; i++)
  {
    LinkedPair *node = ht->storage[i];
    if (node != NULL)
    {
      int new_i = hash(node->key, new_ht->capacity);
      new_ht->storage[new_i] = node;
    }
  }

  free(ht->storage);
  free(ht);
  return new_ht;
}

#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n");
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n");

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));

  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);

  destroy_hash_table(ht);

  return 0;
}
#endif
