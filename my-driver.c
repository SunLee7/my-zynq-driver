#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>      // file_operations
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/gpio.h>

#define MYNMAJOR  200
#define MYNAME    "my_first_driver"

// ZYNQ PL axi_gpio 硬件地址
#define AXI_GPIO_BASE 0x41200000
#define ADDR_RANGE 0x10000
static void __iomem *baseaddr;

// 设备相关
static dev_t dev_id;
static struct cdev  *pcdev;
static struct class *test_class;

// 内核定时器
static struct timer_list htim;
struct timeval oldtv;

// GPIO
#define LED1 502

void my_gpio_init(char port, char pin, char data)
{
    int port_num = 0;
    char str[20];
    switch(port)
    {
        case 0: port_num = 496 + pin; break;
        case 1: port_num = 480 + pin; break;
        case 2: port_num = 464 + pin; break;
        case 3: port_num = 448 + pin; break;
        case 4: port_num = 432 + pin; break;
        case 5: port_num = 416 + pin; break;
        default:
            printk(KERN_INFO "GPIO NUM ERROR!\n");
            return;
    }
    sprintf(str, "SEG_GPIO_%d", port_num);
    gpio_request(port_num, str); //申请gpio
    gpio_direction_output(port_num, data);//设置为输出方向并输出0
}

void my_gpio_free(char port, char pin)
{
    int port_num = 0;
    switch(port)
    {
        case 0: port_num = 496 + pin; break;
        case 1: port_num = 480 + pin; break;
        case 2: port_num = 464 + pin; break;
        case 3: port_num = 448 + pin; break;
        case 4: port_num = 432 + pin; break;
        case 5: port_num = 416 + pin; break;
        default:
            printk(KERN_INFO "GPIO NUM ERROR!\n");
            return;
    }
    gpio_free(port_num);
}

void my_gpio_wirte(char port, char pin, char data)
{
    int port_num = 0;
    switch(port)
    {
        case 0: port_num = 496 + pin; break;
        case 1: port_num = 480 + pin; break;
        case 2: port_num = 464 + pin; break;
        case 3: port_num = 448 + pin; break;
        case 4: port_num = 432 + pin; break;
        case 5: port_num = 416 + pin; break;
        default:
            printk(KERN_INFO "GPIO NUM ERROR!\n");
            return;
    }
    gpio_set_value(port_num, data);
}

void seg_init(void)
{
    my_gpio_init(2, 3, 0);
    my_gpio_init(2, 4, 0);
    my_gpio_init(3, 3, 0);
    my_gpio_init(3, 4, 0);
    my_gpio_init(3, 5, 0);
    my_gpio_init(3, 6, 0);
    my_gpio_init(3, 7, 0);
    my_gpio_init(3, 8, 0);
    my_gpio_init(1, 5, 1);

    my_gpio_init(3, 9,  0);
    my_gpio_init(3, 10, 0);
    my_gpio_init(5, 9,  0);
    my_gpio_init(5, 10, 0);
    my_gpio_init(5, 11, 0);
    my_gpio_init(5, 12, 0);
    my_gpio_init(5, 13, 0);
    my_gpio_init(5, 14, 0);
}

void seg_deinit(void)
{
    my_gpio_free(2, 3);
    my_gpio_free(2, 4);
    my_gpio_free(3, 3);
    my_gpio_free(3, 4);
    my_gpio_free(3, 5);
    my_gpio_free(3, 6);
    my_gpio_free(3, 7);
    my_gpio_free(3, 8);
    my_gpio_free(1, 5);

    my_gpio_free(3, 9 );
    my_gpio_free(3, 10);
    my_gpio_free(5, 9 );
    my_gpio_free(5, 10);
    my_gpio_free(5, 11);
    my_gpio_free(5, 12);
    my_gpio_free(5, 13);
    my_gpio_free(5, 14);
}

void seg_display(char data)
{
    const unsigned char sec_tab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
    unsigned char data_buff;
    if(data > 99) data = 99;
    data_buff = sec_tab[data / 10];
    my_gpio_wirte(2, 3, data_buff & (1 << 0));
    my_gpio_wirte(2, 4, data_buff & (1 << 1));
    my_gpio_wirte(3, 3, data_buff & (1 << 2));
    my_gpio_wirte(3, 4, data_buff & (1 << 3));
    my_gpio_wirte(3, 5, data_buff & (1 << 4));
    my_gpio_wirte(3, 6, data_buff & (1 << 5));
    my_gpio_wirte(3, 7, data_buff & (1 << 6));
    my_gpio_wirte(3, 8, data_buff & (1 << 7));
    my_gpio_wirte(1, 5, 1);

    data_buff = sec_tab[data % 10];
    my_gpio_wirte(3, 9,  data_buff & (1 << 0));
    my_gpio_wirte(3, 10, data_buff & (1 << 1));
    my_gpio_wirte(5, 9,  data_buff & (1 << 2));
    my_gpio_wirte(5, 10, data_buff & (1 << 3));
    my_gpio_wirte(5, 11, data_buff & (1 << 4));
    my_gpio_wirte(5, 12, data_buff & (1 << 5));
    my_gpio_wirte(5, 13, data_buff & (1 << 6));
    my_gpio_wirte(5, 14, data_buff & (1 << 7));
}

