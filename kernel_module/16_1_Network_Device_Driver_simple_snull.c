
/*
http://nano-chicken.blogspot.tw/2016/02/linux-kernel161-network-device-driver.html
這一篇藉由LDD(Linux Device Drivers)中的SNULL來了解最基本的Network Device Driver的架構，
本章的sample code比原本的SNULL更為簡化，但是Network Topology是相同的，
讓interface sn0/sn1可以透過遠方虛擬的remote0/remote1彼此溝通。 
            _________________________
	--> | localhost		 sn0 |<--> snullnet0 ..         	
	|   |                        |		      .     
	--< | localhost          sn1 |<--> snullnet1 ..         
	    |                        |
localnet <->| eth0                   |
            -------------------------
	    
最基本的Network Device Driver的寫法就是allocate network device, 
"struct net_device"並且賦予hook function, "struct net_device_ops"，
然後將該network device註冊到kernel中，Kernel就可以呼叫該Network device，

最基本的net_device_ops包含
    ndo_open() and ndo_validate_addr() are called, when the NIC is bring up.
    ndo_stop() is called, when the NIC is shut down.
    ndo_start_xmit() is called, when a packet is sent from the NIC.
    ndo_change_mtu() is called, when the MTU of the NIC is changed.
    ndo_set_mac_address() is called, when the MAC address of the NIC is changed.


我們是模擬ethernet，而ethernet有一些hook function可以用，如下

    ndo_validate_addr()   -> eth_validate_addr().
    ndo_change_mtu()      -> eth_change_mtu().
    ndo_set_mac_address() -> eth_mac_addr().

在demo code中，ndo_validate_addr()/ndo_change_mtu()/ndo_set_mac_address()
我都是將其轉成ethernet的default hook function，我沒直接掛，是因為我想印出訊息來看 	    
	    
另外幾個比較重要的function是netif_start_queue()/netif_stop_queue()

    netif_start_queue()是通知上層，可以將資料送到該網卡，通常放在ndo_open()裡面
    netif_stop_queue() 是通知上層，停止將資料送到該網卡，通常放在ndo_stop()裡面



由於我們沒有真的remote0/remote1可以回應，所以必須設定flag/IFF_NOARP在sn0跟sn1，
並且自己要處理L2的header，所以必須在額外掛上"struct header_ops"。


 
*/
/* reference ldd3, snull.c */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

/* for in_device, in_ifaddr */
#include <linux/inetdevice.h>

MODULE_AUTHOR("Brook");
MODULE_DESCRIPTION("Kernel module for demo");
MODULE_LICENSE("GPL");

#define MAX_ETH_FRAME_SIZE   1792
struct nic_priv {
    /* you can use array to queue more packet */
    unsigned char *tx_buf;
    unsigned int  tx_len;
    u32           msg_enable;
};

static struct net_device *nic_dev[2];
/* netif msg type, defined in netdevice.h
    NETIF_MSG_DRV           = 0x0001,
    NETIF_MSG_PROBE         = 0x0002,
    NETIF_MSG_LINK          = 0x0004,
    NETIF_MSG_TIMER         = 0x0008,
    NETIF_MSG_IFDOWN        = 0x0010,
    NETIF_MSG_IFUP          = 0x0020,
    NETIF_MSG_RX_ERR        = 0x0040,
    NETIF_MSG_TX_ERR        = 0x0080,
    NETIF_MSG_TX_QUEUED     = 0x0100,
    NETIF_MSG_INTR          = 0x0200,
    NETIF_MSG_TX_DONE       = 0x0400,
    NETIF_MSG_RX_STATUS     = 0x0800,
    NETIF_MSG_PKTDATA       = 0x1000,
    NETIF_MSG_HW            = 0x2000,
    NETIF_MSG_WOL           = 0x4000,
*/
#define DEF_MSG_ENABLE 0xffff

static void dump(unsigned char *buf)
{
    unsigned char *p, sbuf[2*(sizeof(struct ethhdr) + sizeof(struct iphdr))];
    int i;
    p = sbuf;

    for(i = 0; i < sizeof(struct ethhdr); i++) {
        p += sprintf(p, "%02X ", buf[i]);
    }
    printk("eth %s\n", sbuf);

    p = sbuf;
    for(i = 0; i < sizeof(struct iphdr); i++) {
        p += sprintf(p, "%02X ", buf[sizeof(struct ethhdr) + i]);
    }
    printk("iph %s\n", sbuf);

    p = sbuf;
    for(i = 0; i < 4; i++) {
        p += sprintf(p, "%02X ", buf[sizeof(struct ethhdr) + sizeof(struct iphdr) + i]);
    }
    printk("payload %s\n", sbuf);
}


