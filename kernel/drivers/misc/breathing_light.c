#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
//#include <asm/mach/map.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>

#define RED_LED_DTS_COMPATIBLE       "lhd,breathing_light_test"       /* 设备树节点匹配属性 */

#define LED_PWM_CMD_SET_DUTY         0x01
#define LED_PWM_CMD_SET_PERIOD       0x02
#define LED_PWM_CMD_SET_BOTH         0x03
#define LED_PWM_CMD_ENABLE           0x04
#define LED_PWM_CMD_DISABLE          0x05

struct led_pwm_param {
    int duty_ns;
    int period_ns;
};

struct red_led_dev {
    dev_t dev_no;                    
    struct cdev chrdev;            
    struct class *led_class;
    struct device_node *dev_node;
    struct pwm_device *red_led_pwm;
};

static struct led_pwm_param led_pwm;
static struct red_led_dev led_dev;

static int red_led_drv_open (struct inode *node, struct file *file)
{
    int ret = 0;
    
	//pwm_set_periodnnn(led_dev.red_led_pwm, PWM_POLARITY_INVERSED);//设置PWM信号的极性
	pwm_enable(led_dev.red_led_pwm);//启用指定PWM设备，使其开始输出PWM信号。

    printk("red_led_pwm open\r\n");
    return ret;
}

static ssize_t red_led_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    int err;

    if (size != sizeof(led_pwm)) return -EINVAL;

	err = copy_from_user(&led_pwm, buf, size);
    if (err > 0) return -EFAULT;

	pwm_config(led_dev.red_led_pwm, led_pwm.duty_ns, led_pwm.period_ns);//配置PWM设备的基本参数，如频率、占空比等。
    printk("red_led_pwm write\r\n");
	return 1;
}

static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    void __user *my_user_space = (void __user *)arg;
    
    switch (cmd)
    {
        case LED_PWM_CMD_SET_DUTY:
            ret = copy_from_user(&led_pwm.duty_ns, my_user_space, sizeof(led_pwm.duty_ns));
            if (ret > 0) return -EFAULT;
            pwm_config(led_dev.red_led_pwm, led_pwm.duty_ns, led_pwm.period_ns);
            break;
        case LED_PWM_CMD_SET_PERIOD:
            ret = copy_from_user(&led_pwm.period_ns, my_user_space, sizeof(led_pwm.period_ns));
            if (ret > 0) return -EFAULT;
            pwm_config(led_dev.red_led_pwm, led_pwm.duty_ns, led_pwm.period_ns);
            break;
        case LED_PWM_CMD_SET_BOTH: 
            ret = copy_from_user(&led_pwm, my_user_space, sizeof(led_pwm));
            if (ret > 0) return -EFAULT;
            pwm_config(led_dev.red_led_pwm, led_pwm.duty_ns, led_pwm.period_ns);
            break;
        case LED_PWM_CMD_ENABLE:
            pwm_enable(led_dev.red_led_pwm);
            break;
        case LED_PWM_CMD_DISABLE:
            pwm_disable(led_dev.red_led_pwm);
            break;
    }
    return 0;
}

static int red_led_drv_release(struct inode *node, struct file *filp)
{
    int ret = 0;

    pwm_config(led_dev.red_led_pwm, 0, 5000);//配置PWM设备的基本参数，如频率、占空比等。
    printk("led pwm dev close\r\n");
//    pwm_disable(led_dev.red_led_pwm);
    return ret;
}

static struct file_operations red_led_drv = {
	.owner	 = THIS_MODULE,
	.open    = red_led_drv_open,
	.write   = red_led_drv_write,
    .unlocked_ioctl = drv_ioctl,
    .release  = red_led_drv_release,
};

/*设备树的匹配列表 */
static struct of_device_id dts_match_table[] = {
    {.compatible = RED_LED_DTS_COMPATIBLE, },  
    {},                  
};


static int led_red_driver_probe(struct platform_device *pdev)
{
    int err;
    int ret;
    struct device *tdev;
    struct device_node *child;

    tdev = &pdev->dev;
    child = of_get_next_child(tdev->of_node, NULL);      /* 获取设备树子节点 */
	if (!child) {
        return -EINVAL;
    }

    led_dev.red_led_pwm = devm_of_pwm_get(tdev, child, NULL);     /* 从子节点中获取PWM设备，设备树获取这个设备就可以了 */
    if (IS_ERR(led_dev.red_led_pwm)) {
        printk(KERN_ERR"can't get breathing_light!!\n");
        return -EFAULT;
    }

    ret = alloc_chrdev_region(&led_dev.dev_no, 0, 1, "breathing_light");//动态分配字符设备的主设备号
	if (ret < 0) {
		pr_err("Error: failed to register mbochs_dev, err: %d\n", ret);
		return ret;
	}

	cdev_init(&led_dev.chrdev, &red_led_drv);//初始化字符设备结构体cdev

	cdev_add(&led_dev.chrdev, led_dev.dev_no, 1);//将已经初始化的字符设备结构体cdev添加到系统中

    led_dev.led_class = class_create(THIS_MODULE, "breathing_light");//创建一个设备类（device class）并注册到内核中
	err = PTR_ERR(led_dev.led_class);
	if (IS_ERR(led_dev.led_class)) {
        goto failed1;
	}

    tdev = device_create(led_dev.led_class , NULL, led_dev.dev_no, NULL, "breathing_light"); //创建一个设备实例并注册到设备类中
    if (IS_ERR(tdev)) {
        ret = -EINVAL;
		goto failed2;
	}

   	printk(KERN_INFO"%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    
    return 0;
failed2:
    device_destroy(led_dev.led_class, led_dev.dev_no);
    class_destroy(led_dev.led_class);
failed1:
    cdev_del(&led_dev.chrdev);
	unregister_chrdev_region(led_dev.dev_no, 1);
    return ret;
}

int led_red_driver_remove(struct platform_device *dev)
{
    // pwm_disable(led_dev.red_led_pwm);
    // pwm_free(led_dev.red_led_pwm);
    printk(KERN_INFO"driver remove %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    device_destroy(led_dev.led_class, led_dev.dev_no);
	class_destroy(led_dev.led_class);
	unregister_chrdev_region(led_dev.dev_no, 1);
    cdev_del(&led_dev.chrdev);
     
    return 0;
}

static struct platform_driver red_led_platform_driver = {
      .probe = led_red_driver_probe,
      .remove = led_red_driver_remove,
      .driver = {
        .name = "lhd,breathing_light_test",
        .owner = THIS_MODULE,
        .of_match_table = dts_match_table,         //通过设备树匹配
      },
};

module_platform_driver(red_led_platform_driver);

MODULE_AUTHOR("LHD");
MODULE_LICENSE("GPL");