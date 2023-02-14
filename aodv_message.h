#include "contiki.h"
#define R_REQ 0
#define R_REP 1
#define R_ERR 2
#define HEART_BEAT 3
#define NODE_UP 1
#define NODE_DOWN 0

struct RREQ_message {
    int start;
    int dest;
    int hop_to_start;
};

struct RREP_message {
    int start;
    int dest;
    int hop_to_dest;
};

struct RREPACK_message {
    int start;
    int dest;
};

struct RERR_message {
    int start;
    int dest;
    int hop_to_start;
};

struct rrep_node_down_handler_payload {
    struct unicast_conn *c;
    struct RREP_message rrep_msg;
    int next_node;
};

void set_neighbor_status(int node_id, int status);

int parse_message_type(char* msg);

struct RREQ_message RREQ_parse(char* msg);

void RREQ_stringify(struct RREQ_message msg,char *msg_str);

struct RREP_message RREP_parse(char* msg);

void RREP_stringify(struct RREP_message msg, char *msg_str);

struct RERR_message RERR_parse(char* msg);

void RERR_stringify(struct RERR_message msg, char *msg_str);

void rreq_forward(struct broadcast_conn *c, struct RREQ_message msg);

void rrep_forward(struct unicast_conn *c, struct RREP_message msg);

void heartbeat_send(struct broadcast_conn *c);