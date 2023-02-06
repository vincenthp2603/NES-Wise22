#include <stdio.h>
#include <stdlib.h>
#include "aodv_table.h"

static struct aodv_table_entry aodv_routing_table[AODV_TABLE_SIZE];

void aodv_init_table()
{
    int i = 0;
    for(i = 0; i < AODV_TABLE_SIZE; i++)
    {
        aodv_routing_table[i].in_use = 0;
    }
};

void aodv_scroll_up_table_entries(int start_index)
{
    if (!start_index)
    {
        start_index++;
    }
    int i;
    for (i = start_index; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry *curr = &aodv_routing_table[i];    
        struct aodv_table_entry *prev = &aodv_routing_table[i-1];
        prev->start = curr->start;
        prev->hop = curr->hop;
        prev->next_node = curr->next_node;
        prev->in_use = curr->in_use;
    }
};

void aodv_scroll_down_table_entries(int start_index)
{
    if (start_index == AODV_TABLE_SIZE - 1)
    {
        start_index--;
    }
    int i;
    for (i = AODV_TABLE_SIZE - 2; i >= start_index; i--)
    {
        struct aodv_table_entry *curr = &aodv_routing_table[i];    
        struct aodv_table_entry *next = &aodv_routing_table[i+1];
        next->start = curr->start;
        next->hop = curr->hop;
        next->next_node = curr->next_node;
        next->in_use = curr->in_use;
        curr->in_use = 0;
    }
};

void aodv_delete_entry(int start, int next_node)
{
    int to_delete_index = -1;
    int i;
    for (i = 0; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry entry = aodv_routing_table[i];
        if (entry.start == start && entry.next_node == next_node)
        {
            to_delete_index = i;
        }
    }
    if (to_delete_index != -1)
    {
        if (to_delete_index == AODV_TABLE_SIZE - 1)
        {
            aodv_routing_table[to_delete_index].in_use = 0;
            return;
        }
        aodv_scroll_up_table_entries(to_delete_index+1);
    }
}

void aodv_print_table()
{
    printf("----- AODV Routing table ------\n");
    printf("|  Start   |   Hop   |  Next  |\n");
    int i = 0;
    for (i = 0; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry *entry = &aodv_routing_table[i];
        if (!entry->in_use)
        {
            break;
        }
        printf("-------------------------------\n");
        printf("|     %d    |    %d    |    %d   |\n", entry->start, entry->hop, entry->next_node);
    }
    printf("-------------------------------\n\n");
}

struct aodv_table_entry aodv_create_entry(int start, int hop, int next_node)
{
    struct aodv_table_entry entry;
    entry.start = start;
    entry.hop = hop;
    entry.next_node = next_node;
    entry.in_use = 1;
    return entry; 
}

void aodv_copy_entry(struct aodv_table_entry **dest, struct aodv_table_entry src)
{
    (*dest)->start = src.start;
    (*dest)->hop = src.hop;
    (*dest)->next_node = src.next_node;
    (*dest)->in_use = src.in_use;
}

struct aodv_table_entry *aodv_get_best_entry(int start)
{
    int i;
    for (i = 0; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry *entry = &aodv_routing_table[i];
        if (entry->in_use && entry->start == start)
        {
            return entry;
        }
    }
    return NULL;
}

struct aodv_table_entry *aodv_get_best_entry_with_next_node(int start, int next_node)
{
    int i;
    for (i = 0; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry *entry = &aodv_routing_table[i];
        if (entry->in_use && entry->start == start && entry->next_node == next_node)
        {
            return entry;
        }
    }
    return NULL;
}

int aodv_insert_entry(struct aodv_table_entry new_entry)
{
    struct aodv_table_entry *dup_entry = aodv_get_best_entry_with_next_node(new_entry.start, new_entry.next_node);
    if (dup_entry)
    {
        if (dup_entry->hop <= new_entry.hop)
        {
            return 0;
        }
        else
        {
            aodv_delete_entry(dup_entry->start, dup_entry->next_node);
        }
    }
        
    int insert_index = -1;
    int i;
    for (i = 0; i < AODV_TABLE_SIZE; i++)
    {
        struct aodv_table_entry *curr = &aodv_routing_table[i];
        if (!curr->in_use)
        {
            insert_index = i;
            break;
        }
        
        if (curr->start < new_entry.start)
        {
            continue;
        } 
        else if (curr->start == new_entry.start)
        {
            if (curr->hop <= new_entry.hop)
            {
                continue;
            } 
            else
            {
                insert_index = i;
                aodv_scroll_down_table_entries(insert_index);
                break;
            }
        }
        else
        {
            insert_index = i;
            aodv_scroll_down_table_entries(insert_index);
            break;
        }

    }

    if (insert_index != -1)
    {
        struct aodv_table_entry *entry = &aodv_routing_table[insert_index];
        aodv_copy_entry(&entry, new_entry);
        return 1;
    }

    return 0;
}