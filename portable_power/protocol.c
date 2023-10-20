/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的wifi_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

#include "wifi.h"



/******************************************************************************
                              第一步:初始化
1:在需要使用到wifi相关文件的文件中include "wifi.h"
2:在MCU初始化中调用mcu_api.c文件中的wifi_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的wifi_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:tsl数据点序列类型对照表
                        **请和平台所设置的保持一致**         
******************************************************************************/

const DOWNLOAD_CMD_S download_cmd[] = {
	{TSLID_ENERGY_TEMPER_F_VALUE, TSL_TYPE_VALUE},
	{TSLID_BACKLIGHT_VALUE, TSL_TYPE_VALUE},
	{TSLID_MODE_VALUE, TSL_TYPE_VALUE},
	{TSLID_LIGHT_BOOL, TSL_TYPE_BOOL},
	{TSLID_ECO_BOOL, TSL_TYPE_BOOL},
	{TSLID_ANION_BOOL, TSL_TYPE_BOOL},
	{TSLID_SOUND_BOOL, TSL_TYPE_BOOL},
	{TSLID_WINDOW_CHECK_BOOL, TSL_TYPE_BOOL},
	{TSLID_SLEEP_BOOL, TSL_TYPE_BOOL},
	{TSLID_FROST_BOOL, TSL_TYPE_BOOL},
	{TSLID_COMFORTABLE_TEMPER_VALUE, TSL_TYPE_VALUE},
	{TSLID_ENERGY_TEMPER_VALUE, TSL_TYPE_VALUE},
	{TSLID_VALVE_CHECK_BOOL, TSL_TYPE_BOOL},
	{TSLID_SWITCH_VERTICAL_BOOL, TSL_TYPE_BOOL},
	{TSLID_SWITCH_HORIZONTAL_BOOL, TSL_TYPE_BOOL},
	{TSLID_FAN_VERTICAL_VALUE, TSL_TYPE_VALUE},
	{TSLID_FAN_HORIZONTAL_VALUE, TSL_TYPE_VALUE},
	{TSLID_TEMP_SET_VALUE, TSL_TYPE_VALUE},
	{TSLID_TEMP_SET_F_VALUE, TSL_TYPE_VALUE},
	{TSLID_TEMP_UNIT_VALUE, TSL_TYPE_VALUE},
	{TSLID_UPPER_TEMP_VALUE, TSL_TYPE_VALUE},
	{TSLID_UPPER_TEMP_F_VALUE, TSL_TYPE_VALUE},
	{TSLID_LOWER_TEMP_VALUE, TSL_TYPE_VALUE},
	{TSLID_LOWER_TEMP_F_VALUE, TSL_TYPE_VALUE},
	{TSLID_TEMP_CORRECTION_VALUE, TSL_TYPE_VALUE},
	{TSLID_LOCK_BOOL, TSL_TYPE_BOOL},
	{TSLID_COUNTDOWN_VALUE, TSL_TYPE_VALUE},
	{TSLID_LEVEL_VALUE, TSL_TYPE_VALUE},
	{TSLID_FACTORY_RESET_BOOL, TSL_TYPE_BOOL},
	{TSLID_WEEK_PROGRAM_STRING, TSL_TYPE_STRING},
	{TSLID_COMFORTABLE_TEMPER_F_VALUE, TSL_TYPE_VALUE},

};

tsl_demo_t g_tsl_demo = {
	.energy_temper_f = 1,	// 节能温度_f
	.backlight = 1,	// 背光亮度
	.week_program_unit = 1,	// 周程序单位
	.mode = 1,	// 工作模式
	.work_state = 1,	// 工作状态
	.light = 1,	// 灯光
	.eco = 1,	// ECO开关
	.anion = 1,	// 负离子
	.sound = 1,	// 声音
	.window_check = 1,	// 开窗检测
	.sleep = 1,	// 睡眠开关
	.frost = 1,	// 防霜冻开关
	.comfortable_temper = 1,	// 舒适温度
	.energy_temper = 1,	// 节能温度
	.valve_check = 1,	// 阀门检测
	.valve_state = 1,	// 阀门状态
	.switch_vertical = 1,	// 上下摆风
	.switch_horizontal = 1,	// 左右摆风
	.fan_vertical = 1,	// 上下摆风角度
	.fan_horizontal = 1,	// 左右摆动角度
	.temp_set = 1,	// 目标温度
	.temp_set_f = 1,	// 目标温度_F
	.temp_unit = 1,	// 温标切换
	.upper_temp = 1,	// 设置温度上限
	.upper_temp_f = 1,	// 设置温度上限_F
	.lower_temp = 1,	// 设置温度下限
	.lower_temp_f = 1,	// 设置温度下限_F
	.temp_current = 1,	// 当前温度
	.temp_current_f = 1,	// 当前温度_F
	.humidity = 1,	// 湿度
	.temp_correction = 1,	// 温度校准
	.lock = 1,	// 童锁
	.countdown = 1,	// 倒计时
	.level = 1,	// 档位
	.factory_reset = 1,	// 恢复出厂设置
	.week_program = (unsigned char *)"aabbccdd",	// 周程序
	.battery_level = 1,	// 电池电量
	.comfortable_temper_f = 1,	// 舒适温度_f
};



