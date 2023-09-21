#include "mqtt.h"

/*******************消息标识符 MSB********************/
#define Lable_ID_MSB  0x00
#define Lable_ID_LSB  0x01
/**********CONNACK--确认连接请求**服->客****************/
const u8 parket_connetAck[] = {0x20, 0x02, 0x00, 0x00};
/*************断开连接数据包**客->服********************/
const u8 parket_disconnet[] = {0xE0, 0x00};
/***********PING数据包(俗称心跳包)**客->服**************/
const u8 parket_heart[] = {0xC0, 0x00};
/**************PING数据响应包**服->客******************/
const u8 parket_heart_reply[] = {0xD0, 0x00};
/***************订阅确认数据包**服->客*****************/
const u8 parket_subAck[] = {0x90, 0x03, Lable_ID_MSB, Lable_ID_MSB};
/***************取消订阅确认数据包**服->客*****************/
const u8 parket_cancel_subAck[] = {0xB0, 0x02, Lable_ID_MSB, Lable_ID_MSB};


/*****************对结构体变量进行赋值*****************/
mqtt_struct MY_MQTT = {wifi.ReceiveBuff, 0, wifi.SendBuff, 0, Init, Connect, SubscribeTopic, PublishData, SentHeart,
                       Disconnect,};

/*****************************************************
函 数 名：static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen)
函数功能：MQTT初始化函数---清空
参    数：rx_buff：接收数组
					rx_len：接收长度
					tx_buff：发送数组
					tx_len：发送长度
返 回 值：无
******************************************************/
static void Init(u8 *rx_buff, u16 rx_len, u8 *tx_buff, u16 tx_len) {
    /*发送数组接收数组分别进行赋值/址*/
    MY_MQTT.rxbuf = (char *) rx_buff;
    MY_MQTT.rxlen = rx_len;
    MY_MQTT.txbuf = (char *) tx_buff;
    MY_MQTT.txlen = tx_len;
    /*清空数组*/
    memset(MY_MQTT.rxbuf, 0, MY_MQTT.rxlen);
    memset(MY_MQTT.txbuf, 0, MY_MQTT.txlen);
    /*无条件先主动断开*/
    MY_MQTT.Disconnect();
    Delay_ms(100);
    MY_MQTT.Disconnect();
    Delay_ms(100);
    MY_MQTT.rxlen = 0;
    MY_MQTT.txlen = 0;
}


/*****************************************************
函 数 名：static void Mqtt_SendBuf(u8 *buf,u16 len)
函数功能：MQTT发送数据函数
参    数：buf发送数据首地址
					len发送长度
返 回 值：无
******************************************************/
static void Mqtt_SendBuf(u8 *buf, u16 len) {
    memset(MY_MQTT.rxbuf, 0, sizeof((u8 *) MY_MQTT.rxbuf));
    Usart2_Send_n_Byte((char *) buf, len);
}


