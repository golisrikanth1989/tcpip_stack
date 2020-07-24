/*
 * =====================================================================================
 *
 *       Filename:  layer3.h
 *
 *    Description:  This file defines the routines for Layer 3
 *
 *        Version:  1.0
 *        Created:  Tuesday 24 September 2019 01:17:56  IST
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Networking Developer (AS), sachinites@gmail.com
 *        Company:  Brocade Communications(Jul 2012- Mar 2016), Current : Juniper Networks(Apr 2017 - Present)
 *        
 *        This file is part of the NetworkGraph distribution (https://github.com/sachinites).
 *        Copyright (c) 2017 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by  
 *        the Free Software Foundation, version 3.
 *
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License 
 *        along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */

#ifndef __LAYER3__
#define __LAYER3__

#include <stdint.h>
#include "../tcpconst.h"

#pragma pack (push,1)

/*The Ip hdr format as per the standard specification*/
typedef struct ip_hdr_{

    uint32_t version : 4 ;  /*version number, always 4 for IPv4 protocol*/    
    uint32_t ihl : 4 ;      /*length of IP hdr, in 32-bit words unit. for Ex, if this value is 5, it means length of this ip hdr is 20Bytes*/
    char tos;
    short total_length;         /*length of hdr + ip_hdr payload*/

    /* Fragmentation Related members, we shall not be using below members
     * as we will not be writing fragmentation code. if you wish, take it
     * as a extension of the project*/
    short identification;       
    uint32_t unused_flag : 1 ;
    uint32_t DF_flag : 1;   
    uint32_t MORE_flag : 1; 
    uint32_t frag_offset : 13;  


    char ttl;
    char protocol;
    short checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} ip_hdr_t;

#pragma pack(pop)

static inline void
initialize_ip_hdr(ip_hdr_t *ip_hdr){
    
    ip_hdr->version = 4;
    ip_hdr->ihl = 5; /*We will not be using option field, hence hdr size shall always be 5*4 = 20B*/
    ip_hdr->tos = 0;

    ip_hdr->total_length = 0; /*To be filled by the caller*/

    /*Fragmentation related will not be used
     * int this course, initialize them all to zero*/
    ip_hdr->identification = 0; 
    ip_hdr->unused_flag = 0;
    ip_hdr->DF_flag = 1;
    ip_hdr->MORE_flag = 0;
    ip_hdr->frag_offset = 0;

    ip_hdr->ttl = 64; /*Let us use 64*/
    ip_hdr->protocol = 0; /*To be filled by the caller*/
    ip_hdr->checksum = 0; /*Not used in this course*/
    ip_hdr->src_ip = 0; /*To be filled by the caller*/ 
    ip_hdr->dst_ip = 0; /*To be filled by the caller*/
}
#define IP_HDR_LEN_IN_BYTES(ip_hdr_ptr)  (ip_hdr_ptr->ihl * 4)
#define IP_HDR_TOTAL_LEN_IN_BYTES(ip_hdr_ptr)   (ip_hdr_ptr->total_length * 4)
#define INCREMENT_IPHDR(ip_hdr_ptr) ((char *)ip_hdr_ptr + (ip_hdr_ptr->ihl * 4))
#define IP_HDR_PAYLOAD_SIZE(ip_hdr_ptr) (IP_HDR_TOTAL_LEN_IN_BYTES(ip_hdr_ptr) - \
        IP_HDR_LEN_IN_BYTES(ip_hdr_ptr))
#define IP_HDR_COMPUTE_DEFAULT_TOTAL_LEN(ip_payload_size)  \
    (5 + (short)(ip_payload_size/4) + (short)((ip_payload_size % 4) ? 1 : 0))

#include "../gluethread/glthread.h"

typedef struct rt_table_{

    glthread_t route_list;    
} rt_table_t;

typedef struct nexthop_{

    char gw_ip[16];
    interface_t *oif;
    uint32_t ref_count;
} nexthop_t;

#define nexthop_node_name(nexthop_ptr)  \
   ((get_nbr_node(nexthop_ptr->oif))->node_name)

typedef struct l3_route_{

    char dest[16];  /*key*/
    char mask;      /*key*/
    bool_t is_direct;    /*if set to True, then gw_ip and oif has no meaning*/
    nexthop_t *nexthops[MAX_NXT_HOPS];
    int nxthop_idx;
    glthread_t rt_glue;
} l3_route_t;
GLTHREAD_TO_STRUCT(rt_glue_to_l3_route, l3_route_t, rt_glue);

nexthop_t *
l3_route_get_active_nexthop(l3_route_t *l3_route);

void
init_rt_table(rt_table_t **rt_table);

#if 0
l3_route_t *
rt_table_lookup(rt_table_t *rt_table, char *ip_addr, char mask);
#endif

void
clear_rt_table(rt_table_t *rt_table);

void
delete_rt_table_entry(rt_table_t *rt_table, char *ip_addr, char mask);

void
rt_table_add_route(rt_table_t *rt_table, 
                   char *dst, char mask,
                   char *gw, interface_t *oif);

void
rt_table_add_direct_route(rt_table_t *rt_table,
                          char *dst, char mask);

void
dump_rt_table(rt_table_t *rt_table);

l3_route_t *
l3rib_lookup_lpm(rt_table_t *rt_table,
                 uint32_t dest_ip);

void
tcp_ip_send_ip_data(node_t *node, char *app_data, uint32_t data_size,
                    int L5_protocol_id, uint32_t dest_ip_address);

void
tcp_ip_stack_register_l3_proto_for_l3_hdr_inclusion(
        uint8_t L3_protocol_no);
void
tcp_ip_stack_unregister_l3_proto_for_l3_hdr_inclusion(
        uint8_t L2_protocol_no);

#endif /* __LAYER3__ */
