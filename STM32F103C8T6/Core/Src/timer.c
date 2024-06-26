/*
 * timer.c
 *
 *  Created on: Jun 26, 2024
 *      Author: HP
 */

#include "timer.h"

#define TIMERNUM 10
int timer_counter[TIMERNUM]={0,0,0,0,0,0,0,0,0,0};
int timer_flag[TIMERNUM]={0,0,0,0,0,0,0,0,0,0};

void setTimer(int INDEX,int duration){
	timer_counter[INDEX]=duration;
	timer_flag[INDEX]=0;
}

void timerRun(){
	for(int i=0;i<TIMERNUM;i++){
		if(timer_counter[i] > 0){
			timer_counter[i]--;
			if(timer_counter[i] <=0){
				timer_flag[i]=1;
			}
		}
	}
}