/*****************************************************
函 数 名：static u8 Connect(char *ClientID,char *Username,char *Password)
函数功能：连接CONNECT – 连接服务端
参    数：ClientID:客户端ID标识
					Username：用户名
					Password：密码
返 回 值：返回值
******************************************************/
static u8 Connect(char *ClientID, char *Username, char *Password) {
    u8 cnt = 3;
    u8 wait;
    char temp_value = 0;
    /*临时变量---剩余长度十进制数据*/
    int DataLen;
    /*计算客户端ID、用户名、密码分别长度*/
    int ClientIDLen = strlen(ClientID);
    int UsernameLen = strlen(Username);
    int PasswordLen = strlen(Password);
    /*清除发送长度*/
    MY_MQTT.txlen = 0;
    /*剩余长度等于可变报头的长度(10 字节)加上有效载荷的长度*/
    DataLen = 10 + (ClientIDLen + 2) + (UsernameLen + 2) + (PasswordLen + 2);
    /*固定报头----控制报文类型*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x10;
    /*固定报头----剩余长度(不包括固定头部)*/
    do {
        u8 encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        /*剩余长度--如果要编码的数据更多，请设置此字节的顶部位*/
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        MY_MQTT.txbuf[MY_MQTT.txlen++] = encodedByte;
    } while (DataLen > 0);
    /*可变报头----协议名*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x00;                        //协议名长度MSB  
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x04;                        //协议名长度LSB    
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 'M';                         //协议名M    
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 'Q';                         //协议名Q    
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 'T';                         //协议名T    
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 'T';                         //协议名T    
    /*可变报头----协议级别*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x04;                        //协议级别字段的值是 4(0x04)。   
    /*可变报头----连接标志*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0xc2;                        //连接标志 
    /*保持连接(Keep Alive)(保活包) 是一个以秒为单位的时间间隔*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x00;                                        // 保持连接 MSB
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x64;                                        // 保持连接LSB  100S心跳包
    /*有效载荷-----客户端 ID 字节长度*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(ClientIDLen);                    // 客户端 ID 字节长度MSB
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(ClientIDLen);                    // 客户端 ID 字节长度LSB
    /*有效载荷-----客户端ID*/
    memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], ClientID, ClientIDLen);
    MY_MQTT.txlen += ClientIDLen;                                                //偏移存储位置
    /*有效载荷-----用户名*/
    if (UsernameLen > 0) {
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(UsernameLen);           //用户名长度 MSB
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(UsernameLen);         //用户名长度 LSB
        memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], Username, UsernameLen);//用户名内容
        MY_MQTT.txlen += UsernameLen;
    }
    /*有效载荷-----密码*/
    if (PasswordLen > 0) {
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(PasswordLen);           //密码长度 MSB
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(PasswordLen);         //密码长度 LSB
        memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], Password, PasswordLen);//密码内容
        MY_MQTT.txlen += PasswordLen;
    }
    /*发送数据*/
    while (cnt--) {
        Mqtt_SendBuf((u8 *) MY_MQTT.txbuf, MY_MQTT.txlen);
        Processing_before_sending();
        wait = 100;//等待10s时间
        while (wait--) {
            /*串口2接收完成---进行对比*/
            if ((temp_value = Reception_Comparison(parket_connetAck, 4)) != (char) -1) {
                printf("temp_value:%d\r\n", temp_value);
                return temp_value;
            }
            Delay_ms(100);
        }
    }
    return 0;
}

/*****************************************************
函 数 名：u8 Reception_Comparison(const u8 * Comparison_Buff,u8 Comparison_Len)
函数功能：接收对比函数
参    数：Comparison_Buff:对比数组
					Comparison_Len：对比长度
返 回 值：1成功 0 失败
******************************************************/
char Reception_Comparison(const u8 *Comparison_Buff, u8 Comparison_Len) {
    char return_value = -1;
    if (wifi.FinishFlag) {
        return_value = strncmp((const char *) Comparison_Buff, (const char *) MY_MQTT.rxbuf, Comparison_Len);
        return_value == 0 ? (return_value = 1) : (return_value = 0);
        printf("%d对比数据:%02x %02x %02x %02x %02x\r\n", Comparison_Len, Comparison_Buff[0], Comparison_Buff[1],
               Comparison_Buff[2], Comparison_Buff[3], Comparison_Buff[4]);
        printf("%d接收数据:%02x %02x %02x %02x %02x\r\n", Comparison_Len, MY_MQTT.rxbuf[0], MY_MQTT.rxbuf[1],
               MY_MQTT.rxbuf[2], MY_MQTT.rxbuf[3], MY_MQTT.rxbuf[4]);
        Processing_before_sending();
    }
    return return_value;
}

/*****************************************************
函 数 名：void Processing_before_sending(void)
函数功能：MQTT发送前进行清空串口内数据
参    数：无 
返 回 值：无
******************************************************/
void Processing_before_sending(void) {
    wifi.FinishFlag = 0;
    wifi.CurrentTimer = 0;
    Clean_Recv_Data();
}

