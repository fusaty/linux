from ..qemu_config import QemuArchParams

QEMU_ARCH = QemuArchParams(freax_arch='alpha',
			   kconfig='''
CONFIG_SERIAL_8250=y
CONFIG_SERIAL_8250_CONSOLE=y''',
			   qemu_arch='alpha',
			   kernel_path='arch/alpha/boot/vmfreax',
			   kernel_command_line='console=ttyS0',
			   extra_qemu_params=[])
