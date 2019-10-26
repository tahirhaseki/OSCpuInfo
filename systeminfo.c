#include <linux/init.h> 
#include <linux/module.h> //try_module_get, module_put
#include <linux/kernel.h>
#include <linux/fs.h> //file_operations
#include <asm/uaccess.h> //memory access
#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DEUCENG");
MODULE_DESCRIPTION("An example Linux kernel module.");
MODULE_VERSION("0.01");

struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

int file_close(struct file *file) 
{
    filp_close(file, NULL);
    return 0;
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_sync(struct file *file) 
{
    vfs_fsync(file, 0);
    return 0;
}
struct file * f;

void cpuinfo(void){
    char in[500];
    char *tok;
    char *end;
	int counter;

    // Create and open the file in user space
    f = file_open("/proc/cpuinfo", O_CREAT |  O_RDWR | O_APPEND, \
                                		S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,in,500);
    printk("file = %s\n",in);
    printk("end\n");

	counter = 0;
    tok = in;
    end = in;
    while (counter < 7){
		strsep(&end,"\n");
		if(counter != 3 && counter != 5){
			printk("xx| %s\n",tok);
		}
		tok = end;
		counter++;
    }
    file_close(f);
}

static int __init file_init(void) { 
	cpuinfo();
    return 0;
}

static void __exit file_exit(void){
    printk("Exiting");
}


module_init(file_init);
module_exit(file_exit);