/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    // #error "请将MCU串口发送函数填入该函数,并删除该行"
    int Qhal_uartSendOneByte(uint8_t *data);
    Qhal_uartSendOneByte(&value);
}

/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/



/**
 * @brief  系统所有tsl点信息上传,实现APP和muc数据同步
 * @param  Null
 * @return Null
 * @note   此函数SDK内部需调用，MCU必须实现该函数内数据上报功能，包括只上报和可上报可下发型数据
 */
void all_data_update(void)
{
	mcu_tsl_datap_t g_datap = {0};
	unsigned char g_datap_buf[1024] = {0};
	my_memset(&g_datap, 0x00, sizeof(mcu_tsl_datap_t));
	my_memset(g_datap_buf, 0x00, sizeof(g_datap_buf));
	mcu_tsl_datap_init(&g_datap, g_datap_buf, sizeof(g_datap_buf));
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_ENERGY_TEMPER_F_VALUE, (const unsigned char *)&g_tsl_demo.energy_temper_f, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_BACKLIGHT_VALUE, (const unsigned char *)&g_tsl_demo.backlight, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_WEEK_PROGRAM_UNIT_VALUE, (const unsigned char *)&g_tsl_demo.week_program_unit, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_MODE_VALUE, (const unsigned char *)&g_tsl_demo.mode, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_WORK_STATE_VALUE, (const unsigned char *)&g_tsl_demo.work_state, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_LIGHT_BOOL, (const unsigned char *)&g_tsl_demo.light, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_ECO_BOOL, (const unsigned char *)&g_tsl_demo.eco, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_ANION_BOOL, (const unsigned char *)&g_tsl_demo.anion, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_SOUND_BOOL, (const unsigned char *)&g_tsl_demo.sound, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_WINDOW_CHECK_BOOL, (const unsigned char *)&g_tsl_demo.window_check, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_SLEEP_BOOL, (const unsigned char *)&g_tsl_demo.sleep, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_FROST_BOOL, (const unsigned char *)&g_tsl_demo.frost, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_COMFORTABLE_TEMPER_VALUE, (const unsigned char *)&g_tsl_demo.comfortable_temper, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_ENERGY_TEMPER_VALUE, (const unsigned char *)&g_tsl_demo.energy_temper, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_VALVE_CHECK_BOOL, (const unsigned char *)&g_tsl_demo.valve_check, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_VALVE_STATE_VALUE, (const unsigned char *)&g_tsl_demo.valve_state, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_SWITCH_VERTICAL_BOOL, (const unsigned char *)&g_tsl_demo.switch_vertical, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_SWITCH_HORIZONTAL_BOOL, (const unsigned char *)&g_tsl_demo.switch_horizontal, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_FAN_VERTICAL_VALUE, (const unsigned char *)&g_tsl_demo.fan_vertical, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_FAN_HORIZONTAL_VALUE, (const unsigned char *)&g_tsl_demo.fan_horizontal, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_SET_VALUE, (const unsigned char *)&g_tsl_demo.temp_set, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_SET_F_VALUE, (const unsigned char *)&g_tsl_demo.temp_set_f, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_UNIT_VALUE, (const unsigned char *)&g_tsl_demo.temp_unit, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_UPPER_TEMP_VALUE, (const unsigned char *)&g_tsl_demo.upper_temp, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_UPPER_TEMP_F_VALUE, (const unsigned char *)&g_tsl_demo.upper_temp_f, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_LOWER_TEMP_VALUE, (const unsigned char *)&g_tsl_demo.lower_temp, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_LOWER_TEMP_F_VALUE, (const unsigned char *)&g_tsl_demo.lower_temp_f, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_CURRENT_VALUE, (const unsigned char *)&g_tsl_demo.temp_current, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_CURRENT_F_VALUE, (const unsigned char *)&g_tsl_demo.temp_current_f, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_HUMIDITY_VALUE, (const unsigned char *)&g_tsl_demo.humidity, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_TEMP_CORRECTION_VALUE, (const unsigned char *)&g_tsl_demo.temp_correction, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_LOCK_BOOL, (const unsigned char *)&g_tsl_demo.lock, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_COUNTDOWN_VALUE, (const unsigned char *)&g_tsl_demo.countdown, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_LEVEL_VALUE, (const unsigned char *)&g_tsl_demo.level, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_BOOL, TSLID_FACTORY_RESET_BOOL, (const unsigned char *)&g_tsl_demo.factory_reset, 1);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_STRING, TSLID_WEEK_PROGRAM_STRING, (const unsigned char *)g_tsl_demo.week_program, my_strlen((unsigned char *)g_tsl_demo.week_program));
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_BATTERY_LEVEL_VALUE, (const unsigned char *)&g_tsl_demo.battery_level, 4);
	mcu_tsl_datap_add(&g_datap, TSL_TYPE_VALUE, TSLID_COMFORTABLE_TEMPER_F_VALUE, (const unsigned char *)&g_tsl_demo.comfortable_temper_f, 4);
	mcu_tsl_datap_update(&g_datap, 0);
}


