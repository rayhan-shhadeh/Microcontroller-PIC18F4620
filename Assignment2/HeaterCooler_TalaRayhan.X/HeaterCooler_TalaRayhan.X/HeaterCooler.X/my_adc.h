#ifndef MY_PWM_H
#define	MY_PWM_H
#include <xc.h> // include processor files - each processor file is guarded.  

void init_adc_no_lib(void);
int read_adc_raw_no_lib(unsigned char channel);
float read_adc_voltage(unsigned char channel);


#endif	/* MY_PWM_H */

