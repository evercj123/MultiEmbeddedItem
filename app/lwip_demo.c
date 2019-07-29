#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/timers.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <stdio.h>


struct netif enc28j60_netif;
extern err_t ethernetif_init(struct netif *netif);
extern void process_mac(void);

void lwip_init_task(void)
{
    struct ip_addr ipaddr,ipmask,gw;

    lwip_init();
    IP4_ADDR(&ipaddr,192,168,1,143);
    IP4_ADDR(&ipmask,255,255,255,0);
    IP4_ADDR(&gw,192,168,1,1);
    
    netif_add(&enc28j60_netif,&ipaddr,&ipmask,&gw,NULL,ethernetif_init,ethernetif_input);
    netif_set_default(&enc28j60_netif);
    netif_set_up(&enc28j60_netif);

}



void lwip_demo(void *pdata)
{

    lwip_init_task();


    while(1)
    {
        process_mac();
        sys_check_timeouts();   
    }
    
}


