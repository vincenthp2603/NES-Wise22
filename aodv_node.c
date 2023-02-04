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
static int dest = 5;
//static struct aodv_table *routing_table = aodv_init_table();
static struct broadcast_conn broadcast;

static void rreq_recv(struct broadcast_conn *c, const linkaddr_t *from) 
{
    if (linkaddr_node_addr.u8[0] == start || linkaddr_node_addr.u8[0] == dest)
    {
        return;
    };
    struct RREQ_message *rreq_msg = RREQ_parse((char *)packetbuf_dataptr()); 
    rreq_msg->start = start;
    rreq_msg->dest = dest;
    rreq_msg->hop_to_start = rreq_msg->hop_to_start + 1;

    char* msg_str = RREQ_stringify(rreq_msg);
    packetbuf_copyfrom(msg_str, strlen(msg_str));
    RREQ_free(rreq_msg);
    free(msg_str);

    broadcast_send(c);
    //printf("broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}

static const struct broadcast_callbacks rreq_cb = { rreq_recv };

PROCESS(rreq_process, "RREQ");
AUTOSTART_PROCESSES(&rreq_process);

PROCESS_THREAD(rreq_process, ev, data) {
    static struct etimer et;
    
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();
    
    broadcast_open(&broadcast, 129, &rreq_cb);
    
    while(1) {
        /* Delay 2-4 seconds*/
        unsigned long ticks = CLOCK_SECOND * 2;
        
        etimer_set(&et, ticks);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        
        /* Copy data to the packet buffer */
        if (linkaddr_node_addr.u8[0] == start)
        {
            struct RREQ_message *rreq_msg = malloc(sizeof(struct RREQ_message*));
            rreq_msg->start = start;
            rreq_msg->dest = dest;
            rreq_msg->hop_to_start = 0;

            char* msg_str = RREQ_stringify(rreq_msg);
            packetbuf_copyfrom(msg_str, strlen(msg_str));
            RREQ_free(rreq_msg);
            free(msg_str);
        }       
        
        /* Send broadcast packet */
        broadcast_send(&broadcast);       
    }
    
    PROCESS_END();
}