#include <linMasterTask.h>
#include "key.h"
#include <applicationTask.h>



#if LIN_MASTER_EN == 1




#define USER_LIN_MSG_READ_DATA_LIN_ID   FYB
#define NAD_ID							0x01
#define SETFID							setFID
#define setFID							0x2c

#define FLDOOR_ID   					0x2c
#define FRDOOR_ID   					0x2D
#define RLDOOR_ID   					0x2E
#define RRDOOR_ID   					0x2F
#define FYB         					0x30

#define ReceiveNADNum		4
#define linmSendslaveNADMax		0x14	//0x11 0x12 0x13 0x14 分配的4个地址
#define linmSendslaveNADMin		0x11


void linm10msTimerExpired(SoftTimer_t *timer);
void linmSendTimerExpired(SoftTimer_t *timer);
static void ReceiveSlaveNADPaddingData(void);
static void KeyTimerExpired(SoftTimer_t *timer);
static void linmSend(void);
static void sendData(uint8_t table);


TaskState_t linmTaskState = TASK_STATE_INIT;
LIN_Device_Frame_t activelinmFrame;


static uint8_t timer10MS = 0;
static uint8_t timer10msFlag = 0;
static uint8_t commandCount = 0;
static uint8_t index = 0;
static uint8_t linmSendFlag =0;
static uint8_t ReceiveSlaveNADFlag =0;	//接收到从机NAD的标志位
static uint8_t ChangeFiddata[8];
static uint8_t SendTable = 0;			//决定了主节点发送的FID: 0->3D接收; 1->3C发送; 4->24改变颜色亮度;
static uint8_t linmLedState =0;	//点亮板的灯光颜色？
static uint8_t linmSendModeFlag = 0;
static uint8_t linmSendColorFlag = 0;
static uint8_t linmSendbrightnessFlag = 0;
static uint8_t SetColorTemp = 0;
static uint8_t linmColorChangeTemp = 0; 
static uint8_t linmSetbrightness = 0; 
static uint16_t brightness = 0;
static uint8_t slaveNAD = 0x01;
static uint32_t key;
static uint16_t Sendtemp = 0;		//控制报文发送的数量 每15ms--
static uint8_t linmSendslaveNAD; 	//给从机分配的地址从?开始
static uint8_t ReceiveSlaveNAD[ReceiveNADNum]={0,0,0};
static uint8_t ReceiveFlag =0;
static uint8_t ReceiveSlaveNADNum =0;	//接收到的从机NAD数量 即成功分配的从机数量 0~4
static uint16_t receivedTimerNum = 0;	//接受从节点数据的超时定时器？
static uint32_t old_crc = 0xffff;
static uint8_t linmSendSta=0;

volatile uint8_t linmState = 0;

extern uint8_t state;



LIN_Device_Frame_t linm_cmd_table[] = {
    [0] = {
        .frame_id = 0x3D,
        .msg_type = LIN_MSG_TYPE_RX,
        .checksum = LIN_CHECKSUM_CLASSIC,
        .length   = LIN_BUFF_SIZE,
    },
    [1] = {
        .frame_id = 0x3C,
        .msg_type = LIN_MSG_TYPE_TX,
        .checksum = LIN_CHECKSUM_CLASSIC,
        .length   = LIN_BUFF_SIZE,
    },
    [2] = {
        .frame_id = 0x3E,
        .msg_type = LIN_MSG_TYPE_RX,
        .checksum = LIN_CHECKSUM_ENHANCED,
        .length   = LIN_BUFF_SIZE,
    },
    [3] = {
 //       .frame_id = 0x00,
        .msg_type = LIN_MSG_TYPE_TX,
        .checksum = LIN_CHECKSUM_CLASSIC,
        .length   = 0,
    },
    [4] = {
        .frame_id = 0x24,
        .msg_type = LIN_MSG_TYPE_TX,
        .checksum = LIN_CHECKSUM_ENHANCED,
        .length   = LIN_BUFF_SIZE,
    },

};

