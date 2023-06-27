#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2a1826b7, "module_layout" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x52096e97, "cdev_del" },
	{ 0xe11f6c6c, "class_destroy" },
	{ 0x50c3f86b, "device_destroy" },
	{ 0xb4364f80, "device_create" },
	{ 0xf3cd999d, "__class_create" },
	{ 0x4859db92, "cdev_add" },
	{ 0xe7575142, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0x24d273d1, "add_timer" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xdcb764ad, "memset" },
	{ 0x84bc974b, "__arch_copy_from_user" },
	{ 0xb35dea8f, "__arch_copy_to_user" },
	{ 0xad27f361, "__warn_printk" },
	{ 0x7c32d0f0, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

