#ifndef __CalculateAngles_H__
#define __CalculateAngles_H__

extern int16_t pitch, roll;  // 俯仰角和横滚角（整数，单位：度）
extern uint8_t ErrorStutes;

void CalculateAngles(void) ;

#endif