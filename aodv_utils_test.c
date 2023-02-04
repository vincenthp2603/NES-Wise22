#include <stdio.h>
#include <stdlib.h>
#include "./aodv_utils/aodv_message.h"
#include "./aodv_utils/aodv_table.h"

int main()
{
    printf("/*--------------------------------*/\n");
    printf("/*       AODV MESSAGE TEST        */\n");
    printf("/*--------------------------------*/\n\n");

    printf("%d\n", parse_message_type("RREQ_1_5_2"));
    printf("%d\n", parse_message_type("RREP_1_5_3-5"));
    printf("%d\n", parse_message_type("RREPACK_1_5_4"));
    
    struct RREQ_message* rreq_msg = RREQ_parse("RREQ_1_5_2");
    struct RREP_message* rrep_msg = RREP_parse("RREP_1_5_3-5");
    struct RREPACK_message* rrepack_msg = RREPACK_parse("RREPACK_1_5_4");
    RREP_add_node_to_path(rrep_msg, 2);

    printf("%s\n", RREQ_stringify(rreq_msg));
    printf("%s\n", RREP_stringify(rrep_msg));
    printf("%s\n", RREPACK_stringify(rrepack_msg));

    RREQ_free(rreq_msg);
    RREP_free(rrep_msg);
    RREPACK_free(rrepack_msg);

    printf("\n\n");

    printf("/*--------------------------------*/\n");
    printf("/*        AODV TABLE TEST         */\n");
    printf("/*--------------------------------*/\n\n");
    
    struct aodv_table *routing_table = aodv_init_table();

    struct aodv_table_entry *entry1 = aodv_create_entry(1, 1, 2);
    struct aodv_table_entry *entry2 = aodv_create_entry(1, 4, 2);
    struct aodv_table_entry *entry3 = aodv_create_entry(1, 2, 3);
    struct aodv_table_entry *entry4 = aodv_create_entry(2, 3, 4);
    struct aodv_table_entry *entry5 = aodv_create_entry(3, 2, 2);

    aodv_insert_entry(&routing_table, entry3);
    aodv_insert_entry(&routing_table, entry5);
    aodv_insert_entry(&routing_table, entry4);
    aodv_insert_entry(&routing_table, entry1);
    aodv_insert_entry(&routing_table, entry2);

    aodv_print_table(routing_table);

    aodv_free_table(routing_table);

    printf("\n\n");

    return 0;
}