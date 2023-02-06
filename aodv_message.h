#define R_REQ 0
#define R_REP 1
#define R_REP_ACK 2

struct RREQ_message {
    int start;
    int dest;
    int hop_to_start;
};

struct RREP_message {
    int start;
    int dest;
    char path[30];
};

struct RREPACK_message {
    int start;
    int dest;
    int rrep_receiver;
};

int parse_message_type(char* msg);

struct RREQ_message RREQ_parse(char* msg);

void RREQ_stringify(struct RREQ_message msg,char *msg_str);

struct RREP_message RREP_parse(char* msg);

void RREP_stringify(struct RREP_message msg, char *msg_str);

void RREP_add_node_to_path(struct RREP_message *msg, int node);

struct RREPACK_message RREPACK_parse(char* msg);

void RREPACK_stringify(struct RREPACK_message msg, char *msg_str);