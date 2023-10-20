#在这个示例 Makefile 中，使用了 wildcard 函数和 patsubst 函数来自动查找当前目录下的所有 .c 文件，并生成对应的 .o 文件和目标文件 target.bin。使用 -lpthread -lutil 选项来链接 lpthread 和 lutil 库。如果需要清理目标文件和中间文件，可以输入 make clean 命令。
#在命令行中进入 Makefile 所在的目录，输入 make 命令即可编译所有的 .c 文件并链接 lpthread 和 lutil 库，并将生成的可执行文件命名为 target.bin，存放在新建的 target 文件夹中。如果需要清理目标文件和中间文件，可以输入 make clean 命令。

# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -g

# 定义目标文件夹、目标文件和源文件
OBJDIR = target
SRCDIR = portable_power
TARGET = $(OBJDIR)/target.bin
SRCS = $(wildcard $(SRCDIR)/*.c *.c)

# 链接库文件
LIBS = -lpthread -lutil

# 头文件
INCLUDES = -Iportable_power

# 默认目标：编译所有 C 文件并链接库
all: $(TARGET)
	@echo "Build completed."

$(TARGET): $(SRCS)
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $^ $(LIBS)
	@rm -f $(OBJDIR)/*.o

# 清理目标文件和中间文件
clean:
	@rm -rf $(OBJDIR)

.PHONY: all clean
