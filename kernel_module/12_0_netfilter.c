/*
netfilter是一個packet mangling的framework，
主要在protocol stack中提供一些hook point
(NF_IP_PRE_ROUTING、NF_IP_LOCAL_IN、NF_IP_FORWARD、NF_IP_POST_ROUTING和NF_IP_LOCAL_OUT)，
讓user可以在這些hook point上註冊並且執行一些hook function，
根據hook function傳回來的數值來決定是否要丟棄(NF_DROP)、pass(NF_ACCEPT)、或者queue(NF_QUEUE)等等。

NF_ACCEPT︰ continue traversal as normal.
NF_DROP︰   drop the packet; don't continue traversal.
NF_STOLEN:  I've take over the packet; don't continue traversal.
NF_QUEUE︰  queue the packet.
NF_REPEAT︰ call this hook again.


struct nf_hook_ops {
	struct list_head 	list;

	// User fills in from here down. 
	nf_hookfn		*hook;
	struct net_device	*dev;
	struct module		*owner;
	void			*priv;
	u_int8_t		pf;
	unsigned int		hooknum;
	// Hooks are ordered in ascending priority. 
	int			priority;
};

pf是protocol family，
目前有NFPROTO_UNSPEC、NFPROTO_IPV4、NFPROTO_ARP、NFPROTO_BRIDGE、NFPROTO_IPV6和NFPROTO_DECNET等等。
這些值也等同sock的protocol family。
hooknum則是填入hook point num，netfilter是一個framework，會在很多地方設立hook point，
而我們可以使用nf_register_hook()/nf_register_hooks()將我們的hook function掛在這些點上，
以IPv4來說(上面的圖示)，就提供了五個hook point，包含：
Hook			Called
NF_IP_PRE_ROUTING	After sanity checks, before routing decisions.
NF_IP_LOCAL_IN		After routing decisions if packet is for this host.
NF_IP_FORWARD		If the packet is destined for another interface.
NF_IP_LOCAL_OUT		For packets coming from local processes on their way out.
NF_IP_POST_ROUTING	Just before outbound packets "hit the wire".

當然有了nf_register_hook()/nf_register_hooks()提供註冊，也會提供nf_unregister_hook()/nf_unregister_hooks()做unregister。
以下就是一個分別在IPv4的五個hook點上，印出saddr和daddr。
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

MODULE_LICENSE("GPL");

inline void dumpIpHdr(const char *fn, const struct sk_buff *skb)
{
    const struct iphdr *ip = ip_hdr(skb);

    printk("%12s, saddr:%pI4, daddr:%pI4\n", fn, &ip->saddr, &ip->daddr);
}

static unsigned int
prerouting(unsigned int hook, struct sk_buff *skb,
        const struct net_device *in, const struct net_device *out,
        int (*okfn)(struct sk_buff*))
{
    dumpIpHdr(__FUNCTION__, skb);
    return NF_ACCEPT;
}

static unsigned int
localin(unsigned int hook, struct sk_buff *skb,
        const struct net_device *in, const struct net_device *out,
        int (*okfn)(struct sk_buff*))
{
    dumpIpHdr(__FUNCTION__, skb);
    return NF_ACCEPT;
}

static unsigned int
localout(unsigned int hook, struct sk_buff *skb,
        const struct net_device *in, const struct net_device *out,
        int (*okfn)(struct sk_buff*))
{
    dumpIpHdr(__FUNCTION__, skb);
    return NF_ACCEPT;
}

static unsigned int
postrouting(unsigned int hook, struct sk_buff *skb,
        const struct net_device *in, const struct net_device *out,
        int (*okfn)(struct sk_buff*))
{
    dumpIpHdr(__FUNCTION__, skb);
    return NF_ACCEPT;
}

static unsigned int
fwding(unsigned int hook, struct sk_buff *skb,
        const struct net_device *in, const struct net_device *out,
        int (*okfn)(struct sk_buff*))
{
    dumpIpHdr(__FUNCTION__, skb);
    return NF_ACCEPT;
}

static struct nf_hook_ops brook_ops[] __read_mostly = {
    {
        .hook = prerouting,
        .pf = PF_INET,
        .hooknum = NF_INET_PRE_ROUTING,
        .priority = NF_IP_PRI_RAW,
        //.owner = THIS_MODULE,
    }, {
        .hook = localin,
        .pf = PF_INET,
        .hooknum = NF_INET_LOCAL_IN,
        .priority = NF_IP_PRI_RAW,
        //.owner = THIS_MODULE,
    }, {
        .hook = fwding,
        .pf = PF_INET,
        .hooknum = NF_INET_FORWARD,
        .priority = NF_IP_PRI_RAW,
        //.owner = THIS_MODULE,
    }, {
        .hook = localout,
        .pf = PF_INET,
        .hooknum = NF_INET_LOCAL_OUT,
        .priority = NF_IP_PRI_RAW,
       // .owner = THIS_MODULE,
    }, {
        .hook = postrouting,
        .pf = PF_INET,
        .hooknum = NF_INET_POST_ROUTING,
        .priority = NF_IP_PRI_RAW,
        //.owner = THIS_MODULE,
    },
};

/*
nf_hook_register_hook()其實就是在將要註冊的callback function依照所屬的protocol family
以及hooknum插入struct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS]，
並且會依照priority由小到大，

而nf_unregister_hook()就是很簡單的reg由nf_hooks中移除。

*/

static int __init init_modules(void)
{
    if (nf_register_hooks(brook_ops, ARRAY_SIZE(brook_ops)) < 0) {
        printk("nf_register_hook failed\n");
    }
    return 0;
}

static void __exit exit_modules(void)
{
    nf_unregister_hooks(brook_ops, ARRAY_SIZE(brook_ops));
}

module_init(init_modules);
module_exit(exit_modules);
