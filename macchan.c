/*
 * MACCHANger for learning NIC driver
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/version.h>

static char *mac="01:23:45:67:89:AB";
module_param(mac, charp, S_IRUGO);
MODULE_PARM_DESC(mac, "MAC Address");

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,8,0)
#define __devinit
#define __devexit
#define __devexit_p
#endif

/**
 * struct macchan - driver-tpate device structure
 * @iobase:	pointer to I/O memory region
 * @membase:	pointer to buffer memory region
 * @netdev:	pointer to network device structure
 * @rx_lock:	receive lock
 * @lock:	device lock
 */
struct macchan {
	struct net_device *netdev;
};

static int macchan_open(struct net_device *dev)
{
	struct macchan *tp = netdev_priv(dev);

	return 0;
}

static int macchan_stop(struct net_device *dev)
{
	struct macchan *tp = netdev_priv(dev);

	netif_stop_queue(dev);

	return 0;
}

static struct net_device_stats *macchan_stats(struct net_device *dev)
{
	return &dev->stats;
}

static netdev_tx_t macchan_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	netif_stop_queue(dev);

	dev_kfree_skb(skb);
	dev->trans_start = jiffies;

	return NETDEV_TX_OK;
}

static const struct net_device_ops macchan_netdev_ops = {
	.ndo_open	= macchan_open,
	.ndo_stop	= macchan_stop,
	.ndo_get_stats	= macchan_stats,
	.ndo_start_xmit	= macchan_start_xmit,
};

static int __devinit macchan_probe(struct platform_device *pdev)
{
	struct net_device *netdev = NULL;
	struct macchan *tp = NULL;
	int ret = 0;
	unsigned char macadr[6], *p;
	static int first = 1;

	/* allocate networking device */
	netdev = alloc_etherdev(sizeof(struct macchan));
	if (!netdev) {
		dev_err(&pdev->dev, "cannot allocate network device\n");
		ret = -ENOMEM;
		return ret;
	}

	SET_NETDEV_DEV(netdev, &pdev->dev);
	platform_set_drvdata(pdev, netdev);

	tp = netdev_priv(netdev);
	tp->netdev = netdev;

	/* store MAC address */
	for (p=mac; *p ; ++p) {
		if (*p >= '0' && *p <= '9')
			*p -= '0';
		if (*p >= 'A' && *p <= 'F')
			*p -= 'A' - 10;
		if (*p >= 'a' && *p <= 'f')
			*p -= 'a' - 10;
	}

	macadr[0] = (mac[0] << 4) | mac[1];
	macadr[1] = (mac[3] << 4) | mac[4];
	macadr[2] = (mac[6] << 4) | mac[7];
	macadr[3] = (mac[9] << 4) | mac[10];
	macadr[4] = (mac[12] << 4) | mac[13];
	macadr[5] = (mac[15] << 4) | mac[16];

	memcpy(netdev->dev_addr, macadr, IFHWADDRLEN);

	if (first) {
		printk(KERN_INFO "macchan: HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
		macadr[0], macadr[1], macadr[2], macadr[3], macadr[4], macadr[5] );
		first = 0;
	}

	ether_setup(netdev);

	/* net_device */
	netdev->netdev_ops     = &macchan_netdev_ops;
	netdev->features       = 0;

	/* reset stats */
	memset(&netdev->stats, 0, sizeof(netdev->stats));

	ret = register_netdev(netdev);
	if (ret < 0) {
		dev_err(&netdev->dev, "failed to register interface\n");
		goto error;
	}

	goto out;

error:
	free_netdev(netdev);
out:
	return ret;
}

static void __devexit  macchan_remove(struct platform_device *pdev)
{
	struct net_device *netdev = platform_get_drvdata(pdev);
	struct macchan *tp = netdev_priv(netdev);

	platform_set_drvdata(pdev, NULL);

	if (netdev) {
		unregister_netdev(netdev);
		free_netdev(netdev);
	}

	return 0;
}

static struct platform_driver macchan_driver = {
	.probe   = macchan_probe,
	.remove  = __devexit_p(macchan_remove),
	.driver  = {
		.name = "macchan",
		.owner = THIS_MODULE,
	},
};
MODULE_ALIAS("platform:macchan");

static struct platform_device macchan_device = {
	.name = "macchan",
	.id = 0,
	.num_resources = 0,
};

static int __init macchan_init(void)
{
	platform_device_register(&macchan_device);
	return platform_driver_register(&macchan_driver);
}

static void __exit macchan_exit(void)
{
	platform_driver_unregister(&macchan_driver);
}

module_init(macchan_init);
module_exit(macchan_exit);

MODULE_AUTHOR("macchan@sfc.wide.ad.jp");
MODULE_DESCRIPTION("MACCHANger driver");
MODULE_LICENSE("GPL");