const RGB_t rgbTable[COLOR_TABLE_SIZE] = {
{0x00,0x00,0xff},//11
{0x04,0xff,0x00},//22
{0xff,0x11,0x00},//32
{0xff,0xea,0x00},//28
{0x25,0xf9,0xfe},//48
{0xfc,0x00,0xf4},//5
{0xff,0xff,0xff},//227
/*
{0xff,0x00,0x15},//1
{0xff,0x00,0x48},//2
{0xff,0x00,0x88},//3
{0xff,0x0a,0xc9},//4
{0xfc,0x00,0xf4},//5
{0xea,0x00,0xff},//6
{0xbf,0x00,0xff},//7
{0x88,0x00,0xff},//8
{0x51,0x00,0xff},//9
{0x22,0x00,0xff},//10
{0x00,0x00,0xff},//11
{0x00,0x26,0xff},//12
{0x00,0x59,0xff},//13
{0x00,0x99,0xff},//14
{0x00,0xd0,0xff},//15
{0x00,0xf8,0xfc},//16
{0x00,0xff,0xe6},//17
{0x00,0xff,0xb7},//18
{0x00,0xff,0x84},//19
{0x00,0xff,0x4c},//20
{0x00,0xff,0x1e},//21
{0x04,0xff,0x00},//22
{0x26,0xff,0x00},//23
{0x59,0xff,0x00},//24
{0x95,0xff,0x00},//25
{0xcc,0xff,0x00},//26
{0xf2,0xff,0x00},//27
{0xff,0xea,0x00},//28
{0xff,0xb7,0x00},//29
{0xff,0x7b,0x00},//30
{0xff,0x40,0x00},//31
{0xff,0x11,0x00},//32
{0xff,0x25,0x35},//33
{0xff,0x25,0x62},//34
{0xff,0x25,0x99},//35
{0xff,0x25,0xcf},//36
{0xfe,0x25,0xf6},//37
{0xec,0x25,0xff},//38
{0xc6,0x25,0xff},//39
{0x99,0x25,0xff},//40
{0x6b,0x25,0xff},//41
{0x42,0x25,0xff},//42
{0x28,0x27,0xff},//43
{0x25,0x44,0xff},//44
{0x25,0x72,0xff},//45
{0x25,0xa7,0xff},//46
{0x25,0xd8,0xff},//47
{0x25,0xf9,0xfe},//48
{0x25,0xff,0xe8},//49
{0x25,0xff,0xc1},//50
{0x25,0xff,0x94},//51
{0x25,0xff,0x65},//52
{0x25,0xff,0x3e},//53
{0x29,0xff,0x27},//54
{0x46,0xff,0x25},//55
{0x72,0xff,0x25},//56
{0xa4,0xff,0x25},//57
{0xd2,0xff,0x25},//58
{0xf5,0xff,0x25},//59
{0xff,0xec,0x25},//60
{0xff,0xc1,0x25},//61
{0xff,0x8e,0x25},//62
{0xff,0x5b,0x25},//63
{0xff,0x32,0x25},//64
{0xff,0x46,0x54},//65
{0xff,0x46,0x7a},//66
{0xff,0x46,0xa9},//67
{0xff,0x46,0xd6},//68
{0xff,0x46,0xf8},//69
{0xef,0x46,0xff},//70
{0xcf,0x46,0xff},//71
{0xa9,0x46,0xff},//72
{0x81,0x46,0xff},//73
{0x5f,0x46,0xff},//74
{0x49,0x49,0xff},//75
{0x46,0x61,0xff},//76
{0x46,0x88,0xff},//77
{0x46,0xb5,0xff},//78
{0x46,0xde,0xff},//79
{0x46,0xfa,0xfe},//80
{0x46,0xff,0xec},//81
{0x46,0xff,0xcb},//82
{0x46,0xff,0xa4},//83
{0x46,0xff,0x7d},//84
{0x46,0xff,0x5c},//85
{0x4a,0xff,0x48},//86
{0x63,0xff,0x46},//87
{0x88,0xff,0x46},//88
{0xb2,0xff,0x46},//89
{0xd9,0xff,0x46},//90
{0xf7,0xff,0x46},//91
{0xff,0xef,0x46},//92
{0xff,0xcb,0x46},//93
{0xff,0x9f,0x46},//94
{0xff,0x74,0x46},//95
{0xff,0x52,0x46},//96
{0xff,0x6d,0x78},//97
{0xff,0x6d,0x95},//98
{0xff,0x6d,0xbb},//99
{0xff,0x6d,0xdf},//100
{0xff,0x6d,0xf9},//101
{0xf2,0x6d,0xff},//102
{0xd9,0x6d,0xff},//103
{0xbb,0x6d,0xff},//104
{0x9b,0x6d,0xff},//105
{0x80,0x6d,0xff},//106
{0x6f,0x6f,0xff},//107
{0x6d,0x81,0xff},//108
{0x6d,0xa1,0xff},//109
{0x6d,0xc4,0xff},//110
{0x6d,0xe5,0xff},//111
{0x6d,0xfb,0xfe},//112
{0x6d,0xff,0xf0},//113
{0x6d,0xff,0xd6},//114
{0x6d,0xff,0xb7},//115
{0x6d,0xff,0x98},//116
{0x6d,0xff,0x7d},//117
{0x6f,0xff,0x6e},//118
{0x83,0xff,0x6d},//119
{0xa1,0xff,0x6d},//120
{0xc2,0xff,0x6d},//121
{0xe1,0xff,0x6d},//122
{0xf9,0xff,0x6d},//123
{0xff,0xf2,0x6d},//124
{0xff,0xd6,0x6d},//125
{0xff,0xb3,0x6d},//126
{0xff,0x91,0x6d},//127
{0xff,0x76,0x6d},//128
{0xff,0x93,0x9c},//129
{0xff,0x93,0xb1},//130
{0xff,0x93,0xcd},//131
{0xff,0x93,0xe7},//132
{0xff,0x93,0xfb},//133
{0xf6,0x93,0xff},//134
{0xe3,0x93,0xff},//135
{0xcd,0x93,0xff},//136
{0xb6,0x93,0xff},//137
{0xa2,0x93,0xff},//138
{0x95,0x95,0xff},//139
{0x93,0xa3,0xff},//140
{0x93,0xba,0xff},//141
{0x93,0xd4,0xff},//142
{0x93,0xec,0xff},//143
{0x93,0xfc,0xfe},//144
{0x93,0xff,0xf4},//145
{0x93,0xff,0xe1},//146
{0x93,0xff,0xca},//147
{0x93,0xff,0xb3},//148
{0x93,0xff,0xa0},//149
{0x95,0xff,0x94},//150
{0xa4,0xff,0x93},//151
{0xba,0xff,0x93},//152
{0xd2,0xff,0x93},//153
{0xe9,0xff,0x93},//154
{0xfa,0xff,0x93},//155
{0xff,0xf6,0x93},//156
{0xff,0xe1,0x93},//157
{0xff,0xc7,0x93},//158
{0xff,0xae,0x93},//159
{0xff,0x9a,0x93},//160
{0xff,0xb9,0xbf},//161
{0xff,0xb9,0xcd},//162
{0xff,0xb9,0xdf},//163
{0xff,0xb9,0xf0},//164
{0xff,0xb9,0xfc},//165
{0xf9,0xb9,0xff},//166
{0xed,0xb9,0xff},//167
{0xdf,0xb9,0xff},//168
{0xd0,0xb9,0xff},//169
{0xc3,0xb9,0xff},//170
{0xba,0xba,0xff},//171
{0xb9,0xc3,0xff},//172
{0xb9,0xd2,0xff},//173
{0xb9,0xe3,0xff},//174
{0xb9,0xf3,0xff},//175
{0xb9,0xfd,0xff},//176
{0xb9,0xff,0xf8},//177
{0xb9,0xff,0xec},//178
{0xb9,0xff,0xdd},//179
{0xb9,0xff,0xce},//180
{0xb9,0xff,0xc2},//181
{0xbb,0xff,0xba},//182
{0xc4,0xff,0xb9},//183
{0xd2,0xff,0xb9},//184
{0xd2,0xff,0x93},//185
{0xf1,0xff,0xb9},//186
{0xfc,0xff,0xb9},//187
{0xff,0xf9,0xb9},//188
{0xff,0xec,0xb9},//189
{0xff,0xdb,0xb9},//190
{0xff,0xcb,0xb9},//191
{0xff,0xbe,0xb9},//192
{0xff,0xdb,0xde},//193
{0xff,0xdb,0xe5},//194
{0xff,0xdb,0xee},//195
{0xff,0xdb,0xf7},//196
{0xff,0xdb,0xfe},//197
{0xfc,0xdb,0xff},//198
{0xf6,0xdb,0xff},//199
{0xee,0xdb,0xff},//200
{0xe6,0xdb,0xff},//201
{0xe0,0xdb,0xff},//202
{0xdb,0xdb,0xff},//203
{0xdb,0xe0,0xff},//204
{0xdb,0xe7,0xff},//205
{0xdb,0xf1,0xff},//206
{0xdb,0xf9,0xff},//207
{0xdb,0xfe,0xff},//208
{0xdb,0xff,0xfb},//209
{0xdb,0xff,0xf5},//210
{0xdb,0xff,0xed},//211
{0xdb,0xff,0xe5},//212
{0xdb,0xff,0xdf},//213
{0xdc,0xff,0xdb},//214
{0xe0,0xff,0xdb},//215
{0xe7,0xff,0xdb},//216
{0xd2,0xff,0x93},//217
{0xf8,0xff,0xdb},//218
{0xfd,0xff,0xdb},//219
{0xff,0xfc,0xdb},//220
{0xff,0xf5,0xdb},//221
{0xff,0xec,0xdb},//222
{0xff,0xe4,0xdb},//223
{0xff,0xdd,0xdb},//224
{0xff,0xf5,0xf5},//225
{0xff,0xf5,0xf7},//226
{0xff,0xff,0xff},//227
{0xfd,0xf5,0xff},//228
{0xfa,0xf5,0xff},//229
{0xf8,0xf5,0xff},//230
{0xf6,0xf5,0xff},//231
{0xf5,0xf5,0xff},//232
{0xf5,0xf6,0xff},//233
{0xf5,0xf8,0xff},//234
{0xf5,0xfb,0xff},//235
{0xf5,0xfd,0xff},//236
{0xf5,0xff,0xff},//237
{0xf5,0xff,0xfe},//238
{0xf5,0xff,0xfc},//239
{0xf5,0xff,0xfa},//240
{0xf5,0xff,0xf7},//241
{0xf5,0xff,0xf6},//242
{0xf5,0xff,0xf5},//243
{0xf6,0xff,0xf5},//244
{0xf8,0xff,0xf5},//245
{0xfb,0xff,0xf5},//246
{0xfd,0xff,0xf7},//247
{0xff,0xff,0xf7},//248
{0xff,0xfe,0xf5},//249
{0xff,0xfc,0xf5},//250
{0xff,0xfa,0xf5},//251
{0xff,0xf7,0xf5},//252
{0xff,0xf5,0xf5},//253
*/
};


