#include "voice.h"


const char *voice_buff[] = {
        "00001",        //第几组展示
        "00002",        //切换页面
        "00003",        //光照过高
        "00004",        //湿度过大
        "00005",        //
        "00006",        //
        "00007",        //
        "00008",        //
        "00009",        //
        "00010"         //
};

/*
*函数名称：Voice_Config()
*函数形参：u32 bound
*函数返回值：void
//////*函数功能：语音串口5初始化函数
//////*函数描述：PB10---串口3的TX    PB11----串口3的RX
 * 用串口4
 * PC10 -> UART4_TX
 * PC11 -> UART4_RX
*/
void Voice_Config(u32 bound) {
    USART_InitTypeDef USART_InitStruct = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    //开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    //PC10配置
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;        //复用推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;                //选择管脚12
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        //最高输出速率 50MHz
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    //PC11配置
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //浮空输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;                //选择管脚2
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    //UART5配置
    USART_InitStruct.USART_BaudRate = bound;            //波特率设置
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    //硬件控制流失能
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;    //接收发送使能
    USART_InitStruct.USART_Parity = USART_Parity_No;    //无奇偶校验
    USART_InitStruct.USART_StopBits = USART_StopBits_1;    //1停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;    //8数据位
    USART_Init(UART4, &USART_InitStruct);

    USART_Cmd(UART4, ENABLE);                        //使能串口
}

//上一曲
char Last_song[3] = {0xAA, 0x55, 0x00};
//下一曲
char Next_song[3] = {0xAA, 0x06, 0x00};
//播放
char Play_song[3] = {0xAA, 0x02, 0x00};
//音量


/*************************** 发送歌曲命令函数 **************************
函数功能：发送歌曲命令函数
参    数：命令数组  长度
返 回 值：无
1T = 1024G
1G = 1024MB
1MB = 1024KB
1KB = 1024B
1B = 8bit
B-----Byte
char----字符型----占1个字节  8bit
char----signed char ---有符号---+-
 有正负则最高位作为符号位  7bit  -2^7  ~2^7-1  -128 ~127
unsigned char ---无符号 0~2^8-1   0~255
**************************************************************************/
void Send_Song_CMD(u8 *cmd, u8 cmd_len) {
    unsigned char i = 0;  //循环变量
    unsigned char check = 0; //校验和

    for (i = 0; i < cmd_len; i++) {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);//轮询检测发送数据寄存器是否为空
        check += *cmd; //计算校验和
        USART_SendData(UART4, *cmd++); //发送数据
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);//轮询检测发送数据寄存器是否为空
    USART_SendData(UART4, check); //发送数据
}

/*************************** 发送音量命令函数 **************************
函数功能：发送音量命令函数
参    数：音量大小 0---30
返 回 值：无
**************************************************************************/
void Set_Song_Volume(unsigned char volume_num) {
    char Appoint_Play_Song[5] = {0xAA, 0x13, 0x01, 0x00, 0x00};
    //指定歌曲进行赋值
    Appoint_Play_Song[3] = volume_num;
    Send_Song_CMD((u8 *) Appoint_Play_Song, sizeof(Appoint_Play_Song));
}

/*************************** 发送指定歌曲命令函数 **********************
函数功能：发送指定歌曲命令函数
参    数：曲目数1---65535
返 回 值：无
**************************************************************************/
void Appoint_Song_Volume(unsigned char Song_num) {
    //指定歌曲数组
    char Appoint_Play_Song[5] = {0xAA, 0x07, 0x02, 0x00, 0x00};
    //指定歌曲进行赋值
    Appoint_Play_Song[4] = Song_num;
    Send_Song_CMD((u8 *) Appoint_Play_Song, sizeof(Appoint_Play_Song));
}

/*************************** 发送指定歌曲命令函数 **********************
函数功能：发送指定歌曲命令函数
参    数：歌名不用带格式  合成.wav  以--->合成
返 回 值：无
举例：Flash /00007*???   名字为00007歌曲
**************************************************************************/
void Appoint_Song_Name(unsigned char *Song_Name) {
    //指定歌曲数组注意Appoint_Song_Name[2]默认为0x02内部flash
    char Appoint_Song_Name[40] = {0xAA, 0x08, 0x00, 0x02};
    char Temp_Data[20] = {0};//"00099"
    sprintf(Temp_Data, "/%s*???\0", Song_Name);
    Appoint_Song_Name[2] = strlen(Temp_Data) + 1;   //长 度 ＝ 盘 符 长 度 + 路 径 长 度 = 1 + 路 径 长 度
    strcpy(Appoint_Song_Name + 4, Temp_Data);
    //指定歌曲进行赋值
    Send_Song_CMD((u8 *) Appoint_Song_Name, strlen(Appoint_Song_Name));
}




