#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aodv_message.h"
#include "aodv_table.h"
#include "aodv_down_detector.h"

void clean_msg_buffer(char *msg)
{
    int i;
    for (i = 0; i < 30; i++)
    {
        msg[i] = 0;
    }
}

/*
    Identify the message type from the message string
*/
int parse_message_type(char* msg)
{
    char buffer[30];
    strcpy(buffer, msg);
    char *msg_type_str = strtok(buffer, "_");
    if (strcmp(msg_type_str, "RREQ") == 0) {
        return R_REQ;
    } else if (strcmp(msg_type_str, "RREP") == 0) {
        return R_REP;
    } else if (strcmp(msg_type_str, "RERR") == 0) {
        return R_ERR;
    }
    
    return HEART_BEAT;
}

/*
    RREQ
*/
struct RREQ_message RREQ_parse(char* msg)
{
    struct RREQ_message rreq_msg;
    const char* delim = "_";
    char str[30];
    strcpy(str, msg);

    int i = 0;
    char *token = strtok(str, delim);
    for (i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rreq_msg.start = atoi(token);
                break;
            case 1:
                rreq_msg.dest = atoi(token);
                break;
            default:
                rreq_msg.hop_to_start = atoi(token);
        }
    }
    return rreq_msg;
}

void RREQ_stringify(struct RREQ_message msg,char *msg_str)
{
    clean_msg_buffer(msg_str);
    strcat(msg_str, "RREQ_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg.start + 48; // ascii hack
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg.dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    char hop_str[2];
    hop_str[1] = '\0';
    hop_str[0] = (char) msg.hop_to_start + 48;
    strcat(msg_str, hop_str); 
}

/*
    RREP
*/
struct RREP_message RREP_parse(char* msg)
{
    
    struct RREP_message rrep_msg;
    const char* delim = "_";
    char str[30];
    strcpy(str, msg);

    char *token = strtok(str, delim);
    int i = 0;
    for (i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rrep_msg.start = atoi(token);
                break;
            case 1:
                rrep_msg.dest = atoi(token);
                break;
            default:
                rrep_msg.hop_to_dest = atoi(token);
        }
    }
    return rrep_msg;
}

void RREP_stringify(struct RREP_message msg, char *msg_str)
{
    clean_msg_buffer(msg_str);
    strcat(msg_str, "RREP_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg.start + 48;
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg.dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    char hop_str[2];
    hop_str[1] = '\0';
    hop_str[0] = (char) msg.hop_to_dest + 48;
    strcat(msg_str, hop_str); 
}

/*
    RERR
*/
struct RERR_message RERR_parse(char* msg)
{
    struct RERR_message rerr_msg;
    const char* delim = "_";
    char str[30];
    strcpy(str, msg);

    char *token = strtok(str, delim);
    int i = 0;
    for (i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rerr_msg.start = atoi(token);
                break;
            case 1:
                rerr_msg.dest = atoi(token);
                break;
            default:
                rerr_msg.hop_to_start = atoi(token);
        }
    }
    return rerr_msg;
}

void RERR_stringify(struct RERR_message msg, char *msg_str)
{
    clean_msg_buffer(msg_str);
    strcat(msg_str, "RERR_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg.start + 48; // ascii hack
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg.dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    char rrep_receiver_str[2];
    rrep_receiver_str[1] = '\0';
    rrep_receiver_str[0] = (char) msg.hop_to_start + 48;
    strcat(msg_str, rrep_receiver_str); 
}

/*
    UTILS FUNCTIONS
*/
void rreq_forward(struct broadcast_conn *c, struct RREQ_message msg)
{
    aodv_print_table();
    msg.hop_to_start = msg.hop_to_start + 1;

    char msg_str[30];
    RREQ_stringify(msg, msg_str);
    packetbuf_copyfrom(msg_str, strlen(msg_str));

    broadcast_send(c);
}

void rrep_node_down_handler(struct unicast_conn *c, struct RREP_message rrep_msg, int down_node)
{

    struct aodv_table_entry *entry = aodv_get_best_entry(rrep_msg.start);
    if (entry && entry->next_node == down_node)
    {
        struct RERR_message rerr_msg;
        rerr_msg.start = rrep_msg.start;
        rerr_msg.dest = rrep_msg.dest;
        rerr_msg.hop_to_start = entry->hop;

        aodv_delete_entry(rrep_msg.start);

        int current_node = linkaddr_node_addr.u8[0];
        if (current_node == rrep_msg.dest)
        {
            aodv_delete_entry(down_node);
            return;
        }

        struct aodv_table_entry *entry_to_dest = aodv_get_best_entry(rrep_msg.dest);
        if (entry_to_dest)
        {
            char msg_str[30];
            RERR_stringify(rerr_msg, msg_str);

            linkaddr_t addr;
            addr.u8[0] = entry_to_dest->next_node;
            addr.u8[1] = 0;
            packetbuf_copyfrom(msg_str, strlen(msg_str)+1);
            
            unicast_send(c, &addr);
        } 
    }

    aodv_delete_entry(down_node);
}

void rrep_forward(struct unicast_conn *c, struct RREP_message msg)
{
    int start = msg.start;
    struct aodv_table_entry *entry = aodv_get_best_entry(start);

    if (entry)
    {
        linkaddr_t addr;
        addr.u8[0] = entry->next_node;
        addr.u8[1] = 0;

        msg.hop_to_dest = msg.hop_to_dest + 1;

        char msg_str[30];
        RREP_stringify(msg, msg_str);
        packetbuf_copyfrom(msg_str, strlen(msg_str)+1);

        if (aodv_node_is_up(entry->next_node))
        {
            printf("Node %d seems alive.\n", entry->next_node);
            unicast_send(c, &addr);
        }
        else
        {
            printf("Node %d seems dead.\n", entry->next_node);
            rrep_node_down_handler(c, msg, entry->next_node);
        }
    }
}

void heartbeat_send(struct broadcast_conn *c)
{
    packetbuf_copyfrom("HEARTBEAT", 10);
    broadcast_send(c);
}