/*****************************************************************************
 * 函数名称 : tsl_download_energy_temper_f_handle
 * 功能描述 : 针对 TSLID_ENERGY_TEMPER_F_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_energy_temper_f_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.energy_temper_f = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_ENERGY_TEMPER_F_VALUE, g_tsl_demo.energy_temper_f);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_backlight_handle
 * 功能描述 : 针对 TSLID_BACKLIGHT_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_backlight_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.backlight = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_BACKLIGHT_VALUE, g_tsl_demo.backlight);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_mode_handle
 * 功能描述 : 针对 TSLID_MODE_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_mode_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.mode = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.mode)
	{
        case 1:
            // 工作模式->制冷
			printf("工作模式->制冷");
            break;
        case 2:
            // 工作模式->制热
			printf("工作模式->制热");
            break;
        case 3:
            // 工作模式->送风
			printf("工作模式->送风");
            break;
        case 4:
            // 工作模式->自动
			printf("工作模式->自动");
            break;
        case 5:
            // 工作模式->舒适
			printf("工作模式->舒适");
            break;
        case 6:
            // 工作模式->节能
			printf("工作模式->节能");
            break;
        case 7:
            // 工作模式->手动
			printf("工作模式->手动");
            break;
        case 8:
            // 工作模式->经济
			printf("工作模式->经济");
            break;
        case 9:
            // 工作模式->睡眠
			printf("工作模式->睡眠");
            break;
        case 10:
            // 工作模式->除湿
            break;
        case 11:
            // 工作模式->编程
            break;
        case 12:
            // 工作模式->地暖
            break;
        case 13:
            // 工作模式->辅热
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_MODE_VALUE, g_tsl_demo.mode);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_light_handle
 * 功能描述 : 针对 TSLID_LIGHT_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_light_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.light = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_LIGHT_BOOL, g_tsl_demo.light);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_eco_handle
 * 功能描述 : 针对 TSLID_ECO_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_eco_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.eco = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_ECO_BOOL, g_tsl_demo.eco);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_anion_handle
 * 功能描述 : 针对 TSLID_ANION_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_anion_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.anion = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_ANION_BOOL, g_tsl_demo.anion);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_sound_handle
 * 功能描述 : 针对 TSLID_SOUND_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_sound_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.sound = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_SOUND_BOOL, g_tsl_demo.sound);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_window_check_handle
 * 功能描述 : 针对 TSLID_WINDOW_CHECK_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_window_check_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.window_check = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_WINDOW_CHECK_BOOL, g_tsl_demo.window_check);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_sleep_handle
 * 功能描述 : 针对 TSLID_SLEEP_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_sleep_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.sleep = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_SLEEP_BOOL, g_tsl_demo.sleep);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_frost_handle
 * 功能描述 : 针对 TSLID_FROST_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_frost_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.frost = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_FROST_BOOL, g_tsl_demo.frost);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_comfortable_temper_handle
 * 功能描述 : 针对 TSLID_COMFORTABLE_TEMPER_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_comfortable_temper_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.comfortable_temper = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_COMFORTABLE_TEMPER_VALUE, g_tsl_demo.comfortable_temper);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_energy_temper_handle
 * 功能描述 : 针对 TSLID_ENERGY_TEMPER_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_energy_temper_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.energy_temper = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_ENERGY_TEMPER_VALUE, g_tsl_demo.energy_temper);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_valve_check_handle
 * 功能描述 : 针对 TSLID_VALVE_CHECK_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_valve_check_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.valve_check = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_VALVE_CHECK_BOOL, g_tsl_demo.valve_check);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_switch_vertical_handle
 * 功能描述 : 针对 TSLID_SWITCH_VERTICAL_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_switch_vertical_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.switch_vertical = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_SWITCH_VERTICAL_BOOL, g_tsl_demo.switch_vertical);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_switch_horizontal_handle
 * 功能描述 : 针对 TSLID_SWITCH_HORIZONTAL_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_switch_horizontal_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.switch_horizontal = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_SWITCH_HORIZONTAL_BOOL, g_tsl_demo.switch_horizontal);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_fan_vertical_handle
 * 功能描述 : 针对 TSLID_FAN_VERTICAL_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_fan_vertical_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.fan_vertical = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.fan_vertical)
	{
        case 1:
            // 上下摆风角度->30
            break;
        case 2:
            // 上下摆风角度->90
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_FAN_VERTICAL_VALUE, g_tsl_demo.fan_vertical);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_fan_horizontal_handle
 * 功能描述 : 针对 TSLID_FAN_HORIZONTAL_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_fan_horizontal_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.fan_horizontal = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.fan_horizontal)
	{
        case 1:
            // 左右摆动角度->30
            break;
        case 2:
            // 左右摆动角度->90
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_FAN_HORIZONTAL_VALUE, g_tsl_demo.fan_horizontal);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_temp_set_handle
 * 功能描述 : 针对 TSLID_TEMP_SET_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_temp_set_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.temp_set = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_TEMP_SET_VALUE, g_tsl_demo.temp_set);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_temp_set_f_handle
 * 功能描述 : 针对 TSLID_TEMP_SET_F_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_temp_set_f_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.temp_set_f = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_TEMP_SET_F_VALUE, g_tsl_demo.temp_set_f);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_temp_unit_handle
 * 功能描述 : 针对 TSLID_TEMP_UNIT_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_temp_unit_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.temp_unit = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.temp_unit)
	{
        case 1:
            // 温标切换->c
            break;
        case 2:
            // 温标切换->f
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_TEMP_UNIT_VALUE, g_tsl_demo.temp_unit);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_upper_temp_handle
 * 功能描述 : 针对 TSLID_UPPER_TEMP_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_upper_temp_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.upper_temp = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_UPPER_TEMP_VALUE, g_tsl_demo.upper_temp);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_upper_temp_f_handle
 * 功能描述 : 针对 TSLID_UPPER_TEMP_F_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_upper_temp_f_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.upper_temp_f = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_UPPER_TEMP_F_VALUE, g_tsl_demo.upper_temp_f);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_lower_temp_handle
 * 功能描述 : 针对 TSLID_LOWER_TEMP_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_lower_temp_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.lower_temp = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_LOWER_TEMP_VALUE, g_tsl_demo.lower_temp);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_lower_temp_f_handle
 * 功能描述 : 针对 TSLID_LOWER_TEMP_F_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_lower_temp_f_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.lower_temp_f = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_LOWER_TEMP_F_VALUE, g_tsl_demo.lower_temp_f);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_temp_correction_handle
 * 功能描述 : 针对 TSLID_TEMP_CORRECTION_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_temp_correction_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.temp_correction = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_TEMP_CORRECTION_VALUE, g_tsl_demo.temp_correction);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_lock_handle
 * 功能描述 : 针对 TSLID_LOCK_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_lock_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.lock = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_LOCK_BOOL, g_tsl_demo.lock);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_countdown_handle
 * 功能描述 : 针对 TSLID_COUNTDOWN_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_countdown_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.countdown = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.countdown)
	{
        case 1:
            // 倒计时->取消
            break;
        case 2:
            // 倒计时->1h
            break;
        case 3:
            // 倒计时->2h
            break;
        case 4:
            // 倒计时->3h
            break;
        case 5:
            // 倒计时->4h
            break;
        case 6:
            // 倒计时->5h
            break;
        case 7:
            // 倒计时->6h
            break;
        case 8:
            // 倒计时->7h
            break;
        case 9:
            // 倒计时->8h
            break;
        case 10:
            // 倒计时->9h
            break;
        case 11:
            // 倒计时->10h
            break;
        case 12:
            // 倒计时->11h
            break;
        case 13:
            // 倒计时->12h
            break;
        case 14:
            // 倒计时->13h
            break;
        case 15:
            // 倒计时->14h
            break;
        case 16:
            // 倒计时->15h
            break;
        case 17:
            // 倒计时->16h
            break;
        case 18:
            // 倒计时->17h
            break;
        case 19:
            // 倒计时->18h
            break;
        case 20:
            // 倒计时->19h
            break;
        case 21:
            // 倒计时->20h
            break;
        case 22:
            // 倒计时->21h
            break;
        case 23:
            // 倒计时->22h
            break;
        case 24:
            // 倒计时->23h
            break;
        case 25:
            // 倒计时->24h
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_COUNTDOWN_VALUE, g_tsl_demo.countdown);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_level_handle
 * 功能描述 : 针对 TSLID_LEVEL_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_level_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.level = mcu_get_tsl_download_value(value, length);
	switch(g_tsl_demo.level)
	{
        case 1:
            // 档位->低
            break;
        case 2:
            // 档位->中
            break;
        case 3:
            // 档位->高
            break;
        case 4:
            // 档位->自动
            break;
		default:
			// Default handling
			break;
	}

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_LEVEL_VALUE, g_tsl_demo.level);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_factory_reset_handle
 * 功能描述 : 针对 TSLID_FACTORY_RESET_BOOL 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_factory_reset_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.factory_reset = mcu_get_tsl_download_bool(value, length);
	/*
	// BOOL type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_bool_update(TSLID_FACTORY_RESET_BOOL, g_tsl_demo.factory_reset);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_week_program_handle
 * 功能描述 : 针对 TSLID_WEEK_PROGRAM_STRING 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_week_program_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	ret = mcu_get_tsl_download_string(value, length, g_tsl_demo.week_program, &length);
	/*
	// STRING type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_string_update(TSLID_WEEK_PROGRAM_STRING, g_tsl_demo.week_program, my_strlen(g_tsl_demo.week_program));
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}
/*****************************************************************************
 * 函数名称 : tsl_download_comfortable_temper_f_handle
 * 功能描述 : 针对 TSLID_COMFORTABLE_TEMPER_F_VALUE 的处理函数
 * 输入参数 : value:数据源数据
 *           length:数据长度
 * 返回参数 : 成功返回:SUCCESS/失败返回:ERROR
 * 使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
 *****************************************************************************/
