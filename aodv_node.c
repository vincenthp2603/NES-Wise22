#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aodv_message.h"
#include "aodv_table.h"
#include "aodv_down_detector.h"

static int start = 1;
static int dest = 6;
static struct broadcast_conn broadcast;
static struct broadcast_conn hb_broadcast;
static struct unicast_conn unicast;

static void rreq_recv(struct broadcast_conn *c, const linkaddr_t *from) 
{
    struct RREQ_message rreq_msg = RREQ_parse((char *)packetbuf_dataptr());
    int next_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0];
    aodv_node_set_new_checkpoint(next_node);
    
    struct aodv_table_entry neighbor_entry = aodv_create_entry(next_node, 1, next_node);
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
        rrep_msg.hop_to_dest = 0;
       
        rrep_forward(&unicast, rrep_msg);
    }
}

void heartbeat_recv(struct broadcast_conn *c, const linkaddr_t *from) 
{
    printf("Broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
    int ack_node = from->u8[0];       
    aodv_node_set_new_checkpoint(ack_node);
}

void rrep_recv(struct unicast_conn *c, const linkaddr_t *from) 
{
    int next_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0];
    aodv_node_set_new_checkpoint(next_node);

    struct RREP_message rrep_msg = RREP_parse((char *)packetbuf_dataptr());    

    struct aodv_table_entry new_entry = aodv_create_entry(dest, rrep_msg.hop_to_dest, next_node);
    aodv_insert_entry(new_entry);

    if (current_node != start)
    {
        rrep_forward(c, rrep_msg);
    }
    else
    {
        aodv_delete_entry(dest);
        aodv_insert_entry(new_entry);
        aodv_print_table();
    }
}

void rerr_recv(struct unicast_conn *c, const linkaddr_t *from)
{
    int rerr_sender_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0]; 

    struct RERR_message rerr_msg = RERR_parse((char *)packetbuf_dataptr());

    struct aodv_table_entry *entry = aodv_get_best_entry(rerr_msg.start);

    if (entry && entry->next_node == rerr_sender_node && entry->hop == rerr_msg.hop_to_start + 1)
    {
        rerr_msg.hop_to_start = entry->hop;

        aodv_delete_entry(rerr_msg.start);
        aodv_print_table();

        if (current_node == dest)
        {
            return;
        }

        struct aodv_table_entry *entry_to_dest = aodv_get_best_entry(rerr_msg.dest);
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
            rerr_recv(c, from);
            break;
    }    
}


static const struct broadcast_callbacks broadcast_cb = { rreq_recv };
static const struct broadcast_callbacks hb_broadcast_cb = { heartbeat_recv };
static const struct unicast_callbacks unicast_cb = { unicast_recv };

PROCESS(aodv_node, "AODV_NODE");
AUTOSTART_PROCESSES(&aodv_node);

PROCESS_THREAD(aodv_node, ev, data) {
    static struct etimer et;
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();
 
    aodv_init_table();
    aodv_node_down_detector_init();
    broadcast_open(&broadcast, 129, &broadcast_cb);
    broadcast_open(&hb_broadcast, 12, &hb_broadcast_cb);
    unicast_open(&unicast, 146, &unicast_cb);
    
    volatile int timer_ctr = 0;
    while(1) 
    {
        /* Delay 1 second */
        unsigned long ticks = CLOCK_SECOND * 1;
        
        etimer_set(&et, ticks);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        /* Send RREQ every 2 seconds */
        if (timer_ctr % 2 == 0)
        {
            if (linkaddr_node_addr.u8[0] == start)
            {
                struct RREQ_message rreq_msg;
                rreq_msg.start = start;
                rreq_msg.dest = dest;
                rreq_msg.hop_to_start = 0;

                rreq_forward(&broadcast, rreq_msg);            
            }
        }

        /* Send HEARTBEAT every 1 second*/
        heartbeat_send(&hb_broadcast);

        timer_ctr++;
        if (timer_ctr > 1)
        {
            timer_ctr = 0;
        }
          
    }
    
    PROCESS_END();
}