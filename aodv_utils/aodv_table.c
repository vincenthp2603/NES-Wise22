#include <stdio.h>
#include <stdlib.h>
#include "aodv_table.h"

struct aodv_table_entry {
    int start;
    int hop;
    int next_node;
    struct aodv_table_entry *next;
};

struct aodv_table {
    struct aodv_table_entry *head;
};

struct aodv_table* aodv_init_table()
{
    struct aodv_table *table = malloc(sizeof(struct aodv_table*));
    table->head = NULL;
    return table;
};

void aodv_print_table(struct aodv_table *table)
{
    printf("----- AODV Routing table ------\n");
    printf("|  Start   |   Hop   |  Next  |\n");
    struct aodv_table_entry *ptr = table->head;
    while (ptr != NULL)
    {
        printf("-------------------------------\n");
        printf("|     %d    |    %d    |    %d   |\n", ptr->start, ptr->hop, ptr->next_node);
        ptr = ptr->next;
    }
    printf("-------------------------------\n\n");
}

struct aodv_table_entry* aodv_create_entry(int start, int hop, int next_node)
{
    struct aodv_table_entry *entry = malloc(sizeof(struct aodv_table_entry*));
    entry->start = start;
    entry->hop = hop;
    entry->next_node = next_node;
    return entry; 
}

struct aodv_table_entry* aodv_get_best_entry(struct aodv_table *table, int start)
{
    struct aodv_table_entry *ptr = table->head;
    while (ptr != NULL)
    {
        if (ptr->start == start)
        {
            break;
        }
        ptr = ptr->next;
    }
    return ptr;
}

struct aodv_table_entry* aodv_get_best_entry_for_next_node(struct aodv_table *table, int start, int next_node)
{
    struct aodv_table_entry *ptr = table->head;
    while (ptr != NULL)
    {
        if (ptr->start == start && ptr->next_node == next_node)
        {
            break;
        }
        ptr = ptr->next;
    }
    return ptr;
}

void aodv_delete_entry(struct aodv_table **table, int start, int next_node)
{
    struct aodv_table_entry *ptr = (*table)->head;
    if (ptr && ptr->start == start && ptr->next_node == next_node)
    {
        (*table)->head = ptr->next;
        free(ptr);
        return;
    }
    while (ptr != NULL)
    {
        if (ptr->next && ptr->next->start == start && ptr->next->next_node == next_node)
        {
            struct aodv_table_entry *to_delete = ptr->next;
            ptr->next = to_delete->next;
            free(to_delete);
            return;
        }
        ptr = ptr->next;   
    }
}

void aodv_insert_entry(struct aodv_table **table, struct aodv_table_entry *entry)
{
    struct aodv_table_entry *dup_entry = aodv_get_best_entry_for_next_node(*table, entry->start, entry->next_node);
    if (dup_entry)
    {
        if (dup_entry->hop <= entry->hop)
        {
            return;
        } else
        {
            aodv_delete_entry(table, dup_entry->start, dup_entry->next_node);
        }
    }
    
    struct aodv_table_entry *head = (*table)->head; 
    if (!head || entry->start < head->start || (entry->start == head->start && entry->hop < head->hop))
    {
        entry->next = head;
        (*table)->head = entry;
        return;
    }

    struct aodv_table_entry *ptr = (*table)->head;

    while (entry->start > ptr->start)
    {
        if (!ptr->next) 
        {
            ptr->next = entry;
            return;
        } else if (ptr->next->start > entry->start)
        {
            entry->next = ptr->next;
            ptr->next = entry;
            return;
        } else if (ptr->next->start == entry->start && ptr->next->start != ptr->start)
        {
            break;
        }
        ptr = ptr->next;
        continue;
    }

    if (!ptr->next || (ptr->next->hop >= entry->hop))
    {
        entry->next = ptr->next;
        ptr->next = entry;
        return;        
    }

    while (entry->hop >= ptr->hop)
    {
        if (!ptr->next) 
        {
            ptr->next = entry;
            return;
        } else if ((ptr->next->hop >= entry->hop && ptr->start == entry->start) || ptr->next->start != entry->start)
        {
            entry->next = ptr->next;
            ptr->next = entry;
            return;
        }
        ptr = ptr->next;
        continue;
    }
}

void aodv_free_table(struct aodv_table *table)
{
    struct aodv_table_entry *ptr = table->head;
    while(ptr != NULL)
    {
        struct aodv_table_entry *to_free = ptr;
        ptr = ptr->next;
        free(to_free);
    }
    free(table);
}