static SoftTimer_t keyTimer = {
    .mode = TIMER_PERIODIC_MODE,
    .interval = 20U,
    .handler = KeyTimerExpired};		// 按键检测计时器


static SoftTimer_t linmSendTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = 10U,
    .handler  = linmSendTimerExpired	// 每10ms发送填充的数据，并将sendtemp--以控制发送次数
};

static SoftTimer_t linm10msTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = 10U,
    .handler  = linm10msTimerExpired	// 每10ms调用自动地址填充函数
};



static void sendData(uint8_t table)
{
	Change_CTRL_t * ChangeCTRL = (Change_CTRL_t *)((void *)linm_cmd_table[table].data); 		//写从机
	
	for (uint8_t i = 0; i < 8U; i++)
	{
	  ChangeCTRL->data[i] = ChangeFiddata[i];
	}
	linmSendSta = LINM_SendFrame(&linm_cmd_table[table]);
}



static void linmSendSetColor()
{
                
		if(linmColorChangeTemp < COLOR_TABLE_SIZE -1)
			linmColorChangeTemp++;
		else
			linmColorChangeTemp =0;	
		ChangeFiddata [0] = 0x7f;
		ChangeFiddata [1] = 0x00;
		ChangeFiddata [2] = (uint8_t)brightness;			//亮度		
		ChangeFiddata [3] = 0x00;
		ChangeFiddata [4] = 0x00;
		ChangeFiddata [5] = rgbTable[linmColorChangeTemp].red;
		ChangeFiddata [6] = rgbTable[linmColorChangeTemp].green;
		ChangeFiddata [7] = rgbTable[linmColorChangeTemp].blue;
		Sendtemp = 65535;
		SendTable = 4;
		linmSendColorFlag = 0;


}


