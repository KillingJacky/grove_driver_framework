#Grove Driver Framework

....

## Rules for writing compatible grove drivers

* All hardware calls must be made through suli2 api
* 规范的格式
  * 完整的header注释
  * 较完整的function注释
  * 函数名及变量名均采用小写
  * 分词采用下划线
* 为自动处理脚本而约定的特殊格式
  * 文件命名格式: grove打头,单词小写,下划线分词, 例如 grove_3axis_acc.h / grove_light_sensor.h / grove_nfc.h
  * 必须在class.h中填加//GROVE_NAME注释行
  * 必须在class.h中填加//IF_TYPE注释行, 接口类型可以是{GPIO, PWM, ANALOG, I2C, UART}
  * 第一层驱动实现中所有函数必须以grove_grovename_ 打头
  * 第一层驱动实现中所有对外开放的读函数必须以grove_grovename_read_ 打头
  * class封装中所有对外开放的读函数必须以read_打头
  * 第一层驱动实现中所有对外开放的写/配置函数必须以grove_grovename_write_ 打头
  * class封装中所有对外开放的写/配置函数必须以write_ 打头
  * class封装中构造函数的参数只能是硬件相关的,例如pin脚, 除硬件外所有配置均通过write写入
  * 第一层及第二层封装中所有非对外开放的函数/内部函数均以下划线打头
  * 所有read/write函数返回类型为bool
  * 所有read函数数值通过指针向外透出, 参数个数不限
  * 所有write函数参数个数不能超过4个
  * class封装中成员方法的参数数值类型支持{int, float, char, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t}及他们的指针型
