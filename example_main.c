/******************************************************************************
                                使用方法:
1:修改串口DEV_NAME，电脑COMx对应 /dev/ttySx
2:修改 protocol.h 中的PK_PS 以及对应需要的宏开关
3:编译命令 make，生成的可执行文件在 /target/target.bin
4:执行 ./target/target.bin
******************************************************************************/

#include "stdio.h"
#include "pthread.h"
#include "sys/select.h"
#include "string.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include "wifi.h"

int uartFd = 0;

void getLocalTime(void)
{
	time_t current_time;
	struct tm *time_info;
	char time_string[40];

	// 获取当前时间
	time(&current_time);
	time_info = localtime(&current_time);

	// 格式化时间为字符串
	strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

	// printf("===============================================\r\n");
	// printf("=======Current time: %s=======\n", time_string);
	// printf("===============================================\r\n");
}

int Qhal_uartSendOneByte(uint8_t *data)
{
	write(uartFd, data, 1);
	return TRUE;
}

void *Qhal_uartIntHandler(void *arg)
{
	int fd = *(int *)arg;
	printf("fd = %d\r\n", fd);
	unsigned char buf[1400] = {0};

	while(1)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		if (select(fd + 1, &rfds, NULL, NULL, NULL) > 0)
		{
			memset(buf, 0, sizeof(buf));
			int bufLen = read(fd, buf, sizeof(buf));
			if (bufLen < 0) {
				perror("read");
				continue;
			}
			printf("Read %d bytes from serial port: %s\n", bufLen, buf);
			// sleep(1);
			getLocalTime();
			uart_receive_buff_input(buf, bufLen);
		}
	}
}

void *WIFI_UART_SERVICE(void *arg)
{
    while (1)
    {
        wifi_uart_service();
        sleep(1);

		static uint8_t backup = 128;
		uint8_t staus = mcu_get_wifi_work_state();
		if (backup != staus)
		{
			backup = staus;
			char *message = "unknow";
			switch(staus)
			{
				case WIFI_CONFIG_STATE:
					message = "WIFI_CONFIG_STATE";
					break;
				case WIFI_NOT_CONNECTED:
					message = "WIFI_NOT_CONNECTED";
					break;
				case WIFI_CONNECTED:
					message = "WIFI_CONNECTED";
					break;
				case WIFI_CONN_CLOUD:
					message = "WIFI_CONN_CLOUD";
					break;
				default:
					break;
			}
			printf("FC41D Net status:%s\r\n", message);
		}
    }
}