static void linmSendSetbrightness()
{

		if(brightness < 100)
			brightness = brightness+20;
		else
			brightness =0;
                
		ChangeFiddata [0] = 0x7f;
		ChangeFiddata [1] = 0x00;
		ChangeFiddata [2] = (uint8_t)brightness;			//亮度		
		ChangeFiddata [3] = 0x00;
		ChangeFiddata [4] = 0x00;
		ChangeFiddata [5] = rgbTable[linmColorChangeTemp].red;
		ChangeFiddata [6] = rgbTable[linmColorChangeTemp].green;
		ChangeFiddata [7] = rgbTable[linmColorChangeTemp].blue;

		Sendtemp = 65535;
		SendTable = 4;
		linmSendbrightnessFlag = 0;		
}

static void linm10msTimerExpired(SoftTimer_t *timer)
{
	static uint8_t Timer10ms = 0;


	if(linmSendFlag)
	{
		linmSend();
		setLEDstate(3);				//闪蓝灯
	}else
	{
		switch (linmLedState)
		{
		case 0 :
			setLEDstate(0);				//关灯
			break;
		case 1 :
			setLEDstate(1);				//亮红灯
		break;
		case 2 :
			setLEDstate(2);				//亮绿灯
			break;
		default :
			setLEDstate(0);				//关灯
			break;
		}
	}

	if(linmSendColorFlag)	//	按键控制此标志位以设置颜色
	{
		
		linmSendSetColor();
	}

	if(linmSendbrightnessFlag)	//	按键控制此标志位以设置亮度
	{
		linmSendSetbrightness();
	}


}	