//file_operations结构体变量中填充的函数指针的实体，函数的格式要遵守
static int test_chrdev_open(struct inode *inode, struct file *file)
{
    //这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    //但是现在我们暂时写不了那么多，所以就就用一个printk打印个信息来做代表 
    printk(KERN_INFO "test_module_open\n");
	  return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");
    return 0;
}

char kbuf[100];//内核空间的一个buf

static ssize_t test_chrdev_write(struct file *file, const char __user *buf,	size_t count, loff_t *ppos)
{
    int ret = -1;
    int seg_data;
    printk(KERN_INFO "test_chrdev_write\n");
    //使用该函数将应用层的传过来的ubuf中的内容拷贝到驱动空间(内核空间)的一个buf中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, buf, count);
    if(ret){
        printk(KERN_ERR "copy_from_user failed\n");
        return -EINVAL;//在真正的的驱动中没复制成功应该有一些纠错机制，这里我们简单点
    }
    printk(KERN_INFO "copy_from_user success..\n");
    kstrtoint(kbuf, 10, &seg_data);
    seg_display(seg_data);
    //到这里我们就成功把用户空间的数据转移到内核空间了
    //真正的驱动中，数据从应用层复制到驱动中后，我们就要根据这个数据去写硬件完成硬件的操作
    //所以下面就应该是操作硬件的代码
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
    int ret = -1;
    printk(KERN_INFO "test_chrdev_read\n");
    ret = copy_to_user(buf, kbuf, size);
    if(ret){
        printk(KERN_ERR "copy_to_user failed\n");
        return -EINVAL;//在真正的的驱动中没复制成功应该有一些纠错机制，这里我们简单点
    }
    printk(KERN_INFO "copy_to_user success..\n");
    return 0;
}

// 定时器回调函数
void timer_callback(struct timer_list *t)
{
    struct timeval tv;

    do_gettimeofday(&tv);
    htim.expires = jiffies + 1 * HZ;
    add_timer(&htim);

    // printk(KERN_INFO "kernel timer hander\n");
    static u32 data = 0x01;
    if(data == 0x01) data = 0x00;
    else data = 0x01;
    // writel(data, baseaddr);

}

//自定义一个file_operations结构体变量，并填充
static const struct file_operations test_module_fops = {
	.owner		= THIS_MODULE,         //惯例，所有的驱动都有这一个，这也是这结构体中唯一一个不是函数指针的元素
	.open		= test_chrdev_open,    //将来应用open打开这个这个设备时实际调用的函数
	.release	= test_chrdev_release,   //对应close，为什么不叫close呢？详见后面release和close的区别的讲解
    .read       = test_chrdev_read,
    .write      = test_chrdev_write
};

// 模块安装函数
static int __init chrdev_init(void)
{
    int ret = 0;
    // 分配主次设备号
    printk(KERN_INFO "chrdev_init helloworld init\n");
    ret = alloc_chrdev_region(&dev_id, 0, 1, MYNAME);
    if(ret < 0)
    {
        printk(KERN_ERR "alloc_chrdev_region failed\n");
        return -EINVAL;
    }
    // 完成设备注册
    pcdev = cdev_alloc();
    pcdev -> owner = THIS_MODULE;
    pcdev -> ops = &test_module_fops;
    ret = cdev_add(pcdev, dev_id, 1);
    if(ret)
    {
        printk(KERN_ERR "cdev_add failed\n");
        return -EINVAL;
    }
    // 创建/dev下的设备文件
    test_class = class_create(THIS_MODULE, "driver_class");
    if(IS_ERR(test_class))
    {
        printk(KERN_ERR "class_create failed\n");
        return -EINVAL;
    }
    device_create(test_class, NULL, dev_id, NULL, "my-driver");
    
    // if(!request_mem_region(AXI_GPIO_BASE, ADDR_RANGE, "AXI_GPIO_BASE"))
    // {
    //     printk(KERN_ERR "request_mem_region failed\n");
    //     return -EINVAL;
    // }
    // baseaddr = ioremap(AXI_GPIO_BASE, ADDR_RANGE);

    // 内核定时器初始化
    timer_setup(&htim, timer_callback, 0);

    // // 设置AXI_GPIO为输出模式
    // writel(0x00, baseaddr + 4);
    // writel(0x01, baseaddr);

    gpio_request(LED1,"LED1"); //申请gpio
    gpio_direction_output(LED1, 1);//设置为输出方向并输出1
    // gpio_direction_input(LED1); //设置为输入
    gpio_set_value(LED1, 1);//只是设置输出值
    // gpio_get_value(LED1);//获取值
    seg_init();
    seg_display(0);

    return 0;
}

// 模块卸载函数
static void __exit chrdev_exit(void)
{
    // iounmap(AXI_GPIO_BASE);
    // release_mem_region(AXI_GPIO_BASE, ADDR_RANGE);
    //释放gpio
    gpio_free(LED1);
    seg_deinit();
    // 删除定时器
    del_timer(&htim);

    device_destroy(test_class, dev_id);
    class_destroy(test_class);
    cdev_del(pcdev);
    unregister_chrdev_region(dev_id, 1);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
