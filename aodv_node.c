#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aodv_message.h"
#include "aodv_table.h"

static int start = 1;
static int dest = 6;
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;
//static struct ctimer cb_timer;

static int neighbor_is_up[6] = { 0, 0, 0, 0, 0, 0 };

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) 
{
    struct RREQ_message rreq_msg = RREQ_parse((char *)packetbuf_dataptr());
    int next_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0];
    
    struct aodv_table_entry neighbor_entry = aodv_create_entry(next_node, 0, next_node);
    aodv_insert_entry(neighbor_entry);    

    if (current_node == start)
    {
        return;
    }
    
    if (rreq_msg.hop_to_start >= 6)
    {
        return;
    }

    printf("Broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
    struct aodv_table_entry new_entry = aodv_create_entry(rreq_msg.start, rreq_msg.hop_to_start, from->u8[0]);    
    aodv_insert_entry(new_entry);
 
    if (current_node != dest)
    {
        rreq_forward(c, rreq_msg);       
    }
    else
    {
        aodv_print_table();

        // Start the RREP chain
        struct RREP_message rrep_msg;
        rrep_msg.start = start;
        rrep_msg.dest = dest;
        rrep_msg.hop_to_dest = -1;
       
        rrep_forward(&unicast, rrep_msg);         
    }

}

void rrep_recv(struct unicast_conn *c, const linkaddr_t *from) 
{
    int next_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0]; 

    struct RREP_message rrep_msg = RREP_parse((char *)packetbuf_dataptr());    
    rrepack_send(c, from);

    struct aodv_table_entry new_entry = aodv_create_entry(dest, rrep_msg.hop_to_dest, next_node);
    aodv_insert_entry(new_entry);

    if (current_node != start)
    {
        rrep_forward(c, rrep_msg);
    }
    else
    {
        aodv_print_table();
    }
}

void rrepack_recv(struct unicast_conn *c, const linkaddr_t *from) 
{
    int ack_node = from->u8[0];       
    neighbor_is_up[ack_node] = 1;
}

static void unicast_recv(struct unicast_conn *c, const linkaddr_t *from) 
{
    printf("Unicast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
    int message_type = parse_message_type((char *)packetbuf_dataptr());
    switch(message_type)
    {
        case R_REP:
            rrep_recv(c, from);
            break;
        default:
            rrepack_recv(c, from);
            break;
    }    
}


static const struct broadcast_callbacks broadcast_cb = { broadcast_recv };
static const struct unicast_callbacks unicast_cb = { unicast_recv };

PROCESS(aodv_node, "AODV_NODE");
AUTOSTART_PROCESSES(&aodv_node);

PROCESS_THREAD(aodv_node, ev, data) {
    static struct etimer et;
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();
 
    aodv_init_table();
    broadcast_open(&broadcast, 129, &broadcast_cb);
    unicast_open(&unicast, 146, &unicast_cb);
   
    while(1) 
    {
        /* Delay 2 seconds*/
        unsigned long ticks = CLOCK_SECOND * 2;
        
        etimer_set(&et, ticks);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        if (linkaddr_node_addr.u8[0] == start)
        {
            struct RREQ_message rreq_msg;
            rreq_msg.start = start;
            rreq_msg.dest = dest;
            rreq_msg.hop_to_start = -1;

            rreq_forward(&broadcast, rreq_msg);            
        }
          
    }
    
    PROCESS_END();
}