/* Following sample Code, Shows how to UIO subsystem can engage to Deliver
   hardware interrupt events to user mode applications 
Use case   : modprobe uio 
	   : insmod uioint.ko irq=1 //keyboard
	   : insmod uioint.ko irq=21 //USB keyboard
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uio_driver.h>
#include <linux/mm.h>

#define FB_BASE         0x000A0000
#define FB_SIZE         0x00020000

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

/*  
 * The UIO API
 *
 * In order to write a user-space driver using the UIO API with a
 * small kernel stub driver you'll have to do the following:
 *
 * Allocate space for a uio_info structure, defined
 * in /usr/src/linux/include/linux/uio_driver.h:

 * struct uio_info - UIO device capabilities
 * @uio_dev:            the UIO device this info belongs to
 * @name:               device name
 * @version:            device driver version
 * @mem:                list of mappable memory regions, size==0 for end of list
 * @port:               list of port regions, size==0 for end of list
 * @irq:                interrupt number or UIO_IRQ_CUSTOM
 * @irq_flags:          flags for request_irq()
 * @priv:               optional private data
 * @handler:            the device's irq handler
 * @mmap:               mmap operation for this uio device
 * @open:               open operation for this uio device
 * @release:            release operation for this uio device
 * @irqcontrol:         disable/enable irqs when 0/1 is written to /dev/uioX
 *
 *struct uio_info {
 *        struct uio_device       *uio_dev;
 *        const char              *name;
 *        const char              *version;
 *        struct uio_mem          mem[MAX_UIO_MAPS];
 *        struct uio_port         port[MAX_UIO_PORT_REGIONS];
 *        long                    irq;
 *        unsigned long           irq_flags;
 *        void                    *priv;
 *        irqreturn_t (*handler)(int irq, struct uio_info *dev_info);
 *        int (*mmap)(struct uio_info *info, struct vm_area_struct *vma);
 *        int (*open)(struct uio_info *info, struct inode *inode);
 *        int (*release)(struct uio_info *info, struct inode *inode);
 *        int (*irqcontrol)(struct uio_info *info, s32 irq_on);
 *};
 *
 * You'll need to fill in entries for at least name, irq, irq_flags
 * and handler, which should return IRQ_HANDLED.
 *  
 * The structure should be register and unregistered with:
 *      
 * int uio_register_device(struct device *parent, struct uio_info *info);
 * void uio_unregister_device(struct uio_info *info);
 *
@*/

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

static struct uio_info *info;
static struct device *dev;
static int irq = 1; // 21 
module_param(irq, int, S_IRUGO);

static void uio_release(struct device *dev)
{
	pr_info("%s: Releasing my uio device\n",__func__);
}


static int uio_mmap(struct uio_info *info, struct vm_area_struct *vma)
{
        unsigned long region_origin = vma->vm_pgoff * PAGE_SIZE;
        unsigned long region_length = vma->vm_end - vma->vm_start;
        unsigned long physical_addr = FB_BASE + region_origin;
        unsigned long user_virtaddr = vma->vm_start;

	pr_info("invoked in the context of:%s\n", current->comm);

        /* sanity check: mapped region cannot expend past end of vram */
        if (region_origin + region_length > FB_SIZE)
                return -EINVAL;
        /* set up the page-table entries */
        if (remap_pfn_range(vma, user_virtaddr, physical_addr >> PAGE_SHIFT,
                            region_length, vma->vm_page_prot))
                return -EAGAIN;
        return 0;
}

static irqreturn_t uio_handler(int irq, struct uio_info *dev_info)
{
	static int count = 0;
	pr_info("In UIO handler, count=%d\n", ++count);
	/* must return IRQ_HANDLED for event to reach uspace */
	return IRQ_HANDLED;
}

/**
 * device_register - register a device with the system.
 * @dev: pointer to the device structure
 *
 * This happens in two clean steps - initialize the device
 * and add it to the system. The two steps can be called
 * separately, but this is the easiest and most common.
 * I.e. you should only call the two helpers separately if
 * have a clearly defined need to use and refcount the device
 * before it is added to the hierarchy.
 *
 * For more information, see the kerneldoc for device_initialize()
 * and device_add().
 *
 * NOTE: _Never_ directly free @dev after calling this function, even
 * if it returned an error! Always use put_device() to give up the
 * reference initialized in this function instead.
int device_register(struct device *dev)
{
	device_initialize(dev);
	return device_add(dev);
}
*/

/**
 * uio_register_device - register a new userspace IO device
 * @owner:	module that creates the new device
 * @parent:	parent device
 * @info:	UIO device capabilities
 *
 * returns zero on success or a negative error code.
 */
static int __init uio_init(void)
{
	int retval;
	dev = kzalloc(sizeof(struct device), GFP_KERNEL);
	dev_set_name(dev, "uio_dev");
	dev->release = uio_release;
	retval = device_register(dev);
	if (retval < 0) {
		pr_info("Device registeration Failed\n");
		goto exit;
	}

	info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);

	/* irq information */
	info->name = "uio_device";
	info->version = "0.1";
	info->irq = irq;
	info->irq_flags = IRQF_SHARED;
	info->handler = uio_handler;
	info->mmap = uio_mmap; 
	/*Uio Device registeration*/
	if (uio_register_device(dev, info) < 0) {
		device_unregister(dev);
		kfree(dev);
		kfree(info);
		pr_info("Failing to register uio device\n");
		return -1;
	}
	pr_info("Registered UIO handler for IRQ=%d\n", irq);
	return 0;
exit:
	return retval;
}

static void __exit uio_exit(void)
{
	uio_unregister_device(info);
	device_unregister(dev);
	pr_info("Un-Registered UIO handler for IRQ=%d\n", irq);
	kfree(info);
	kfree(dev);
}

module_init(uio_init);
module_exit(uio_exit);

MODULE_AUTHOR("Chandan Jha <beingChandanjha@gmail.com>");
MODULE_DESCRIPTION("Interrupt communication between userspace to kernel space and momery acees(MMAP) using uio");
MODULE_LICENSE("GPL");
MODULE_VERSION(".1");
