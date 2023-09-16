# 定义目标
TARGETS = myexec mybin

# 默认目标
.DEFAULT_GOAL := all

# 编译器和编译选项
CC = gcc
CFLAGS = -std=c99

# 源文件列表
SRCS = myexec.c mybin.c

# 根据源文件生成目标文件列表
OBJS = $(SRCS:.c=)

# 生成所有目标
all: $(TARGETS)

# 生成目标
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# 清理生成的文件
.PHONY: clean
clean:
	rm -rf $(TARGETS)

