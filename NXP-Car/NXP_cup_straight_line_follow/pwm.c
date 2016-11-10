
#include "MK64F12.h"
#include "pwm.h"
#include "uart.h"

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */
#define CLOCK                    20485760u
#define PWM_FREQUENCY            10000
#define FTM0_MOD_VALUE            (CLOCK/PWM_FREQUENCY)
#define FTM2_PRESCALER_VALUE (0x5)
#define FTM2_CLOCK ((CLOCK)/((FTM2_PRESCALER_VALUE-1)*8))

static volatile unsigned int PWMTick = 0;

/*
 * Change the Motor Duty Cycle and Frequency
 * @param DutyCycle (0 to 100)
 * @param Frequency (~1000 Hz to 20000 Hz)
 * @param dir: 1 for C4 active, else C3 active 
 */
void pwm_set_duty_cycle(unsigned int DutyCycle, unsigned int Frequency, int dir, int motorSelect)
{
    // motorSelect 0 == DC Motor
    // motorSelect 1 == Servo motor
    // Calculate the new cutoff value
    uint16_t mod;
  
    // Set outputs
    if(motorSelect == 0)
    {
        mod = (uint16_t) (((CLOCK/Frequency) * DutyCycle) / 100);
        if(dir==1)
            {FTM3_C2V = mod; FTM3_C3V=0;}
        else
            {FTM3_C3V = mod; FTM3_C2V=0;}
        FTM0_MOD = (CLOCK/Frequency);
    }
    else if(motorSelect == 1)
    {
        mod = (uint16_t) (((FTM2_CLOCK/Frequency) * DutyCycle) / 100);
        FTM2_C0V = mod;
        FTM2_MOD = (FTM2_CLOCK/Frequency);
    }
    else if(motorSelect == 2)
    {
        mod = (uint16_t) (((CLOCK/Frequency) * DutyCycle) / 100);
        if(dir==1)
            {FTM3_C0V = mod; FTM3_C1V=0;}
        else
            {FTM3_C1V = mod; FTM3_C0V=0;}
        FTM3_MOD = (CLOCK/Frequency);
    }
    // Update the clock to the new frequency
}


/*
 * Initialize the FlexTimer for PWM
 */
void pwm_init()
{
    // 12.2.13 Enable the Clock to the FTM0 Module
    //SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
    SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;
    SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;
    
    // Enable clock on PORT A so it can output
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
    
    // 11.4.1 Route the output of FTM channel 0 to the pins
    // Use drive strength enable flag to high drive strength
    //These port/pins may need to be updated for the K64 <Yes, they do. Here are two that work.>
    
    // PTA0, C5 : PTA1, C6 : Both need mux3
    PORTB_PCR18     = PORT_PCR_MUX(3)  | PORT_PCR_DSE_MASK; // FTM2 C0
    PORTD_PCR0      = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK; //FTM3 Ch0
    PORTD_PCR1      = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK;//FTM3 Ch1
    PORTD_PCR2      = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK; //FTM3 Ch2
    PORTD_PCR3      = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK;//FTM3 Ch3
    
    // 39.3.10 Disable Write Protection
    //FTM0_MODE |= FTM_MODE_WPDIS_MASK;
    FTM2_MODE |= FTM_MODE_WPDIS_MASK;
    FTM3_MODE |= FTM_MODE_WPDIS_MASK;
    
    // 39.3.4 FTM Counter Value
    // Initialize the CNT to 0 before writing to MOD
    //FTM0_CNT = 0;
    FTM2_CNT = 0;
    FTM3_CNT = 0;
    
    // 39.3.8 Set the Counter Initial Value to 0
    //FTM0_CNTIN = 0;
    FTM2_CNTIN = 0;
    FTM3_CNTIN = 0;
    
    // 39.3.5 Set the Modulo resister
    //FTM0_MOD = FTM0_MOD_VALUE;
    FTM2_MOD = FTM0_MOD_VALUE;
    FTM3_MOD = FTM0_MOD_VALUE;
    //FTM0->MOD = (DEFAULT_SYSTEM_CLOCK/(1<<7))/1000;

    // 39.3.6 Set the Status and Control of both channels
    // Used to configure mode, edge and level selection
    // See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
    //FTM0_C3SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    //FTM0_C3SC &= ~FTM_CnSC_ELSA_MASK;
    
    
    // See Table 39-67,  Edge-aligned PWM, Low-true pulses (clear out on match)
    //FTM0_C2SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    //FTM0_C2SC &= ~FTM_CnSC_ELSA_MASK;
    
    FTM2_C0SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM2_C0SC &= ~FTM_CnSC_ELSA_MASK;
    
    //FTM2_C1SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    //FTM2_C1SC &= ~FTM_CnSC_ELSA_MASK;
    
    FTM3_C0SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3_C0SC &= ~FTM_CnSC_ELSA_MASK;
    
    FTM3_C1SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3_C1SC &= ~FTM_CnSC_ELSA_MASK;
    
    FTM3_C2SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3_C2SC &= ~FTM_CnSC_ELSA_MASK;
    
    FTM3_C3SC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3_C3SC &= ~FTM_CnSC_ELSA_MASK;
    
    // 39.3.3 FTM Setup
    // Set prescale value to 1 
    // Chose system clock source
    // Timer Overflow Interrupt Enable
    //FTM0_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1); 
    //| FTM_SC_TOIE_MASK;
    FTM2_SC = FTM_SC_PS(FTM2_PRESCALER_VALUE) | FTM_SC_CLKS(1); 
    //FTM2_SC |= FTM_SC_TOIE_MASK;
    FTM3_SC = FTM_SC_PS(0) | FTM_SC_CLKS(1); 

}