static void KeyTimerExpired(SoftTimer_t *timer)
{
    key = Key_Read(50);
  switch (key)
  {
  case KEY_BTN1_VALUE:
    key = 0;
//	linmSendModeFlag =1;		//设置模式切换

	linmLedState = 1;				//亮红灯 
	old_crc = 0xffff;
	linmState = sendInit;
	ReceiveFlag =0;
	linmSendFlag = 1;				//设置NAD
	linmSendColorFlag =0;			//把设置颜色的标志位清0		
	linmSendbrightnessFlag=0;		//设置亮度切换的标志位清0		
	linmSendslaveNAD = linmSendslaveNADMin; 
	ReceiveSlaveNADNum=0;

    break;

  case KEY_BTN2_VALUE:
    key = 0;
	
 	linmSendColorFlag =1;			//设置颜色切换
	linmSendbrightnessFlag=0;		//设置亮度切换的标志位清0
	linmSendFlag = 0;				//把设置FAD/NAD的标志位清0

    break;

  case KEY_BTN3_VALUE:
    key = 0;
	linmSendbrightnessFlag=1;		//设置亮度切换
	linmSendFlag = 0;				//把设置FAD/NAD的标志位清0 
	linmSendColorFlag =0;			//把设置颜色的标志位清0		

    break;

  case KEY_BTN12_VALUE:
  	key = 0;


  	break;
	
  case KEY_BTN13_VALUE:				//按下1和3键，

  	break;
	
  case KEY_BTN23_VALUE:
	key = 0;
  	break;

  case KEY_BTN1_LONG_RELAX_VALUE:			//KEY_BTN1长按释放
	key = 0;

   break;

  case KEY_BTN2_LONG_RELAX_VALUE:			//KEY_BTN2长按释放
	key = 0;
  
   break;

  case KEY_BTN3_LONG_RELAX_VALUE:			//KEY_BTN3长按释放
	key = 0;

   break;

  default:
    break;
  }
}




static void linmSendTimerExpired(SoftTimer_t *timer)	//15MS执行一次
{

	if((Sendtemp == 65535))
	{
		
		sendData(SendTable);	
	}
	else		
	if(Sendtemp>0)
	{
		Sendtemp --;		//	sendtemp在这里改变值 以控制下面的报文发送数量
		sendData(SendTable);		 
	}
}

static uint8_t PaddingDataState =0;
static uint8_t SendLEDTable =0;
static uint8_t rxNADNum = 0;