static void
nic_rx(struct net_device *netdev, int len, unsigned char *buf)
{
    struct sk_buff *skb;
    struct nic_priv *priv = netdev_priv(netdev);

    netif_info(priv, hw, netdev, "%s(#%d), rx:%d\n",
                __func__, __LINE__, len);
    /*
     * The packet has been retrieved from the transmission
     * medium. Build an skb around it, so upper layers can handle it
     */
    skb = dev_alloc_skb(len + 2);
    if (!skb) {
        netif_err(priv, rx_err, netdev,
                  "%s(#%d), rx: low on mem - packet dropped\n",
                  __func__, __LINE__);
        netdev->stats.rx_dropped++;
        return;
    }
    skb_reserve(skb, 2); /* align IP on 16B boundary */
    memcpy(skb_put(skb, len), buf, len);

    /* Write metadata, and then pass to the receive level */
    skb->dev = netdev;
    
    //__be16 eth_type_trans(struct sk_buff *skb,struct net_device *dev)
    skb->protocol = eth_type_trans(skb, netdev);
    skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
    netdev->stats.rx_packets++;
    netdev->stats.rx_bytes += len;
    netif_rx(skb);
}

static int nic_open(struct net_device *netdev)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, ifup, netdev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
    /* may be using DMA */
    priv->tx_buf = kmalloc(MAX_ETH_FRAME_SIZE, GFP_KERNEL);
    if (priv->tx_buf == NULL) {
        netif_info(priv, ifup, netdev, "%s(#%d), cannot alloc tx buf\n",
                    __func__, __LINE__);
        return -ENOMEM;
    }
    netif_start_queue(netdev);
    return 0;
}

static int nic_close(struct net_device *netdev)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, ifdown, netdev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
    netif_stop_queue(netdev);
    return 0;
}

static void nic_hw_xmit(struct net_device *netdev)
{
    struct nic_priv *priv = netdev_priv(netdev);
    struct iphdr *iph;
    u32 *saddr, *daddr;
    struct in_device* in_dev;
    struct in_ifaddr* if_info;

    if (priv->tx_len < sizeof(struct ethhdr) + sizeof(struct iphdr)) {
        netif_info(priv, hw, netdev, "%s(#%d), too short\n",
                   __func__, __LINE__);
        return;
    }
    dump(priv->tx_buf);
    iph = (struct iphdr *)(priv->tx_buf + sizeof(struct ethhdr));
    saddr = &iph->saddr;
    daddr = &iph->daddr;

    netif_info(priv, hw, netdev, "%s(#%d), orig, src:%pI4, dst:%pI4, len:%d\n",
                __func__, __LINE__, saddr, daddr, priv->tx_len);

    in_dev = nic_dev[(netdev == nic_dev[0] ? 1 : 0)]->ip_ptr;
    if (in_dev) {
        if_info = in_dev->ifa_list;
        for (if_info = in_dev->ifa_list; if_info; if_info=if_info->ifa_next) {
#if 1
            printk("label:%s, address=%pI4\n",
               if_info->ifa_label, &if_info->ifa_address);
#endif
            *saddr = *daddr = if_info->ifa_address;
            ((u8 *)saddr)[3]++;
            netif_info(priv, hw, netdev, "%s(#%d), new, src:%pI4, dst:%pI4\n",
                        __func__, __LINE__, saddr, daddr);
            break;
        }
        if (!if_info) {
            /* drop packet */
            netdev->stats.tx_dropped++;
            netif_info(priv, hw, netdev, "%s(#%d), drop packet\n",
                        __func__, __LINE__);
            return;
        }
    }

    iph->check = 0;         /* and rebuild the checksum (ip needs it) */
    iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

    netdev->stats.tx_packets++;
    netdev->stats.tx_bytes += priv->tx_len;

    nic_rx(nic_dev[(netdev == nic_dev[0] ? 1 : 0)], priv->tx_len, priv->tx_buf);
}

