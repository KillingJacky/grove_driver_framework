#Grove Driver Framework


## Supported platforms
* mbed - developing target platform, well tested
* Arduino - suli supported, need migration maybe
* ESP8266 - ON THE PLAN BY MIGRATING SULI

## Devloping Status

Done:

* 透过串口rpc调用任一grove驱动中的公共读写方法 
* 任一驱动中任一时刻有紧急事件即刻上报 
* 加入Arduino Style的用户编程空间, 使高级工程师用户可以基于此做进一步开发
* 由脚本扫描所有grove驱动到database文件中(json格式), 方便WEB端读取, 显示Grove的属性
* 由脚本根据grove的连接配置文件(yaml)自动生成rpc封装代码, 实现极轻量级rpc封装
* Makefile只要传入Grove列表即可按需编译
* 一个极高效的转发server - server.py, 基于Tornado框架编写, 协程式异步IO操作, 实现Web端HTTP请求与Device端协议间的代理转发

Doing / Plan to do:
* Device端透过WiFi的RPC调用 - ESP8266作为网络传输收发器
* server端根据配置yaml编译固件, 并透过wifi OTA到device
* Device端的OTA支持
* server端通过websocket报告device上报的事件
* Server端UI界面实现连接配置
* Server端的用户密码管理和Device管理
* Server端OAuth的认证机制, 以密码为基础生成AccessToken
* Server端与Device端的加密通信, 借鉴shadowsocks的加密方式,即以用户密码为基础生成对称加密密钥




## Rules for writing compatible grove drivers

* All hardware calls must be made through suli2 api
* 规范的格式
  * 完整的header注释
  * 较完整的function注释
  * 函数名及变量名均采用小写
  * 分词采用下划线
* 为自动处理脚本而约定的特殊格式
  * 文件命名格式: grove打头,单词小写,下划线分词, 例如 grove_3axis_acc.h / grove_light_sensor.h / grove_nfc.h
  * class封装层文件名中必须包含class字样, 建议例如 grove_3axis_acc_class.h
  * 必须在class.h中填加//GROVE_NAME注释行
  * 必须在class.h中填加//IF_TYPE注释行, 接口类型可以是{GPIO, PWM, ANALOG, I2C, UART}
  * 第一层驱动实现中所有函数必须以grove_grovename_ 打头
  * 第一层驱动实现中所有对外开放的读函数必须以grove_grovename_read_ 打头
  * class封装中所有对外开放的读函数必须以read_打头
  * 第一层驱动实现中所有对外开放的写/配置函数必须以grove_grovename_write_ 打头
  * class封装中所有对外开放的写/配置函数必须以write_ 打头
  * class封装中构造函数的参数只能是硬件相关的,例如pin脚, 除硬件外所有配置均通过write写入
  * class封装中构造函数的参数按接口类型不同,分别有固定参数,参下一节"构造函数参数"
  * 第一层及第二层封装中所有非对外开放的函数/内部函数均以下划线打头
  * 所有read/write函数返回类型为bool
  * 所有read函数数值通过指针向外透出, 参数个数不限
  * 所有write函数参数个数不能超过4个
  * class封装中成员方法的参数数值类型支持{int, float, char, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t}及他们的指针型
  * class封装层中事件绑定函数的命名必须为attach_event_handler(才能被脚本扫到)
  * 需要上报事件时, EVENT_T全局事件变量的定义放到第一层驱动封装中, 因为事件触发是由底层到上层
  
  
* 构造函数参数
  * GPIO类型: 参数为(int pin)
  * PWM类型: 参数为(int pin)
  * ANALOG类型: 参数为(int pin)
  * I2C类型: 参数为(int pinsda, int pinscl)
  * UART类型: 参数为(int pintx, int pinrx)
  
## How to test newly written grove driver?

###1. Need to scan all the drivers into a database file. 

Steps:

  * python envirenment is needed
  * cd project root dir (where the script scan_drivers.py lies)
  * run "python ./scan_drivers.py" in your shell / cmd line
  
###2. Test building
  * python envirenment needed
  * gnu make tools needed
  * gnu arm gcc cross-compile tools needed, and modify the path in Makefile if they're not in your PATH env
  * edit the yaml file in users_build_dir/local_user, follow the rule 构造函数参数
  * run "python ./build_firmware.py" in your shell / cmd line - Check if error happens, 如果出错,那一定是没有遵循规则
  * cd users_build_dir/local_user (暂时, build_firmware.py完成后无须此步)
  * run "make" (暂时, build_firmware.py完成后无须此步) - Check if error happens, 如果出错, 可能是任意地方有错误
  