static void AutoAddressingPaddingData(void)
{
	static uint8_t SendStateFlag = 0;
	static uint32_t timertemp = 0;
	static uint8_t sendFidStateFlag = 0;


		switch (linmState)
		{
			case  sendInit:						
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
					ChangeFiddata [0] = 0x7F;
					ChangeFiddata [1] = 0x00;
					ChangeFiddata [2] = 0x00;
					ChangeFiddata [3] = 0x00;
					ChangeFiddata [4] = 0x00;
					ChangeFiddata [5] = 0x00;
					ChangeFiddata [6] = 0x00;
					ChangeFiddata [7] = 0x00;
				
												//发送30次唤醒MCU
					Sendtemp = 30;				//计数
					SendTable = 1;
				}
				if(Sendtemp == 0)				//计数器减到0就进入下个阶段
				{
					linmState = sendPojid;
					SendStateFlag = 0;
				}
				
			break;
			case  sendPojid:					//发送00步骤
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
					ChangeFiddata [0] = 0x7F;
					ChangeFiddata [1] = 0X00;
					ChangeFiddata [2] = 0x00;
					ChangeFiddata [3] = 0x00;
					ChangeFiddata [4] = 0x00;
					ChangeFiddata [5] = 0x00;
					ChangeFiddata [6] = 0x00;
					ChangeFiddata [7] = 0x00;

					Sendtemp = 1;
					SendTable = 1;
				}
				if(Sendtemp == 0)
				{
					linmState = send3E_1;
					SendStateFlag = 0;
				}
			break;

			case  send3E_1:						//发送01步骤 初始化 只执行一次
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
					ChangeFiddata [0] = 0x7F;
					ChangeFiddata [1] = 0x06;
					ChangeFiddata [2] = 0xB5;
					ChangeFiddata [3] = 0xFF;
					ChangeFiddata [4] = 0x7F;
					ChangeFiddata [5] = 0x01;
					ChangeFiddata [6] = 0x02;
					ChangeFiddata [7] = 0xFF;
					
					Sendtemp = 1;
					SendTable = 1;
				}
				if(Sendtemp == 0)
				{
					linmState = sendNAD;
					SendStateFlag = 0;		
				}
			break;

			case  sendNAD:						//发送02步骤 配置地址

				if(SendStateFlag ==0)
				{
					SendStateFlag = 1;

					ChangeFiddata [0] = 0x7F;
					ChangeFiddata [1] = 0x06;
					ChangeFiddata [2] = 0xB5;
					ChangeFiddata [3] = 0xFF;
					ChangeFiddata [4] = 0x7F;
					ChangeFiddata [5] = 0x02;
					ChangeFiddata [6] = 0x02;
					ChangeFiddata [7] = linmSendslaveNAD;	//	配置地址
					Sendtemp = 1;
					SendTable = 1;
								
				}

				if(Sendtemp == 0)
				{
					linmState = sendRead;	
					SendStateFlag = 0;
			
				}
			break;

			case  sendRead:						//发送03步骤保存NAD
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
					ChangeFiddata [0] = linmSendslaveNAD;	// 保存地址 这里要求做成7F通配符？
					ChangeFiddata [1] = 0x06;
					ChangeFiddata [2] = 0xB5;
					ChangeFiddata [3] = 0xFF;
					ChangeFiddata [4] = 0x7F;
					ChangeFiddata [5] = 0x03;
					ChangeFiddata [6] = 0x02;
					ChangeFiddata [7] = 0xFF;	//01 -> FF
					Sendtemp = 1;
					SendTable = 1;	
				}
				if(Sendtemp == 0)
				{
					linmState = send3E_2;
					SendStateFlag = 0;
				}
				break;
				case  sendSynchronous:				//发送同步间隔
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
			/*		ChangeFiddata [0] = 0x00;
					ChangeFiddata [1] = 0x00;
					ChangeFiddata [2] = 0x00;
					ChangeFiddata [3] = 0x00;
					ChangeFiddata [4] = 0x00;
					ChangeFiddata [5] = 0x00;
					ChangeFiddata [6] = 0x00;
					ChangeFiddata [7] = 0x00;
			*/		
					Sendtemp = 1;
					SendTable = 3;	
				}
				if(Sendtemp == 0)
				{
					linmState = send3E_2;	
					SendStateFlag = 0;
				}
			break;			
			case  send3E_2:						//发送04步骤 配置结束
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;
					ChangeFiddata [0] = linmSendslaveNAD;	//	配置结束 这里要求做成通配符?
					ChangeFiddata [1] = 0x06;
					ChangeFiddata [2] = 0xB5;
					ChangeFiddata [3] = 0xFF;
					ChangeFiddata [4] = 0x7F;
					ChangeFiddata [5] = 0x04;
					ChangeFiddata [6] = 0x02;
					ChangeFiddata [7] = 0xFF;
					Sendtemp = 1;
					SendTable = 1;	
				}
				if(Sendtemp == 0)
				{
					linmState = send4;
					SendStateFlag = 0;
				}

			break;
			case  send4:
				if(SendStateFlag == 0)
				{
					SendStateFlag = 1;					//发送05步骤 查询地址
					ChangeFiddata [0] = linmSendslaveNAD;
					ChangeFiddata [1] = 0x03;
					ChangeFiddata [2] = 0x22;
					ChangeFiddata [3] = 0x00;
					ChangeFiddata [4] = 0x03;
					ChangeFiddata [5] = 0xFF;
					ChangeFiddata [6] = 0xFF;
					ChangeFiddata [7] = 0xFF;	
					Sendtemp = 1;
					SendTable = 1;	
				}

				if(Sendtemp == 0)
				{								
					linmState = send3d;				
					SendStateFlag = 0;
				}
				break;
				case  send3d:		// FID为3D 即准备从从机接收8个字节 接收到的FID放在第1个和第6个字节里
					if(SendStateFlag == 0)
					{
						SendStateFlag = 1;
						ChangeFiddata [0] = 0x00;
						ChangeFiddata [1] = 0x00;
						ChangeFiddata [2] = 0x00;
						ChangeFiddata [3] = 0x00;
						ChangeFiddata [4] = 0x00;
						ChangeFiddata [5] = 0x00;
						ChangeFiddata [6] = 0x00;
						ChangeFiddata [7] = 0x00;
						
						Sendtemp = 1;					//发送1次3D读NAD
						SendTable = 0;					
					}


					if(Sendtemp == 0)					//发送完数据		
					{		
						receivedTimerNum++;	// 接收从机数据的超时定时器？
						if((0==ReceiveFlag)&&(receivedTimerNum < 100))			//等待从机返回
						{
							return;
						}
						
						//收到从机返回正确的NAD标志位					
						if((ReceiveSlaveNADNum == 1)||(ReceiveSlaveNADNum == 2)||(ReceiveSlaveNADNum == 3)||(ReceiveSlaveNADNum == 4))				
						{
																					
							ReceiveFlag =0;
							ReceiveSlaveNADFlag =1; 		// 表示收到了从机的FID了					
							ReceiveSlaveNADPaddingData();	// 进去给响应的从机点灯了
							if(Sendtemp)
							return;
							


							if(linmSendslaveNAD<=linmSendslaveNADMax)
							{
								linmSendslaveNAD++;	// 地址在这里改变 0X11 0X12 0X13 0X14
								linmLedState = 1;	// 红灯

								receivedTimerNum =0;	// 超时定时器归零
								linmState = sendNAD;	// 跳过初始化阶段，从switch的配置地址步骤开始进行
								SendStateFlag =0;
							}
							 if(linmSendslaveNAD > linmSendslaveNADMax)
							{
								linmSendFlag = 0;									//退出
								linmState = sendInit;
								SendStateFlag = 0;
								linmSendslaveNAD = linmSendslaveNADMin;
								ReceiveSlaveNADFlag =1; 
								uint8_t i=0;
								for(i=0;i<ReceiveNADNum;i++)
								{
									ReceiveSlaveNAD[i]=0;
								}
							
								
							}
							Sendtemp =0;

							if(ReceiveSlaveNADNum == 4)
							{
								linmLedState = 2;	//绿灯
								
							}
							else
							{
								linmLedState = 1;	//红灯
								
							}
						}
						
						//超时没有收到从机返回正确的NAD标志位
						else if((0==ReceiveFlag)&&(0 ==ReceiveSlaveNADNum))								
						{		
							SendStateFlag = 0;
							receivedTimerNum =0;
							linmSendFlag = 0;									//退出
							linmState = sendInit;
							Sendtemp =0;
							linmSendslaveNAD = linmSendslaveNADMin;
							ReceiveSlaveNADFlag =1; 
							uint8_t i=0;
							for(i=0;i<ReceiveNADNum;i++)
							{
								ReceiveSlaveNAD[i]=0;
							}

						}	

							
					}
				break;
					
			default:
				
			break;
		}

}



