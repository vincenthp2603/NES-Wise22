struct aodv_table_entry {
    int start;
    int hop;
    int next_node;
    struct aodv_table_entry *next;
};

struct aodv_table {
    struct aodv_table_entry *head;
};

struct aodv_table_entry;

struct aodv_table;

struct aodv_table* aodv_init_table();

void aodv_print_table(struct aodv_table *table);

struct aodv_table_entry* aodv_create_entry(int start, int hop, int next_node);

struct aodv_table_entry* aodv_get_best_entry(struct aodv_table *table, int start);

struct aodv_table_entry* aodv_get_best_entry_for_next_node(struct aodv_table *table, int start, int next_node);

void aodv_delete_entry(struct aodv_table **table, int start, int next_node);

void aodv_insert_entry(struct aodv_table **table, struct aodv_table_entry *entry);

void aodv_free_table(struct aodv_table *table);