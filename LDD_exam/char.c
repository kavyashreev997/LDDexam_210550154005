#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include<linux/semaphore.h>
#include<linux/delay.h>

char k_buff[50]="this is kernel";
struct semaphore sem_dev;
int open_dev1(struct inode *inode, struct file *file_op_dev1);
int release_dev1(struct inode *inode, struct file *file_op_dev1);
ssize_t write_dev1(struct file *file_op_dev1,const char __user *u_buff,size_t count,loff_t *offp );
ssize_t read_dev1(struct file *file_op_dev1,char __user *u_buff, size_t count, loff_t *offp);

//static int add_result; 

struct file_operations fop_dev1=
{
	.owner = THIS_MODULE,
	.open = open_dev1,
	.read = read_dev1,
	.write = write_dev1,
	.release = release_dev1,
};

int open_dev1(struct inode *inode, struct file *file_op_dev1)
{
	printk("dev1 is opened...\n");
	return 0;
}

ssize_t read_dev1(struct file *file_op_dev1,char __user *u_buff,size_t count,loff_t *offp)
{
	    int result;
        ssize_t retval;
	
	if(down_interruptible(&sem_dev))
        {       
              return -ERESTARTSYS;
        }
        result = copy_to_user((char*)u_buff,(char *)k_buff,sizeof(k_buff));

        up(&sem_dev);
        
        if(result == 0)
        {

                printk("copy to user successfull data written \n");
                retval = sizeof(k_buff);
                return count;
        }
        else
        {
                printk(KERN_ALERT"ERROR writting data to user\n");
                retval = -EFAULT;
                return retval;
        }
        
}


ssize_t write_dev1(struct file *file_op_add, const char __user *u_buff, size_t count, loff_t *offp)
{
	 int result;
        ssize_t retval;
  
  	if(down_interruptible(&sem_dev))
        {
             return -ERESTARTSYS;
        }

        result = copy_from_user((char*)k_buff,(char*)u_buff,count);
        
        up(&sem_dev);
        if(result==0)
        {
                printk("Message from user : %s\n",k_buff);
                printk("data written succesfully \n");
                retval = count;
                return count;
        }
        else
        {
                printk("ERROR writing data\n");
                retval = -EFAULT;
                return retval;
        }

	
	
}

int release_dev1(struct inode *inode, struct file *file_op_add)
{
	printk("dev1 is released...\n");
	return 0;
}

int open_dev2(struct inode *inode, struct file *file_op_dev2);
int release_dev2(struct inode *inode, struct file *file_op_dev2);
ssize_t write_dev2(struct file *file_op_sub,const char __user *u_buff,size_t count, loff_t *offp);
ssize_t read_dev2(struct file *file_op_sub, char __user *u_buff, size_t count, loff_t *offp);

//static int sub_result;

struct file_operations fop_dev2=
{
	.owner = THIS_MODULE,
	.open = open_dev2,
	.read = read_dev2,
	.write = write_dev2,
	.release = release_dev2,
};

int open_dev2(struct inode *inode, struct file *file_op_sub)
{
	printk("dev2 is opened...\n");
	return 0;
}

ssize_t write_dev2(struct file *file_op_sub, const char __user *u_buff, size_t count,loff_t *offp)
{
		 int result;
        ssize_t retval;
  
  	if(down_interruptible(&sem_dev))
        {
            return -ERESTARTSYS;
        }
        result = copy_from_user((char*)k_buff,(char*)u_buff,count);
        up(&sem_dev);
        
        if(result==0)
        {
                printk("Message from user : %s\n",k_buff);
                printk("data written succesfully \n");
                retval = count;
                return count;
        }
        else
        {
                printk("ERROR writing data\n");
                retval = -EFAULT;
                return retval;
        }

	
	
}

ssize_t read_dev2(struct file *file_op_sub, char __user *u_buff, size_t count, loff_t *offp)
{
	if(down_interruptible(&sem_dev))
        {
                 return -ERESTARTSYS;
        }
         int result;
        ssize_t retval;

        result = copy_to_user((char*)u_buff,(char *)k_buff,sizeof(k_buff));

         up(&sem_dev);
        if(result == 0)
        {

                printk("copy to user successfull  data written \n");
                retval = sizeof(k_buff);
                return count;
        }
        else
        {
                printk("ERROR writting data to user\n");
                retval = -EFAULT;
                return retval;
        }
       
}


int release_dev2(struct inode *inode, struct file *file_op_sub)
{
	printk("dev2 is released...\n");
	return 0;
}

struct cdev *Dev1;
struct cdev *Dev2;


dev_t my_device_number;
dev_t mdn_dev1;
dev_t mdn_dev2;

//module initialization
static int initmod(void) 
{
	int on_success;
	int major;
	int result_dev1;
	int result_dev2;
	on_success = alloc_chrdev_region(&my_device_number, 0,2,"MyCalDriver"); 

	if (on_success < 0) 
	{
		printk("not successfully allocated region\n");
		return 0;
	}
        
	major = MAJOR(my_device_number); 
	printk("the allocated driver having the major number: %d\n\nminor no 0: for Dev1 device\nminor no 1: for Dev2 device\n",major);
	
	
	
	mdn_dev1 = MKDEV(major,0);
	mdn_dev2 = MKDEV(major,1);

	Dev1 = cdev_alloc(); 
	Dev1->ops = &fop_dev1;
	
	Dev2 = cdev_alloc();
	Dev2->ops = &fop_dev2;

	result_dev1 = cdev_add(Dev1,mdn_dev1,1); 
	if(result_dev1 <0 ) 
	{
		printk("driver not allocated by kernel\n");
		return -1;
	}

	result_dev2 = cdev_add(Dev2,mdn_dev2,1);
	if(result_dev2 <0)
	{
		printk("driver not allocated by kernel\n");
		return -1;

	}
	printk("Driver inserted successfully...");
	sema_init(&sem_dev,1);
	return 0;
}
//module exit
static void exitmod(void)
{
	printk("removing drivers...\n");
	
	cdev_del(Dev1);
	cdev_del(Dev2);
	
	
	unregister_chrdev_region(my_device_number, 2);
	printk("drivers are removed successfully...\n");
}

module_init(initmod);
module_exit(exitmod);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("KAVYASHREE");
MODULE_DESCRIPTION("Semaphore usage");

