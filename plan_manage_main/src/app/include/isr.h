/*
 * isr.h - 重新宏定义中断号，重映射中断向量表里的中断函数地址，
 * 使其指向所定义的中断服务函数。
 *
 * 只能在"vectors.c"包含，而且必须在"vectors.h"包含的后面！！！
 */

#ifndef ISR_H
#define ISR_H 



#ifdef VECTOR_028
#undef VECTOR_028
#define VECTOR_028	UART0_isr
#endif

#ifdef VECTOR_038
#undef VECTOR_038
#define VECTOR_038	PIT_isr
#endif

#ifdef VECTOR_046
#undef VECTOR_046
#define VECTOR_046	PTA_isr
#endif


#ifdef VECTOR_047
#undef VECTOR_047
#define VECTOR_047	PTD_isr
#endif



void PIT_isr(void);


void PTA_isr(void);


void PTD_isr(void);


void UART0_isr(void);


#endif /* ISR_H */
