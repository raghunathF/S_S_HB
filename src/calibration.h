/*
 * calibration.h
 *
 * Created: 10/10/2017 9:54:05 AM
 *  Author: raghu
 */ 


#ifndef CALIBRATION_H_
#define CALIBRATION_H_

void calibration_init();
int  convert2degree2(volatile int x , volatile int y);
extern int intersect_min_value ;
extern int intersect_max_value ;
extern int position_global;
extern bool test_main;

#define Y_OFFSET 4

#endif /* CALIBRATION_H_ */