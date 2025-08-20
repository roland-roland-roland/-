#ifndef __AD_H__
#define __AD_H__

void AD_Init(void);
uint16_t AD_GetValue(void);
void AD_GetJoystick(uint16_t* Joystick_Array);

#endif