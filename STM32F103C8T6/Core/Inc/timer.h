/*
 * timer.h
 *
 *  Created on: Jun 26, 2024
 *      Author: HP
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_


extern int timer_flag[];

void setTimer(int timer_index,int duration);
void timerRun();

#endif /* INC_TIMER_H_ */