static unsigned char tsl_download_comfortable_temper_f_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;

	g_tsl_demo.comfortable_temper_f = mcu_get_tsl_download_value(value, length);
	/*
	// VALUE type data processing
	*/

	// There should be a report after processing the tsl
	ret = mcu_tsl_value_update(TSLID_COMFORTABLE_TEMPER_F_VALUE, g_tsl_demo.comfortable_temper_f);
	if (ret == SUCCESS)
		return SUCCESS;
	else
		return ERROR;
}


/******************************************************************************
                                WARNING!!!    
                            2:所有数据上报处理
                            具体请用户自行实现数据处理
******************************************************************************/

/******************************************************************************
                                WARNING!!!                     
此部分函数用户请勿修改!!
******************************************************************************/

#ifdef SUPPORT_MCU_FIRM_UPDATE
/**
 * @brief  升级包大小选择
 * @param[in] {package_sz} 升级包大小
 * @ref           0x00: 256byte (默认)
 * @ref           0x01: 512byte
 * @ref           0x02: 1024byte
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void upgrade_package_choose(unsigned char package_sz)
{
    // #error "请自行实现请自行实现升级包大小选择代码,完成后请删除该行"
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len, package_sz);
    wifi_uart_write_frame(UPDATE_START_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  MCU进入固件升级模式
 * @param[in] {value} 固件缓冲区
 * @param[in] {position} 当前数据包在于固件位置
 * @param[in] {length} 当前固件包长度(固件包长度为0时,表示固件包发送完成)
 * @return Null
 * @note   MCU需要自行实现该功能
 */
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
    // #error "请自行完成MCU固件升级代码,完成后请删除该行"
    if(length == 0) {
        //固件数据发送完成
        /* 接受完成需要重启进入bootloader完成升级（上报新的版本），如果不重启，FC41D询问MCU版本，而MCU仍然回复历史版本，固件将会被重复下发五次 */
    }else {
        //固件数据处理
      
    }
    
    return SUCCESS;
}

