#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aodv_message.h"

struct RREQ_message {
    int start;
    int dest;
    int hop_to_start;
};

struct RREP_message {
    int start;
    int dest;
    char* path;
};

struct RREPACK_message {
    int start;
    int dest;
    int rrep_receiver;
};

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
    }
    
    return R_REP_ACK;
}

/*
    RREQ
*/
struct RREQ_message* RREQ_parse(char* msg)
{
    struct RREQ_message* rreq_msg = malloc(sizeof(struct RREQ_message*));
    const char* delim = "_";
    char str[80];
    strcpy(str, msg);

    char *token = strtok(str, delim);
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rreq_msg->start = atoi(token);
                break;
            case 1:
                rreq_msg->dest = atoi(token);
                break;
            default:
                rreq_msg->hop_to_start = atoi(token);
        }
    }
    return rreq_msg;
}

char* RREQ_stringify(struct RREQ_message* msg)
{
    char *msg_str = malloc(80);
    strcat(msg_str, "RREQ_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg->start + 48; // ascii hack
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg->dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    char hop_str[2];
    hop_str[1] = '\0';
    hop_str[0] = (char) msg->hop_to_start + 48;
    strcat(msg_str, hop_str); 

    return msg_str;
}

void RREQ_free(struct RREQ_message* msg)
{
    free(msg);
}

/*
    RREP
*/
struct RREP_message* RREP_parse(char* msg)
{
    
    struct RREP_message* rrep_msg = malloc(sizeof(struct RREP_message*));
    const char* delim = "_";
    char str[80];
    strcpy(str, msg);

    char *token = strtok(str, delim);
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rrep_msg->start = atoi(token);
                break;
            case 1:
                rrep_msg->dest = atoi(token);
                break;
            default:
                rrep_msg->path = malloc(sizeof(token));
                strcpy(rrep_msg->path, token);
        }
    }
    return rrep_msg;
}

char* RREP_stringify(struct RREP_message* msg)
{
    char *msg_str = malloc(80);
    strcat(msg_str, "RREP_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg->start + 48;
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg->dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    strcat(msg_str, msg->path); 

    return msg_str;
}

void RREP_add_node_to_path(struct RREP_message* msg, int node)
{
    if (!msg->path) {
        msg->path = malloc(40);
    }

    char old_path[40];
    char new_path[50];
    
    strcpy(old_path, msg->path);
    new_path[0] = (char) node + 48;
    new_path[1] = '-';
    strcat(new_path, old_path);
    strcpy(msg->path, new_path);
}

void RREP_free(struct RREP_message* msg)
{
    free(msg->path);
    free(msg);
}

/*
    RREPACK
*/
struct RREPACK_message* RREPACK_parse(char* msg)
{
    struct RREPACK_message* rrepack_msg = malloc(sizeof(struct RREPACK_message*));
    const char* delim = "_";
    char str[80];
    strcpy(str, msg);

    char *token = strtok(str, delim);
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, "_");
        switch (i) {
            case 0:
                rrepack_msg->start = atoi(token);
                break;
            case 1:
                rrepack_msg->dest = atoi(token);
                break;
            default:
                rrepack_msg->rrep_receiver = atoi(token);
        }
    }
    return rrepack_msg;
}

char* RREPACK_stringify(struct RREPACK_message* msg)
{
    char *msg_str = malloc(80);
    strcat(msg_str, "RREPACK_");

    char start_str[2];
    start_str[1] = '\0';
    start_str[0] = (char) msg->start + 48; // ascii hack
    strcat(msg_str, start_str);
    strcat(msg_str, "_");    

    char dest_str[2];
    dest_str[1] = '\0';
    dest_str[0] = (char) msg->dest + 48;
    strcat(msg_str, dest_str);
    strcat(msg_str, "_");

    char rrep_receiver_str[2];
    rrep_receiver_str[1] = '\0';
    rrep_receiver_str[0] = (char) msg->rrep_receiver + 48;
    strcat(msg_str, rrep_receiver_str); 

    return msg_str;
}

void RREPACK_free(struct RREPACK_message* msg)
{
    free(msg);
}