static netdev_tx_t nic_start_xmit(struct sk_buff *skb,
                                  struct net_device *netdev)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, drv, netdev, "%s(#%d), orig, src:%pI4, dst:%pI4\n",
                __func__, __LINE__, &(ip_hdr(skb)->saddr), &(ip_hdr(skb)->daddr));
    priv->tx_len = skb->len;
    if (likely(priv->tx_len < MAX_ETH_FRAME_SIZE)) {
        if (priv->tx_len < ETH_ZLEN) {
            memset(priv->tx_buf, 0, ETH_ZLEN);
            priv->tx_len = ETH_ZLEN;
        }
        skb_copy_and_csum_dev(skb, priv->tx_buf);
        dev_kfree_skb_any(skb);
    } else {
        dev_kfree_skb_any(skb);
        netdev->stats.tx_dropped++;
        return NETDEV_TX_OK;
    }

    nic_hw_xmit(netdev);
    return NETDEV_TX_OK;
}

static int nic_validate_addr(struct net_device *netdev)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, drv, netdev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
    return eth_validate_addr(netdev);
}

static int nic_change_mtu(struct net_device *netdev, int new_mtu)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, drv, netdev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
    return eth_change_mtu(netdev, new_mtu);
}

static int nic_set_mac_addr(struct net_device *netdev, void *addr)
{
    struct nic_priv *priv = netdev_priv(netdev);
    netif_info(priv, drv, netdev, "%s(#%d), priv:%p\n",
                __func__, __LINE__, priv);
    return eth_mac_addr(netdev, addr);
}

/*
 * This function is called to fill up an eth header, since arp is not
 * available on the interface
 */
int snull_header(struct sk_buff *skb, struct net_device *netdev,
                unsigned short type, const void *daddr, const void *saddr,
                unsigned len)
{
    struct nic_priv *priv = netdev_priv(netdev);
    struct ethhdr *eth = (struct ethhdr *)skb_push(skb, ETH_HLEN);
    struct net_device *dst_netdev;

    netif_info(priv, drv, netdev, "%s(#%d)\n",
                __func__, __LINE__);
    dst_netdev = nic_dev[(netdev == nic_dev[0] ? 1 : 0)];
    eth->h_proto = htons(type);
    memcpy(eth->h_source, saddr ? saddr : netdev->dev_addr, netdev->addr_len);
    memcpy(eth->h_dest, dst_netdev->dev_addr, dst_netdev->addr_len);
    return (netdev->hard_header_len);
}

static const struct header_ops snull_header_ops = {
        .create  = snull_header,
};

static const struct net_device_ops nic_netdev_ops = {
    /* Kernel calls ndo_open() and ndo_validate_addr()
     * when you bring up the NIC
     */
    .ndo_open               = nic_open,
    .ndo_validate_addr      = nic_validate_addr,

    /* when you shut down the NIC, kernel call the .ndo_stop() */
    .ndo_stop               = nic_close,

    /* Kernel calls ndo_start_xmit() when it wants to 
     *   transmit a packet. 
     */
    .ndo_start_xmit         = nic_start_xmit,

    /* ndo_change_mtu() is called, when you change MTU */
    .ndo_change_mtu         = nic_change_mtu,

    /* ndo_set_mac_address() is called,
     *   when you change the MAC addr
     */
    .ndo_set_mac_address    = nic_set_mac_addr,
};

static struct net_device* nic_alloc_netdev(void)
{
    struct net_device *netdev;

    netdev = alloc_etherdev(sizeof(struct nic_priv));
    if (!netdev) {
        pr_err("%s(#%d): alloc dev failed",
               __func__, __LINE__);
        return NULL;
    }
    eth_hw_addr_random(netdev);
    netdev->netdev_ops = &nic_netdev_ops;

    /* keep the default flags, just add NOARP */
    netdev->flags |= IFF_NOARP;

    /* There are no explicit users, so this is 
     *     now equivalent to NETIF_F_HW_CSUM. */
    netdev->features |= NETIF_F_HW_CSUM;

    netdev->header_ops = &snull_header_ops;

    return netdev;
}