/*
OTA示例参考代码
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
    static FILE *fp = NULL;

    if(length == 0) {
        //固件数据发送完成
        long size = ftell(fp);
        printf("=============================================================================Package Received OK! Total Length:%d Bytes(%d Kb)\r\n", size, size/1024);

        fclose(fp);
        fp = NULL;
        //这里强制终止进程，客户可以校验通过后，重启进入bootloader，如果不重启，FC41D询问MCU版本，而MCU仍然回复"1.0.0"，那么会造成固件重复下发。
        exit(0);
    }else {
        if(fp == NULL)
            fp = fopen("mcu_firmware.bin", "wb");

        fseek(fp, position, SEEK_SET);
        fwrite(value, length, 1, fp);

        //固件数据处理
        if(position == 0)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! This is a new Transmission !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
        }
        printf("=============================================================================Frame Received Offset:%d -- length:%d\r\n", position, length);
    }

    return SUCCESS;
}
*/
#endif

/**
 * @brief  tsl下发处理函数
 * @param[in] {tslid} tslid 序号
 * @param[in] {value} tsl数据缓冲区地址
 * @param[in] {length} tsl数据长度
 * @return tsl处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */
unsigned char tsl_download_handle(unsigned short tslid,const unsigned char value[], unsigned short length)
{
	/*********************************
	 当前函数处理可下发/可上报数据调用
	 具体函数内需要实现下发数据处理
	 完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
	 ***********************************/
	unsigned char ret = SUCCESS;
	switch(tslid) {
		case TSLID_ENERGY_TEMPER_F_VALUE:
			// 节能温度_f处理函数
			ret = tsl_download_energy_temper_f_handle(value, length);
		break;
		case TSLID_BACKLIGHT_VALUE:
			// 背光亮度处理函数
			ret = tsl_download_backlight_handle(value, length);
		break;
		case TSLID_MODE_VALUE:
			// 工作模式处理函数
			ret = tsl_download_mode_handle(value, length);
		break;
		case TSLID_LIGHT_BOOL:
			// 灯光处理函数
			ret = tsl_download_light_handle(value, length);
		break;
		case TSLID_ECO_BOOL:
			// ECO开关处理函数
			ret = tsl_download_eco_handle(value, length);
		break;
		case TSLID_ANION_BOOL:
			// 负离子处理函数
			ret = tsl_download_anion_handle(value, length);
		break;
		case TSLID_SOUND_BOOL:
			// 声音处理函数
			ret = tsl_download_sound_handle(value, length);
		break;
		case TSLID_WINDOW_CHECK_BOOL:
			// 开窗检测处理函数
			ret = tsl_download_window_check_handle(value, length);
		break;
		case TSLID_SLEEP_BOOL:
			// 睡眠开关处理函数
			ret = tsl_download_sleep_handle(value, length);
		break;
		case TSLID_FROST_BOOL:
			// 防霜冻开关处理函数
			ret = tsl_download_frost_handle(value, length);
		break;
		case TSLID_COMFORTABLE_TEMPER_VALUE:
			// 舒适温度处理函数
			ret = tsl_download_comfortable_temper_handle(value, length);
		break;
		case TSLID_ENERGY_TEMPER_VALUE:
			// 节能温度处理函数
			ret = tsl_download_energy_temper_handle(value, length);
		break;
		case TSLID_VALVE_CHECK_BOOL:
			// 阀门检测处理函数
			ret = tsl_download_valve_check_handle(value, length);
		break;
		case TSLID_SWITCH_VERTICAL_BOOL:
			// 上下摆风处理函数
			ret = tsl_download_switch_vertical_handle(value, length);
		break;
		case TSLID_SWITCH_HORIZONTAL_BOOL:
			// 左右摆风处理函数
			ret = tsl_download_switch_horizontal_handle(value, length);
		break;
		case TSLID_FAN_VERTICAL_VALUE:
			// 上下摆风角度处理函数
			ret = tsl_download_fan_vertical_handle(value, length);
		break;
		case TSLID_FAN_HORIZONTAL_VALUE:
			// 左右摆动角度处理函数
			ret = tsl_download_fan_horizontal_handle(value, length);
		break;
		case TSLID_TEMP_SET_VALUE:
			// 目标温度处理函数
			ret = tsl_download_temp_set_handle(value, length);
		break;
		case TSLID_TEMP_SET_F_VALUE:
			// 目标温度_F处理函数
			ret = tsl_download_temp_set_f_handle(value, length);
		break;
		case TSLID_TEMP_UNIT_VALUE:
			// 温标切换处理函数
			ret = tsl_download_temp_unit_handle(value, length);
		break;
		case TSLID_UPPER_TEMP_VALUE:
			// 设置温度上限处理函数
			ret = tsl_download_upper_temp_handle(value, length);
		break;
		case TSLID_UPPER_TEMP_F_VALUE:
			// 设置温度上限_F处理函数
			ret = tsl_download_upper_temp_f_handle(value, length);
		break;
		case TSLID_LOWER_TEMP_VALUE:
			// 设置温度下限处理函数
			ret = tsl_download_lower_temp_handle(value, length);
		break;
		case TSLID_LOWER_TEMP_F_VALUE:
			// 设置温度下限_F处理函数
			ret = tsl_download_lower_temp_f_handle(value, length);
		break;
		case TSLID_TEMP_CORRECTION_VALUE:
			// 温度校准处理函数
			ret = tsl_download_temp_correction_handle(value, length);
		break;
		case TSLID_LOCK_BOOL:
			// 童锁处理函数
			ret = tsl_download_lock_handle(value, length);
		break;
		case TSLID_COUNTDOWN_VALUE:
			// 倒计时处理函数
			ret = tsl_download_countdown_handle(value, length);
		break;
		case TSLID_LEVEL_VALUE:
			// 档位处理函数
			ret = tsl_download_level_handle(value, length);
		break;
		case TSLID_FACTORY_RESET_BOOL:
			// 恢复出厂设置处理函数
			ret = tsl_download_factory_reset_handle(value, length);
		break;
		case TSLID_WEEK_PROGRAM_STRING:
			// 周程序处理函数
			ret = tsl_download_week_program_handle(value, length);
		break;
		case TSLID_COMFORTABLE_TEMPER_F_VALUE:
			// 舒适温度_f处理函数
			ret = tsl_download_comfortable_temper_f_handle(value, length);
		break;
		default:
			break;
	}
	return ret;
}


