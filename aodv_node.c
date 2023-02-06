#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aodv_message.h"
#include "aodv_table.h"

static int start = 1;
static int dest = 6;
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;

static void rreq_recv(struct broadcast_conn *c, const linkaddr_t *from) 
{
    int next_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0]; 
    if (current_node == start)
    {
        return;
    }
    struct RREQ_message rreq_msg = RREQ_parse((char *)packetbuf_dataptr());    

    struct aodv_table_entry new_entry = aodv_create_entry(start, rreq_msg.hop_to_start, next_node);

    if (rreq_msg.hop_to_start >= 10)
    {
        return;
    }

    if (aodv_insert_entry(new_entry))
    {
        printf("Broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
        aodv_print_table();

        rreq_msg.hop_to_start = rreq_msg.hop_to_start + 1;

        char msg_str[30];
        RREQ_stringify(rreq_msg, msg_str);
        packetbuf_copyfrom(msg_str, strlen(msg_str));

        if (current_node != dest)
        {
            broadcast_send(c);
        }
    }
}

static void rrep_recv(struct unicast_conn *c, const linkaddr_t *from) 
{
    //int prev_node = from->u8[0];
    int current_node = linkaddr_node_addr.u8[0]; 

    struct RREP_message rrep_msg = RREP_parse((char *)packetbuf_dataptr());    

    printf("Unicast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

    /*if (current_node != start)
    {
        struct aodv_table_entry *entry = aodv_get_best_entry(start);

        if (entry)
        {
            linkaddr_t addr;
            addr.u8[0] = entry->next_node;
            addr.u8[1] = 0;

            RREP_add_node_to_path(&rrep_msg, current_node);

            char msg_str[30]; 
            RREP_stringify(rrep_msg, msg_str);
            packetbuf_copyfrom(msg_str, strlen(msg_str)+1);

            unicast_send(c, &addr);
            return;
        }
    }
    else
    {
        RREP_add_node_to_path(&rrep_msg, start);
        printf("The path from %d to %d is: %s\n", start, dest, rrep_msg.path);
    }*/    
}

static const struct broadcast_callbacks rreq_cb = { rreq_recv };
static const struct unicast_callbacks rrep_cb = { rrep_recv };

PROCESS(aodv_node, "AODV_NODE");
AUTOSTART_PROCESSES(&aodv_node);

PROCESS_THREAD(aodv_node, ev, data) {
    static struct etimer et;
    
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();

    aodv_init_table();
    broadcast_open(&broadcast, 129, &rreq_cb);
    unicast_open(&unicast, 146, &rrep_cb);
    
    while(1) {
        /* Delay 2 seconds*/
        unsigned long ticks = CLOCK_SECOND * 2;
        
        etimer_set(&et, ticks);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        
        /* Copy data to the packet buffer */
        if (linkaddr_node_addr.u8[0] == start)
        {
            struct RREQ_message rreq_msg;
            rreq_msg.start = start;
            rreq_msg.dest = dest;
            rreq_msg.hop_to_start = 0;

            char msg_str[30];
            RREQ_stringify(rreq_msg, msg_str);
            packetbuf_copyfrom(msg_str, strlen(msg_str)+1);

            /* Send broadcast packet */
            broadcast_send(&broadcast);
        }

        if (linkaddr_node_addr.u8[0] == dest)
        {
            struct aodv_table_entry *entry = aodv_get_best_entry(start);

            if (entry)
            {
                linkaddr_t addr;
                addr.u8[0] = entry->next_node;
                addr.u8[1] = 0;

                struct RREP_message rrep_msg;
                rrep_msg.start = start;
                rrep_msg.dest = dest;
                rrep_msg.path[0] = '\0';

                RREP_add_node_to_path(&rrep_msg, dest);

                char msg_str[30]; 
                RREP_stringify(rrep_msg, msg_str);
                packetbuf_copyfrom(msg_str, strlen(msg_str)+1);

                unicast_send(&unicast, &addr);
            }
        }           
    }
    
    PROCESS_END();
}