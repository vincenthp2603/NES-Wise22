#define R_REQ 0
#define R_REP 1
#define R_REP_ACK 2
#define R_ERR 3

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
};

int parse_message_type(char* msg);

struct RREQ_message RREQ_parse(char* msg);

void RREQ_stringify(struct RREQ_message msg,char *msg_str);

struct RREP_message RREP_parse(char* msg);

void RREP_stringify(struct RREP_message msg, char *msg_str);

// struct RREPACK_message RREPACK_parse(char* msg);

// void RREPACK_stringify(struct RREPACK_message msg, char *msg_str);

void rreq_forward(struct broadcast_conn *c, struct RREQ_message msg);

void rrep_forward(struct unicast_conn *c, struct RREP_message msg);

void rrepack_send(struct unicast_conn *c, const linkaddr_t *rrep_sender);