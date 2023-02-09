#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aodv_message.h"
#include "aodv_table.h"

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
    
    return R_REP_ACK;
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
    RREPACK
*/
// struct RREPACK_message RREPACK_parse(char* msg)
// {
//     struct RREPACK_message rrepack_msg;
//     const char* delim = "_";
//     char str[30];
//     strcpy(str, msg);

//     char *token = strtok(str, delim);
//     int i = 0;
//     for (i = 0; i < 2; i++) {
//         token = strtok(NULL, "_");
//         switch (i) {
//             case 0:
//                 rrepack_msg.start = atoi(token);
//                 break;
//             case 1:
//                 rrepack_msg.dest = atoi(token);
//                 break;
//             default:
//                 rrepack_msg.rrep_receiver = atoi(token);
//         }
//     }
//     return rrepack_msg;
// }

// void RREPACK_stringify(struct RREPACK_message msg, char *msg_str)
// {
//     clean_msg_buffer(msg_str);
//     strcat(msg_str, "RREPACK_");

//     char start_str[2];
//     start_str[1] = '\0';
//     start_str[0] = (char) msg.start + 48; // ascii hack
//     strcat(msg_str, start_str);
//     strcat(msg_str, "_");    

//     char dest_str[2];
//     dest_str[1] = '\0';
//     dest_str[0] = (char) msg.dest + 48;
//     strcat(msg_str, dest_str);
//     strcat(msg_str, "_");

//     char rrep_receiver_str[2];
//     rrep_receiver_str[1] = '\0';
//     rrep_receiver_str[0] = (char) msg.rrep_receiver + 48;
//     strcat(msg_str, rrep_receiver_str); 
// }

/*
    RREPACK
*/


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

// void rrep_node_down_handler(void *data)
// {
//     int next_node = *((int*) data);
//     if (neighbor_is_up[next_node])
//     {
//         printf("Node %d seems alive.\n", next_node);
//     }
//     else
//     {
//         printf("Node %d seems dead.\n", next_node);
//     }
// }

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

        unicast_send(c, &addr);

        //neighbor_is_up[entry->next_node] = 0;
        //ctimer_set(&cb_timer, 3 * CLOCK_SECOND, rrep_node_down_handler, &entry->next_node);        
    }
}

void rrepack_send(struct unicast_conn *c, const linkaddr_t *rrep_sender)
{
    packetbuf_copyfrom("RREPACK", 8);
    unicast_send(c, rrep_sender);
}