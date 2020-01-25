#include <stddef.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>


void file_close(void) 
{
    loff_t size;
    int buff[100];
    kernel_read_file_from_path("/etc/cremona", (void**)&buff, &size, sizeof(int) * 100, MODULE);
}