/**
 * @brief  获取所有tsl命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_download_cmd_total(void)
{
    return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}


/******************************************************************************
                                WARNING!!!                     
此代码为SDK内部调用,请按照实际tsl数据实现函数内部数据
******************************************************************************/

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  获取到的格林时间
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_greentime(unsigned char time[])
{
    // #error "请自行完成相关代码,并删除该行"
    /*
    time[0] 为是否获取时间成功标志，为 0 表示失败，为 1表示成功
    time[1] 为年份，0x00 表示 2000 年
    time[2] 为月份，从 1 开始到12 结束
    time[3] 为日期，从 1 开始到31 结束
    time[4] 为时钟，从 0 开始到23 结束
    time[5] 为分钟，从 0 开始到59 结束
    time[6] 为秒钟，从 0 开始到59 结束
    */
    if(time[0] == 1) {
        //正确接收到wifi模块返回的格林数据
        
    }else {
        //获取格林时间出错,有可能是当前wifi模块未联网
    }
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCU校对本地RTC时钟
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_write_rtctime(unsigned char time[])
{
    // #error "请自行完成RTC时钟写入代码,并删除该行"
    /*
    Time[0] 为是否获取时间成功标志，为 0 表示失败，为 1表示成功
    Time[1] 为年份，0x00 表示 2000 年
    Time[2] 为月份，从 1 开始到12 结束
    Time[3] 为日期，从 1 开始到31 结束
    Time[4] 为时钟，从 0 开始到23 结束
    Time[5] 为分钟，从 0 开始到59 结束
    Time[6] 为秒钟，从 0 开始到59 结束
    Time[7] 为星期，从 1 开始到 7 结束，1代表星期一
   */
    if(time[0] == 1) {
        //正确接收到wifi模块返回的本地时钟数据
     
    }else {
        //获取本地时钟数据出错,有可能是当前wifi模块未联网
    }
}
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  wifi功能测试反馈
 * @param[in] {result} wifi功能测试结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @param[in] {rssi} 测试成功表示wifi信号强度/测试失败表示错误类型
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void wifi_test_result(unsigned char result,unsigned char rssi)
{
    // #error "请自行实现wifi功能测试成功/失败代码,完成后请删除该行"
    if(result == 0) {
        //测试失败
        if(rssi == 0x00) {
            //未扫描到名称为quectel_mdev_test路由器,请检查
        }else if(rssi == 0x01) {
            //模块未授权
        }
    }else {
        //测试成功
        //rssi为信号强度(0-100, 0信号最差，100信号最强)
    }
}
#endif


#ifdef MCU_TSL_UPLOAD_SYN
/**
 * @brief  状态同步上报结果
 * @param[in] {result} 结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_upload_syn_result(unsigned char result)
{
    #error "请自行完成状态同步上报结果代码,并删除该行"
      
    if(result == 0) {
        //同步上报出错
    }else {
        //同步上报成功
    }
}
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  获取 WIFI 状态结果
 * @param[in] {result} 指示 WIFI 工作状态
 * @ref       0x00: wifi状态 1 Wi-Fi 配网状态（蓝牙处于配网状态）
 * @ref       0x02: wifi状态 3 WIFI 已配置但未连上路由器
 * @ref       0x03: wifi状态 4 WIFI 已配置且连上路由器
 * @ref       0x04: wifi状态 5 已连上路由器且连接到云端
 * @ref       0x05: wifi状态 6 模组处于低功耗模式
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_wifi_status(unsigned char result)
{
//   #error "请自行完成获取 WIFI 状态结果代码,并删除该行"
    wifi_work_state = result;
    printf("result:%d\r\n", result);

    switch(result) {
        case 0:
            // wifi状态 1 Wi-Fi 配网状态（蓝牙处于配网状态）
        break;

        case 2:
            // wifi状态 3 WIFI 已配置但未连上路由器
        break;
        
        case 3:
            // wifi状态 4 WIFI 已配置且连上路由器
        break;
        
        case 4:
            // wifi状态 5 已连上路由器且连接到云端
        break;
        
        case 5:
            // wifi状态 6 模组处于低功耗模式
        break;
        
        default:break;
    }
}
#endif



#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  获取模块mac结果
 * @param[in] {mac} 模块 MAC 数据
 * @ref       mac[0]: 为是否获取mac成功标志，0x00 表示成功，0x01 表示失败
 * @ref       mac[1]~mac[6]: 当获取 MAC地址标志位如果mac[0]为成功，则表示模块有效的MAC地址
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_mac(unsigned char mac[])
{
    // #error "请自行完成mac获取代码,并删除该行"
    /*
    mac[0]为是否获取mac成功标志，0x00 表示成功，为0x01表示失败
    mac[1]~mac[6]:当获取 MAC地址标志位如果mac[0]为成功，则表示模块有效的MAC地址
   */
   
    if(mac[0] == 1) {
        //获取mac出错
    }else {
        //正确接收到wifi模块返回的mac地址
    }
}