static void ReceiveSlaveNADPaddingData(void)
{

	static uint16_t timerNumber =0;
	static uint8_t StateFlag =0;


	if(ReceiveSlaveNADFlag == 1)
	{
		if(ReceiveSlaveNADNum ==1)	
		PaddingDataState =0;
		else if(ReceiveSlaveNADNum == 2)
		PaddingDataState =1;
		else if(ReceiveSlaveNADNum ==  3)
		PaddingDataState =2;
		else if(ReceiveSlaveNADNum ==  4)
		PaddingDataState =3;

		
		switch (PaddingDataState)
		{
			case 0:
				if(StateFlag ==0)
				{
					StateFlag =1;
						//	点灯(红色)
						ChangeFiddata [0] = ReceiveSlaveNAD[0];
						ChangeFiddata [1] = 0x00;
						ChangeFiddata [2] = 0x64;
						ChangeFiddata [3] = 0x00;
						ChangeFiddata [4] = 0x00;
						ChangeFiddata [5] = 0xff;
						ChangeFiddata [6] = 0x00;
						ChangeFiddata [7] = 0x15;
						Sendtemp = 2;
						SendTable = 4;
					
									
				}
	
				
				if(Sendtemp ==0)
				{
					ReceiveSlaveNADFlag =0; 
					StateFlag =0;
				}

			break;	

			case 1:
				if(StateFlag ==0)
				{
					StateFlag =1;
						
						//	点灯(绿色)
						ChangeFiddata [0] = ReceiveSlaveNAD[1];
						ChangeFiddata [1] = 0x00;
						ChangeFiddata [2] = 0x64;
						ChangeFiddata [3] = 0x00;
						ChangeFiddata [4] = 0x00;
						ChangeFiddata [5] = 0x04;
						ChangeFiddata [6] = 0xff;
						ChangeFiddata [7] = 0x00;
						Sendtemp = 2;
						SendTable = 4;
													
				}
				if(Sendtemp ==0)
				{
					ReceiveSlaveNADFlag =0; 
					StateFlag =0;
				}
				
			break;

			case 2:
				if(StateFlag ==0)
				{
					StateFlag =1;
						//	点灯(蓝色)
						ChangeFiddata [0] = ReceiveSlaveNAD[2];
						ChangeFiddata [1] = 0x00;
						ChangeFiddata [2] = 0x64;
						ChangeFiddata [3] = 0x00;
						ChangeFiddata [4] = 0x00;
						ChangeFiddata [5] = 0x00;
						ChangeFiddata [6] = 0x00;
						ChangeFiddata [7] = 0xff;
						Sendtemp = 2;
						SendTable = 4;
				
				}
				if(Sendtemp ==0)
				{
					ReceiveSlaveNADFlag =0; 
					StateFlag =0;
				}				
			break;
			case 3:
				if(StateFlag ==0)
				{
					StateFlag =1;
						//	点灯(白色)
						ChangeFiddata [0] = ReceiveSlaveNAD[3];
						ChangeFiddata [1] = 0x00;
						ChangeFiddata [2] = 0x64;
						ChangeFiddata [3] = 0x00;
						ChangeFiddata [4] = 0x00;
						ChangeFiddata [5] = 0xdb;
						ChangeFiddata [6] = 0xff;
						ChangeFiddata [7] = 0xe5;
						Sendtemp = 2;
						SendTable = 4;
				
				}
				if(Sendtemp ==0)
				{
					ReceiveSlaveNADFlag =0; 
					StateFlag =0;
				}				
			break;

			default:
			break;
		}					
	}
}


