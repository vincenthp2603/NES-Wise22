#define AODV_TABLE_SIZE 50

struct aodv_table_entry {
    int start;
    int hop;
    int next_node;
    int in_use;
};


void aodv_init_table();

void aodv_delete_entry(int start, int next_node);

void aodv_print_table();

struct aodv_table_entry aodv_create_entry(int start, int hop, int next_node);

struct aodv_table_entry *aodv_get_best_entry(int start);

int aodv_insert_entry(struct aodv_table_entry new_entry);