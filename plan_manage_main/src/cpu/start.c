
#include "common.h"
#include "sysinit.h"

extern void main(void);




//看门狗WDOG(COP)宏定义
#define WDOG_DISABLE() {SIM_COPC = 0x00u;}  //禁用看门狗
//COP使用LPO时钟，COP在2^10个LPO时钟周期后延时
#define WDOG_ENABLE() {SIM_COPC |= SIM_COPC_COPT_MASK;} //使能看门狗


// 系统上电启动程序
 void start(void)
{
	 WDOG_DISABLE();       //禁用看门狗
        //WDOG_ENABLE();        使能看门狗
        common_startup();
        sys_init() ;            //cpu主频设置
	main();                 //进入主函数
	while(1);               //永久循环（主函数内永久循环，含有执行不到该语句）
}

//-------------------------------------------------------------------------------



