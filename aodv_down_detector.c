#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned long neighbor_last_heartbeat[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void aodv_node_down_detector_init()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        neighbor_last_heartbeat[i] = clock_seconds();
    }
}

void aodv_node_set_new_checkpoint(int node_id)
{
    neighbor_last_heartbeat[node_id] = clock_seconds();
}

int aodv_node_is_up(int node_id)
{
    unsigned long current_time = clock_seconds();
    if (current_time - neighbor_last_heartbeat[node_id] > 10)
    {
        return 0;
    }
    return 1;
}