static int __init brook_init(void)
{
    int ret;
    struct nic_priv *priv;

    nic_dev[0] = nic_alloc_netdev();
    if (!nic_dev[0]) {
        pr_err("%s(#%d): alloc netdev[0] failed", __func__, __LINE__);
        return -ENOMEM;
    }

    nic_dev[1] = nic_alloc_netdev();
    if (!nic_dev[1]) {
        pr_err("%s(#%d): alloc netdev[1] failed", __func__, __LINE__);
        ret = -ENOMEM;
        goto alloc_2nd_failed;
    }

    ret = register_netdev(nic_dev[0]);
    if (ret) {
        pr_err("%s(#%d): reg net driver failed. ret:%d",
               __func__, __LINE__, ret);
        goto reg1_failed;
    }

    ret = register_netdev(nic_dev[1]);
    if (ret) {
        pr_err("%s(#%d): reg net driver failed. ret:%d",
               __func__, __LINE__, ret);
        goto reg2_failed;
    }

    priv = netdev_priv(nic_dev[0]);
    priv->msg_enable = DEF_MSG_ENABLE;
    priv = netdev_priv(nic_dev[1]);
    priv->msg_enable = DEF_MSG_ENABLE;
    return 0;

reg2_failed:
    unregister_netdev(nic_dev[0]);
reg1_failed:
    free_netdev(nic_dev[1]);
alloc_2nd_failed:
    free_netdev(nic_dev[0]);
    return ret;
}
module_init(brook_init);

static void __exit brook_exit(void)
{
    int i;
    pr_info("%s(#%d): remove module", __func__, __LINE__);
    for (i = 0; i < ARRAY_SIZE(nic_dev); i++) {
        unregister_netdev(nic_dev[i]);
        free_netdev(nic_dev[i]);
    }
}
module_exit(brook_exit);


/*

#insmod 161Network_Device_Driver_simple_snull.ko


#ifconfig eth0 192.168.1.1 up && ifconfig eth1 192.168.2.1 up



#ping 192.168.1.1 -c 1
PING 192.168.1.1 (192.168.1.1) 56(84) bytes of data.
64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=0.022 ms

--- 192.168.1.1 ping statistics ---
1 packets transmitted, 1 received, 0% packet loss, time 0ms
rtt min/avg/max/mdev = 0.022/0.022/0.022/0.000 ms



#ifconfig
.....................

eth0      Link encap:Ethernet  HWaddr 46:48:9e:a5:9e:93
          inet addr:192.168.1.1  Bcast:192.168.1.255  Mask:255.255.255.0
          inet6 addr: fe80::4448:9eff:fea5:9e93/64 Scope:Link
          UP BROADCAST RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:53 errors:0 dropped:0 overruns:0 frame:0
          TX packets:53 errors:0 dropped:107 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:8323 (8.3 KB)  TX bytes:8846 (8.8 KB)

eth1      Link encap:Ethernet  HWaddr 02:e6:05:89:19:cd
          inet addr:192.168.2.1  Bcast:192.168.2.255  Mask:255.255.255.0
          inet6 addr: fe80::e6:5ff:fe89:19cd/64 Scope:Link
          UP BROADCAST RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:53 errors:0 dropped:0 overruns:0 frame:0
          TX packets:53 errors:0 dropped:111 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:8846 (8.8 KB)  TX bytes:8323 (8.3 KB)



[66514.400946] eth0: snull_header(#285)
[66514.400948] eth0: nic_start_xmit(#229), orig, src:0.0.0.0, dst:68.72.158.255
[66514.400950] eth 02 E6 05 89 19 CD 46 48 9E A5 9E 93 86 DD
[66514.400952] iph 60 01 B9 3A 00 35 11 FF FE 80 00 00 00 00 00 00 44 48 9E FF
[66514.400952] payload FE A5 9E 93
[66514.400954] eth0: nic_hw_xmit(#190), orig, src:0.0.0.0, dst:68.72.158.255, len:107
[66514.400954] label:eth1, address=192.168.2.1
[66514.400955] eth0: nic_hw_xmit(#203), new, src:192.168.2.2, dst:192.168.2.1
[66514.400956] eth1: nic_rx(#121), rx:107
[66514.400968] eth1: snull_header(#285)
[66514.400969] eth1: nic_start_xmit(#229), orig, src:0.0.0.0, dst:0.230.5.255
[66514.400971] eth 46 48 9E A5 9E 93 02 E6 05 89 19 CD 86 DD
[66514.400973] iph 60 0F 14 60 00 35 11 FF FE 80 00 00 00 00 00 00 00 E6 05 FF
[66514.400974] payload FE 89 19 CD
[66514.400975] eth1: nic_hw_xmit(#190), orig, src:0.0.0.0, dst:0.230.5.255, len:107
[66514.400975] label:eth0, address=192.168.1.1
[66514.400976] eth1: nic_hw_xmit(#203), new, src:192.168.1.2, dst:192.168.1.1
[66514.400977] eth0: nic_rx(#121), rx:107




*/