/*****************************************************
函 数 名：static u8 SubscribeTopic(char *topic,u8 qos,u8 whether)
函数功能：MQTT订阅/取消订阅数据打包函数
参    数：topic       主题 
					qos         消息等级 
					whether     订阅/取消订阅请求包
返 回 值：返回值
******************************************************/
static u8 SubscribeTopic(char *topic, u8 qos, u8 whether) {
    char temp_value = 0;
    int topiclen;
    int DataLen;
    u8 cnt = 2;
    u8 wait;
    MY_MQTT.txlen = 0;
    topiclen = strlen(topic);
    /*MQTT订阅/取消订阅数据打包函数------剩余长度*/
    DataLen = 2 + (topiclen + 2) + (whether ? 1 : 0);    //可变报头的长度（2字节）加上有效载荷的长度
    /*固定报头----控制报文类型*/
    if (whether) MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x82; //消息类型和标志订阅
    else MY_MQTT.txbuf[MY_MQTT.txlen++] = 0xA2;       //取消订阅
    /*固定报头----剩余长度*/
    do {
        u8 encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        /*剩余长度--如果要编码的数据更多，请设置此字节的顶部位*/
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        MY_MQTT.txbuf[MY_MQTT.txlen++] = encodedByte;
    } while (DataLen > 0);
    /*可变报头----剩余长度*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = Lable_ID_MSB;            //消息标识符 MSB
    MY_MQTT.txbuf[MY_MQTT.txlen++] = Lable_ID_LSB;      //消息标识符 LSB
    /*有效载荷----主题长度*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(topiclen);        //主题长度 MSB
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(topiclen);        //主题长度 LSB
    /*有效载荷----主题*/
    memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], topic, topiclen);
    MY_MQTT.txlen += topiclen;
    /*有效载荷----服务等级*/
    if (whether) {
        MY_MQTT.txbuf[MY_MQTT.txlen++] = qos;//QoS级别
    }
    while (cnt--) {
        Mqtt_SendBuf((u8 *) MY_MQTT.txbuf, MY_MQTT.txlen);
        Processing_before_sending();
        wait = 200;                                                                                //等待5s时间
        while (wait--) {
            /*串口2接收完成---进行对比*/
            if (whether) {                                                                                            //订阅
                if ((temp_value = Reception_Comparison(parket_subAck, 4)) != (char) -1)
                    return temp_value;
            } else {                                                                                                            //取消订阅
                if ((temp_value = Reception_Comparison(parket_cancel_subAck, 4)) != (char) -1)
                    return temp_value;
            }
            Delay_ms(100);
        }
    }
    return 0;
}


/*****************************************************
函 数 名：static u8 PublishData(char *topic, char *message, u8 qos)
函数功能：MQTT发布数据打包函数
参    数：topic       主题 
					message 消息
					qos         消息等级 
返 回 值：返回值
******************************************************/
static u8 PublishData(char *topic, char *message, u8 qos) {
    /*主题、消息长度*/
    int topicLength = strlen(topic);
    int messageLength = strlen(message);
    static u16 id = 0;
    /*剩余长度暂时变量*/
    int DataLen;
    MY_MQTT.txlen = 0;
    /*有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度*/
    /*QOS为0时没有标识符*/
    /*数据长度             主题名   报文标识符   有效载荷*/
    if (qos) DataLen = (2 + topicLength) + 2 + messageLength;
    else DataLen = (2 + topicLength) + messageLength;
    /*固定报头----控制报文类型*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = 0x30;
    /*固定报头----剩余长度*/
    do {
        u8 encodedByte = DataLen % 128;
        DataLen = DataLen / 128;
        /*剩余长度--如果要编码的数据更多，请设置此字节的顶部位*/
        if (DataLen > 0)
            encodedByte = encodedByte | 128;
        MY_MQTT.txbuf[MY_MQTT.txlen++] = encodedByte;
    } while (DataLen > 0);
    /*可变报头----主题长度*/
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(topicLength);//主题长度MSB
    MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(topicLength);//主题长度LSB 
    /*可变报头----主题内容*/
    memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], topic, topicLength);//拷贝主题
    MY_MQTT.txlen += topicLength;
    /*可变报头----当QoS等级为1 2时有报文标识*/
    if (qos) {
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE1(id);
        MY_MQTT.txbuf[MY_MQTT.txlen++] = BYTE0(id);
        id++;
    }
    /*有效负载*/
    memcpy(&MY_MQTT.txbuf[MY_MQTT.txlen], message, messageLength);
    MY_MQTT.txlen += messageLength;
    Mqtt_SendBuf((u8 *) MY_MQTT.txbuf, MY_MQTT.txlen);
    return MY_MQTT.txlen;
}

/*****************************************************
函 数 名：static void SentHeart(void)
函数功能：发送心跳包
返 回 值：返回值
******************************************************/
static void SentHeart(void) {
    Mqtt_SendBuf((u8 *) parket_heart, sizeof(parket_heart));
}

/*****************************************************
函 数 名：static void Disconnect(void)
函数功能：断开连接
返 回 值：返回值
******************************************************/
static void Disconnect(void) {
    Mqtt_SendBuf((u8 *) parket_disconnet, sizeof(parket_disconnet));
}
