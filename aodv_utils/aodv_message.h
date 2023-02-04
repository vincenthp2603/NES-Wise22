#define R_REQ 0
#define R_REP 1
#define R_REP_ACK 2

struct RREQ_message;

struct RREP_message;

struct RREPACK_message;

int parse_message_type(char* msg);

struct RREQ_message* RREQ_parse(char* msg);

char* RREQ_stringify(struct RREQ_message* msg);

void RREQ_free(struct RREQ_message* msg);

struct RREP_message* RREP_parse(char* msg);

char* RREP_stringify(struct RREP_message* msg);

void RREP_add_node_to_path(struct RREP_message* msg, int node);

void RREP_free(struct RREP_message* msg);

struct RREPACK_message* RREPACK_parse(char* msg);

char* RREPACK_stringify(struct RREPACK_message* msg);

void RREPACK_free(struct RREPACK_message* msg);