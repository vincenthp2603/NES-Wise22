#include <stdio.h>
#include <stdlib.h>
#include "aodv_message.h"
#include "aodv_table.h"

int main()
{
    printf("/*--------------------------------*/\n");
    printf("/*       AODV MESSAGE TEST        */\n");
    printf("/*--------------------------------*/\n\n");

    printf("%d\n", parse_message_type("RREQ_1_5_2"));
    printf("%d\n", parse_message_type("RREP_1_5_3"));
    printf("%d\n", parse_message_type("RREPACK_1_5_4"));
    
    struct RREQ_message rreq_msg = RREQ_parse("RREQ_1_5_2");
    struct RREP_message rrep_msg = RREP_parse("RREP_1_5_3");
    struct RREPACK_message rrepack_msg = RREPACK_parse("RREPACK_1_5_4");
    
    char msg_str[30];
    RREQ_stringify(rreq_msg, msg_str);
    printf("%s\n", msg_str);
    RREP_stringify(rrep_msg, msg_str);
    printf("%s\n", msg_str);
    RREPACK_stringify(rrepack_msg, msg_str);
    printf("%s\n", msg_str);

    printf("\n\n");

    printf("/*--------------------------------*/\n");
    printf("/*        AODV TABLE TEST         */\n");
    printf("/*--------------------------------*/\n\n");
    
    aodv_init_table();

    struct aodv_table_entry entry1 = aodv_create_entry(1, 0, 1);
    struct aodv_table_entry entry2 = aodv_create_entry(1, 2, 4);
    struct aodv_table_entry entry3 = aodv_create_entry(1, 2, 3);
    struct aodv_table_entry entry4 = aodv_create_entry(2, 3, 4);
    struct aodv_table_entry entry5 = aodv_create_entry(3, 2, 2);

    aodv_insert_entry(entry3);
    aodv_print_table();
    aodv_insert_entry(entry5);
    aodv_print_table();
    aodv_insert_entry(entry4);
    aodv_print_table();
    aodv_insert_entry(entry1);
    aodv_print_table();
    aodv_insert_entry(entry2);
    aodv_print_table();
    aodv_insert_entry(entry3);
    aodv_print_table();

    printf("\n\n");

    return 0;
}