/**
 * @brief  获取 BLE 状态结果
 * @param[in] {result} 指示 WIFI 工作状态
 * @ref       0x00: ble状态 1 设备未配网，蓝牙未连接
 * @ref       0x01: ble状态 2 设备未配网，蓝牙已连接
 * @ref       0x02: ble状态 3 设备已配网，蓝牙未连接
 * @ref       0x03: ble状态 4 设备已配网，蓝牙已连接
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_ble_status(unsigned char result)
{
//   #error "请自行完成获取 WIFI 状态结果代码,并删除该行"
    ble_work_state = result;
    printf("result:%d\r\n", result);

    switch(result) {
        case 0:
            // ble状态 1 设备未配网，蓝牙未连接
            break;

        case 1:
            // ble状态 2 设备未配网，蓝牙已连接
            break;

        case 2:
            // ble状态 3 设备已配网，蓝牙未连接
            break;

        case 3:
            // ble状态 4 设备已配网，蓝牙已连接
            break;


        default:break;
    }
}

/**
 * @brief  获取当前模组的IP地址
 * @param[in] {ip} 模块 IP 数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_ip_address(unsigned char ip[])
{
    /*
        模组未获取到IP地址，ip[1]的值为0x00
        模组获取到IP后，ip[1]~ip[N-1]:为当前模组的IP地址
        N = (*(ip-2)<<8 | *(ip-1)<<0) - 1;
    */
    uint16_t length = ((*(ip-2)) << 8 | (*(ip-1)) << 0) - 1;

    printf("IP Address Length:%d\r\n", length);
    if(ip[1] == 0x00) {
        printf("Get IP address error!\r\n");
        return;
    }else {
#if 0
        for(uint8_t i=1; i<=length; i++)
            printf("i:%d -- %c\r\n", i, ip[i]);
        printf("\r\n");
#endif
    }

    char ip_addr_buff[32] = {0};
    my_memcpy(ip_addr_buff, &ip[1], length);
    printf("length:%ld -- IP:%s\r\n", my_strlen((unsigned char *)ip_addr_buff), ip_addr_buff);
}

#endif