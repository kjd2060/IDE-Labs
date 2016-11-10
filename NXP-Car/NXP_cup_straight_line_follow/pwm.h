#ifndef PWM_H_
#define PWM_H_

void pwm_set_duty_cycle(unsigned int DutyCycle, unsigned int Frequency, int dir, int motorSelect);
void pwm_init(void);

#endif /* PWM_H_ */
