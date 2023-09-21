使用CLion和Keil同时开发STM32F103ZET6标准库，标准库版本：`V3.6.0 / 20-September-2021`。

CLion和Keil中已重定向串口1到`printf`。

Keil重定向参考文档：blog.csdn.net/qq_34810707/article/details/112255306


## 项目进度：

> 注意，本项目仅使用CLion开发，Keil请自行添加文件进行编译

- 成功加载2023.9.14代码 `温湿度上报.rar`
- 实现ESP8266连接WiFi功能
- 实现I2C读取光照值
- DHT11
- MQ3
- MQ35
- Rain
- Soil
- MQTT上传成功