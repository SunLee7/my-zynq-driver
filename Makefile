#ubuntu的内核源码树，如果要编译在ubuntu中安装的模块就打开这2个
KERN_VER = $(shell uname -r)
KERN_DIR = /lib/modules/$(KERN_VER)/build	
		
# 开发板的linux内核的源码树目录,根据自己在源码树存放的目录修改
# KERN_DIR = /lib/modules/5.15.0-75-generic/build
# BASEINCLUDE = $(KERN_DIR)

# 设定处理器架构及编译器
ARCH=x86_64
CROSS_COMPILE=x86_64-linux-gnu-

# -m 表示我们要将my-first-drive.c编译成一个模块
# -y表示我们要将my-first-drive.c编译链接进zImage
obj-m	+= my-driver.o    

all:
	make -C $(KERN_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=`pwd` modules
# -C 表示进入到某一个目录下去编译
# `pwd`：表示把两个`号中间的内容当成命令执行
# M=`pwd`则表示把pwd打印的内容保存起来，目的是为了编译好了之后能够返回原来的目录
# modules就是真正用来编译模块的命令，在内核的其他地方定义过了

# 把编译出的模块拷贝到需要的地方
cp:									
	cp *.ko /home/sunlee/my-zynq-driver/  

# 把clean当成一个伪目标 只有'make clean'才会执行
.PHONY: clean
clean:
	make -C $(KERN_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=`pwd` modules clean