void *FunctionalTesting(void *arg)
{
	int value;
	int tslID;
loop:
	value = 0;
	tslID = 0;

	printf("\r\n****************请选择对应操作**************\r\n");
	printf("* \t0. 清屏\r\n");
	printf("* \t1. 重置Wi-Fi\r\n");
	printf("* \t2. 获取模组mac地址\r\n");
	printf("* \t3. 上报raw数据\r\n");
	printf("* \t4. 上报bool数据\r\n");
	printf("* \t5. 上报value数据\r\n");
	printf("* \t6. 上报double数据\r\n");
	printf("* \t7. 上报string数据\r\n");
	printf("* \t8. 上报struct数据\r\n");
	printf("* \t9. 上报多个物模型\r\n");
	printf("* \t10. 全部物模型上报\r\n");
	printf("*******************************************\r\n");
	scanf("%d", &value);

	if (2 < value && 10 > value)
	{
		printf("请输入对应物模型ID：");
		scanf("%d", &tslID);
	}

	switch(value)
	{
		case 0:
			system("clear");
		break;
		case 1://进入配网
		{
			mcu_reset_wifi();
			// system("clear");
			printf("\t WiFi信息已重置\r\n");
		}
		break;
		case 2://获取模组mac地址
			mcu_get_module_mac();
		break;
		case 3://raw型tsl数据上传
		{
			int cnt = 0;
			printf("请输入要上传的raw数据个数：");
			scanf("%d", &cnt);
			unsigned char *buf = malloc(cnt);
			for (int i = 0; i < cnt; i++)
			{
				printf("请输入第%d个数：", i);
				scanf("%d", &buf[i]);
			}
			mcu_tsl_raw_update(tslID, buf, sizeof(buf));
			free(buf);
		}
		break;
		case 4://bool型tsl数据上传
		{
			int state = 0;
			printf("请输入要上报的bool值：");
			scanf("%d", &state);
			mcu_tsl_bool_update(tslID, state > 0 ? true : false);
		}
		break;
		case 5://value型tsl数据上传
		{
			int tempValue = 0;
			printf("请输入要上报的value值：");
			scanf("%d", &tempValue);
			mcu_tsl_value_update(tslID, tempValue);
		}
		break;
		case 6://double型tsl数据上传
		{
			double tempValue = 0;
			printf("请输入要上报的double值：");
			scanf("%lf", &tempValue);
			mcu_tsl_double_update(tslID, tempValue);
		}
		break;
		case 7://string型tsl数据上传
		{
			unsigned char buf[1024] = {0};
			printf("请输入长度小于1024的字符串数据：");
			scanf("%s", buf);
			mcu_tsl_string_update(tslID, buf, strlen((char *)buf));
		}
		break;
		case 8://struct型tsl数据上传
		case 9://多个物模型上报
		{
			struct data
			{
				bool qbool;
				int qvalue;
				double qdouble;
				unsigned char RAW_buf[20];
				unsigned char STRING_buf[40];
			};

			struct data qdata = {
				.qbool = 1,
				.qvalue = 12,
				.qdouble = 123.45,
			};
			my_memcpy(qdata.STRING_buf, "0123456789abcdefghijklmnopqrstuvwxyz", sizeof("0123456789abcdefghijklmnopqrstuvwxyz"));
			for (int i = 0; i < sizeof(qdata.RAW_buf); i++)
			{
				qdata.RAW_buf[i] = i;
			}

			mcu_tsl_struct_t st;
			unsigned char buffer[1024] = {0};
			mcu_tsl_struct_init(tslID, &st, buffer, sizeof(buffer));
			mcu_tsl_struct_add_item(1, &st, TSL_TYPE_RAW, qdata.RAW_buf, my_strlen(qdata.RAW_buf));//添加结构体的成员数据
			mcu_tsl_struct_add_item(2, &st, TSL_TYPE_BOOL, (unsigned char *)&qdata.qbool, 1);//添加结构体的成员数据
			mcu_tsl_struct_add_item(3, &st, TSL_TYPE_VALUE, (unsigned char *)&qdata.qvalue, 4);//添加结构体的成员数据
			mcu_tsl_struct_add_item(4, &st, TSL_TYPE_STRING, qdata.STRING_buf, my_strlen(qdata.STRING_buf));//添加结构体的成员数据
			mcu_tsl_struct_add_item(5, &st, TSL_TYPE_DOUBLE, (unsigned char *)&qdata.qdouble, 8);//添加结构体的成员数据

			if (value == 8)
			{
				mcu_tsl_struct_update(&st);
			}
			else if (value == 9)
			{
				mcu_tsl_datap_t g_datap;
				memset(&g_datap, 0x00, sizeof(mcu_tsl_datap_t));
				unsigned char g_datap_buf[1024] = {0};
				mcu_tsl_datap_init(&g_datap, g_datap_buf, sizeof(g_datap_buf));
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_RAW, 1, qdata.RAW_buf, my_strlen(qdata.RAW_buf));
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, 2, (const unsigned char *)&qdata.qbool, 1);
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, 3, (const unsigned char *)&qdata.qvalue, 4);
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_STRING, 4, qdata.STRING_buf, my_strlen(qdata.STRING_buf));
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_DOUBLE, 5, (const unsigned char *)&qdata.qdouble, 8);
				
				#if 0
				mcu_tsl_datap_update(&g_datap, 0);
				#else
				mcu_tsl_datap_add(&g_datap, TSL_TYPE_STRUCT, 0, (const unsigned char *)&st, sizeof(mcu_tsl_struct_t));
				mcu_tsl_datap_update(&g_datap, 0);
				#endif
			}
		}
		break;
		case 10:
			all_data_update();
		break;
		default:
			system("clear");
			break;
	}
	getLocalTime();
	goto loop;
}

int main(int argc, char const *argv[])
{
	wifi_protocol_init();

	int port = 0;
	printf("=========请输入要打开的USB端口号==========：");
	scanf("%d", &port);

	char dev[20] = {0};
	sprintf(dev, "%s%d", "/dev/tty", port);

	uartFd = open(dev, O_RDWR|O_NOCTTY);
	printf("fd = %d\r\n", uartFd);
	if (uartFd == -1)
	{
		exit(0);
	}

	/* 恢复串口为阻塞状态 */
	if (fcntl(uartFd, F_SETFL, 0) < 0)
	{
		close(uartFd);
		exit(0);
	}
	struct termios options;
	bzero(&options, sizeof(options));
	options.c_cflag |= CLOCAL | CREAD;
	options.c_cflag &= ~CSIZE;         // 清除数据位设置
	cfsetispeed(&options, B2400);    //串口输入波特率
	cfsetospeed(&options, B2400);    //串口输出波特率
	options.c_cflag |= CS8;            // 设置数据位为8位
	options.c_cflag &= ~CSTOPB;        // 设置停止位为1位
	options.c_cflag &= ~PARENB;        // 禁用奇偶校验
	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 0;
	tcflush(uartFd, TCIOFLUSH);
	tcsetattr(uartFd, TCSANOW, &options);

	pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, (void *)Qhal_uartIntHandler, (void *)&uartFd);
	pthread_create(&thread2, NULL, (void *)WIFI_UART_SERVICE, NULL);

	printf("  === 数据测试数据测试 ===  \r\n");

	pthread_create(&thread3, NULL, (void *)FunctionalTesting, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	exit(0);
}
