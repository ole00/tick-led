#ifndef _CONFIG_H_
#define _CONFIG_H_

void printConfig(void);
uint8_t printConfigParam(char* param);
uint8_t setConfigParam(const char* param);

uint8_t getAlarmLoopIndex(uint16_t i);

#endif /* _CONFIG_H_ */

