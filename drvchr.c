#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 	//module_init and module_exit
#include <linux/fs.h> 		//register_chrdev and unregister_chrdev
#include <linux/cdev.h> 	//character device stuff
#include <linux/errno.h> 	//error codes

static const char device_name[] = "devchr";
static const char module_name[] = KBUILD_MODNAME;

static int major = 0;
static struct class *device_class;

static char buffer[64];
static char *pbuffer = &buffer[0];
static const int buffer_size = sizeof(buffer);

static int nbytes = 0;


static int device_open(struct inode *inode, struct file *file){
	pr_info("%s: Open device %s\n", module_name, device_name);
	nbytes = buffer_size;
	pbuffer = &buffer[0];
	return 0;
}

static int device_close(struct inode *inode, struct file *file){
	pr_info("%s: Close device %s\n", module_name, device_name);
	nbytes = 0;
	return 0;
}

static ssize_t device_read(struct file *pfile, char __user *buf, size_t len, loff_t *pos){

	ssize_t bytes_read = 0;

	if(!nbytes){
		*pos = 0;
		return 0;
	}

	while(len && nbytes){
		if(!copy_to_user(buf++, pbuffer++, 1)){
			len--;
			nbytes--;
			bytes_read++;
		}
		else{
			pr_err("%s: Fail copy to user space\n", module_name);
			return 0;
		}
	}

	// *pos += bytes_read;

	pr_info("%s: Read %ld bytes\n", module_name, bytes_read);
	return bytes_read;
}

static ssize_t device_write(struct file *pfile, const char __user *buf, size_t len, loff_t *pos){
	ssize_t bytes_write = 0;

	if(len > buffer_size){
		pr_err("%s: buffer size exceeded (max %d bytes)\n", module_name, buffer_size);
		return EINVAL;
	}

	if(!nbytes){
		*pos = 0;
		return 0;
	}

	while(len){
		if(!copy_from_user(pbuffer++, buf++, 1)){
			len--;
			bytes_write++;
		}
		else{
			pr_err("%s: Fail copy from user space\n", module_name);
			return 0;
		}
	}

	pr_info("%s: Write %ld bytes (%s)\n", module_name, bytes_write, buffer);
	return bytes_write;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_close,
};

static int __init init_drv(void)
{
	//pr_info("%s : info\n", __func__);
	//pr_warn("%s : warn\n", __func__);
	//pr_err("%s : error\n", __func__);
	//pr_alert("%s : alert\n", __func__);

	/*if major number is defined and need to get a minor number better to use
	alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);*/
	major = register_chrdev(0, device_name, &fops);

	if(major < 0){
		pr_err("%s: registering failed\n", module_name);
		return major;
	}

	pr_info("%s: registering success (major=%d)\n", module_name, major);

	device_class = class_create(THIS_MODULE, device_name);

	device_create(device_class, NULL, MKDEV(major,0), NULL, device_name);

	pr_info("%s: device created on /dev/%s\n", module_name, device_name);

	return 0;
}

static void  __exit exit_drv(void)
{
	device_destroy(device_class, MKDEV(major,0));
	class_destroy(device_class);
	unregister_chrdev(major, device_name);
	pr_info("%s: unregistering\n", module_name);
	return;
}

module_init(init_drv);
module_exit(exit_drv);

MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");
MODULE_AUTHOR("Sergey Rygin");
MODULE_DESCRIPTION("Character device driver");

