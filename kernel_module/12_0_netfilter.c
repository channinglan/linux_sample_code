/*
netfilter�O�@��packet mangling��framework�A
�D�n�bprotocol stack�����Ѥ@��hook point
(NF_IP_PRE_ROUTING�BNF_IP_LOCAL_IN�BNF_IP_FORWARD�BNF_IP_POST_ROUTING�MNF_IP_LOCAL_OUT)�A
��user�i�H�b�o��hook point�W���U�åB����@��hook function�A
�ھ�hook function�Ǧ^�Ӫ��ƭȨӨM�w�O�_�n���(NF_DROP)�Bpass(NF_ACCEPT)�B�Ϊ�queue(NF_QUEUE)�����C

NF_ACCEPT�J continue traversal as normal.
NF_DROP�J   drop the packet; don't continue traversal.
NF_STOLEN:  I've take over the packet; don't continue traversal.
NF_QUEUE�J  queue the packet.
NF_REPEAT�J call this hook again.


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

pf�Oprotocol family�A
�ثe��NFPROTO_UNSPEC�BNFPROTO_IPV4�BNFPROTO_ARP�BNFPROTO_BRIDGE�BNFPROTO_IPV6�MNFPROTO_DECNET�����C
�o�ǭȤ]���Psock��protocol family�C
hooknum�h�O��Jhook point num�Anetfilter�O�@��framework�A�|�b�ܦh�a��]��hook point�A
�ӧڭ̥i�H�ϥ�nf_register_hook()/nf_register_hooks()�N�ڭ̪�hook function���b�o���I�W�A
�HIPv4�ӻ�(�W�����ϥ�)�A�N���ѤF����hook point�A�]�t�G
Hook			Called
NF_IP_PRE_ROUTING	After sanity checks, before routing decisions.
NF_IP_LOCAL_IN		After routing decisions if packet is for this host.
NF_IP_FORWARD		If the packet is destined for another interface.
NF_IP_LOCAL_OUT		For packets coming from local processes on their way out.
NF_IP_POST_ROUTING	Just before outbound packets "hit the wire".

��M���Fnf_register_hook()/nf_register_hooks()���ѵ��U�A�]�|����nf_unregister_hook()/nf_unregister_hooks()��unregister�C
�H�U�N�O�@�Ӥ��O�bIPv4������hook�I�W�A�L�Xsaddr�Mdaddr�C
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
nf_hook_register_hook()���N�O�b�N�n���U��callback function�̷ө��ݪ�protocol family
�H��hooknum���Jstruct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS]�A
�åB�|�̷�priority�Ѥp��j�A

��nf_unregister_hook()�N�O��²�檺reg��nf_hooks�������C

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
