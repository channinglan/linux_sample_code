#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd4f8a165, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x19df07d0, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x1eceff92, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x7084fc02, __VMLINUX_SYMBOL_STR(__platform_driver_probe) },
	{ 0x2469810f, __VMLINUX_SYMBOL_STR(__rcu_read_unlock) },
	{ 0x60aeb64a, __VMLINUX_SYMBOL_STR(pid_task) },
	{ 0xe7ad8c9e, __VMLINUX_SYMBOL_STR(find_pid_ns) },
	{ 0x18044d9b, __VMLINUX_SYMBOL_STR(init_pid_ns) },
	{ 0x8d522714, __VMLINUX_SYMBOL_STR(__rcu_read_lock) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0x17cbb8fa, __VMLINUX_SYMBOL_STR(of_get_property) },
	{ 0x725f3bd1, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0xaaeba1d0, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x3cbaece9, __VMLINUX_SYMBOL_STR(irq_of_parse_and_map) },
	{ 0x3d627d29, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x8a7d883e, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xff65d5b7, __VMLINUX_SYMBOL_STR(of_find_matching_node_and_match) },
	{ 0xe52b7eba, __VMLINUX_SYMBOL_STR(of_match_node) },
	{ 0x744dac03, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xa96aaca, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x86e88464, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa125492, __VMLINUX_SYMBOL_STR(send_sig_info) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cpower_fail*");

MODULE_INFO(srcversion, "BF5CAEBE9DD11B9790AB674");
