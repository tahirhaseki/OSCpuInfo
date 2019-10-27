#include <linux/init.h> 
#include <linux/module.h> //try_module_get, module_put
#include <linux/kernel.h>
#include <linux/fs.h> //file_operations
#include <asm/uaccess.h> //memory access
#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/string.h>
#include <linux/syscalls.h>

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

	printk("CPU Information:\n");
    // Create and open the file in user space
    f = file_open("/proc/cpuinfo", O_CREAT |  O_RDWR | O_APPEND, \
                                		S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,in,500);


	counter = 0;
    tok = in;
    end = in;
    while (counter < 7){
		strsep(&end,"\n");
		if(counter != 3 && counter != 5 && counter != 0){
			printk("xx| %s\n",tok);
		}
		tok = end;
		counter++;
    }
    file_close(f);
}

void param1(char* pid){
    char in[500];
    char *tok;
    char *end;
	int counter;
	char fp[50] = "";

	strncat(fp,"/proc/",strnlen("/proc/",10));
	strncat(fp,pid,strnlen(pid,8));
	strncat(fp,"/status",strnlen("/status",10));

	printk("Process Information:\n");
    // Create and open the file in user space
    f = file_open(fp, O_CREAT |  O_RDWR | O_APPEND, \
                                		S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,in,500);

	counter = 0;
    tok = in;
    end = in;
    while (counter < 9){
		strsep(&end,"\n");
		if(counter != 1 && counter != 3 && counter != 4 && counter != 7) {
			printk("xx| %s\n",tok);
		}
		tok = end;
		counter++;
    }
    file_close(f);
}

void param2(void){
    char in[100];
    char *tok;
    char *end;
	int counter;
	char *task = vmalloc(15);


	printk("System Statistics:\n");
    // Create and open the file in user space
    f = file_open("/proc/uptime", O_CREAT |  O_RDWR | O_APPEND, \
                                		S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,in,100);

	counter = 0;
    tok = in;
    end = in;
    while (counter < 2){
		if(counter == 0) {
			strsep(&end," ");
			printk("system was booted since : %s\n",tok);
		}
		else {
			strsep(&end,"\n");
			printk("system has been idle since : %s\n",tok);
		}
		tok = end;
		counter++;
    }
    file_close(f);
	strncpy(in,"",strnlen("",1));
	strncpy(tok,"",strnlen("",1));
	strncpy(end,"",strnlen("",1));

    f = file_open("/proc/loadavg", O_CREAT |  O_RDWR | O_APPEND, \
                                		S_IRWXU | S_IRWXG | S_IRWXO);
    file_read(f,0,in,100);

	counter = 0;
    tok = in;
    end = in;
    while (counter < 5){
		strsep(&end," ");
		if(counter == 3) {
			strcpy(task,tok);
		}
		tok = end;
		counter++;
    }
	tok = task;
	end = task;
	strsep(&end,"/");
	printk("the number of active tasks : %s\n",tok);
	tok = end;
	strsep(&end,"/");
	printk("the total number of processes : %s\n",tok);
    file_close(f);
}

SYSCALL_DEFINE2(test,int,choice,char*,pid) { 
	cpuinfo();
	if(choice == 0){
		if(strncmp(pid,"0",1) == 0){
			// -all only
			param2();
		}
		else {
			// -p pid only
			param1(pid);
		}
	}
	else if(choice == 1){
		// -p pid -all
		param1(pid);
		param2();
	}
	else{
		// -all -p pid
		param2();
		param1(pid);
	}
    return 0;
}