static void linmSend(void)
{
	AutoAddressingPaddingData();

}



static void LIN_Master_ISR(LIN_Device_Frame_t const *frame)
{
    activelinmFrame = *frame;
    TM_PostTask(TASK_ID_LINM);
}


//	处理从从机接收到的报文
static void hanldeLinCommands(LIN_Device_Frame_t *frame)
{
    // Handle received command

	static uint8_t receivedstr = 0;
	static uint8_t i = 0;
	Change_CTRL_t * rxDataFromSlave;
	
    CRC32_Init();

	if(frame->data != NULL)
	{
		uint32_t temp = CRC32_GetRunTimeCRC32((void *)frame->data, LIN_BUFF_SIZE);
		if (temp != old_crc)
		{
		  	old_crc = temp;
			rxDataFromSlave = (Change_CTRL_t *)((void *)frame->data);
				
			if((rxDataFromSlave->strData.Reserved2== 0x09)&&(rxDataFromSlave->strData.Reserved3 == 0x62))
			{
				if(rxDataFromSlave->strData.Reserved6==linmSendslaveNAD)			//如果收到从机返回的NAD正确
				{								
					ReceiveFlag =1;					

					// 接收到的从机数量在这里改变
					ReceiveSlaveNAD[ReceiveSlaveNADNum++] = rxDataFromSlave->strData.Reserved6;	
					if(ReceiveSlaveNADNum>ReceiveNADNum)
					{
						ReceiveSlaveNADNum=0;					
						
					}
				}
			
				return;
			}
		}
	}
}

void LINM_TaskHandler(void)
{
    switch(linmTaskState){
    case TASK_STATE_ACTIVE:
      hanldeLinCommands(&activelinmFrame);

      break;
    case TASK_STATE_INIT:
      LINM_Init();
      LINM_RegisterLINM_IRQ(LIN_Master_ISR);
      SoftTimer_Start(&keyTimer);
	  SoftTimer_Start(&linmSendTimer);	 
      SoftTimer_Start(&linm10msTimer);	
      linmTaskState = TASK_STATE_ACTIVE;
      break;
    }
}

#endif
