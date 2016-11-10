#include "stdio.h"
#include "MK64F12.h"
#include "uart.h"
#include "camera.h"

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#define DEFAULT_SYSTEM_CLOCK 20485760u 
// Integration time (seconds)
// Determines how high the camera values are
// Don't exceed 100ms or the caps will saturate
// Must be above 1.25 ms based on camera clk 
//	(camera clk is the mod value set in FTM2)
#define INTEGRATION_TIME .0075f

// Pixel counter for camera logic
// Starts at -2 so that the SI pulse occurs
//		ADC reads start
int pixcnt = -2;
// clkval toggles with each FTM interrupt
int clkval = 0;
// line stores the current array of camera data
uint16_t line[128];

// These variables are for streaming the camera
//	 data over UART
int debugcamdata = 0;
int capcnt = 0;
char str[100];

// ADC0VAL holds the current ADC value
uint16_t ADC1VAL;

static void ADC1_IRQHandler(void);
static void PIT0_IRQHandler(void);
static void FTM0_IRQHandler(void);
static void gpio_init(void);
static void pit_init(void);
static void ftm0_init(void);
static void adc1_init(void);
static void pdb_init(void);


void camera_test(void)
{
    int i;
    put("start test\r\n");
    put("capcnt: ");
    putnumU(capcnt);
    put("\r\n");
    if (capcnt >= (500)) 
    {
        GPIOB_PCOR |= (1 << 22);
        // send the array over uart
        sprintf(str,"%i\n\r",-1); // start value
        put(str);
        for (i = 0; i < 127; i++) {
            putnumU(i);
            put("\r\n");
            sprintf(str,"%i\n", line[i]);
            put(str);
        }
        sprintf(str,"%i\n\r",-2); // end value
        put(str);
        capcnt = 0;
        GPIOB_PSOR |= (1 << 22);
    }
    put("end test\r\n");
}

void camera_read(uint16_t* data)
{
    int i = 0;
    for (i = 0; i < 128; i++)
    {
        data[i] = line[i];
    }
}

void camera_init(void)
{
	gpio_init();
    ftm0_init();
    adc1_init();
    pit_init();
}

// ADC1 Conversion Complete ISR
static void ADC1_IRQHandler(void) 
{
    put("ADC interrupt\r\n");
    ADC1VAL = ADC1_RA;
}


/* PIT0 determines the integration period
*		When it overflows, it triggers the clock logic from
*		FTM2. Note the requirement to set the MOD register
* 	to reset the FTM counter because the FTM counter is 
*		always counting, I am just enabling/disabling FTM2 
*		interrupts to control when the line capture occurs
*/
static void PIT0_IRQHandler(void)
{
    put("PIT interrupt\r\n");
	if (debugcamdata) {
		// Increment capture counter so that we can only 
		//	send line data once every ~2 seconds
		capcnt += 1;
	}
	// Clear interrupt
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
	
	// Setting mod resets the FTM counter
	FTM0_MOD = FTM_MOD_MOD(0x64);
	
	// Enable FTM2 interrupts (camera)
	FTM0_SC |= FTM_SC_TOIE_MASK;
	
	return;
}

static void FTM0_IRQHandler(void)
{
    put("FTM interrupt\r\n");
	//For FTM timer
	// Clear innterrupt
	FTM0_SC &= ~FTM_SC_TOF_MASK;
	
	// Toggle clk
	GPIOB_PTOR |= (1 << 9);
    clkval = !clkval;
	
	// Line capture logic
	if ((pixcnt >= 2) && (pixcnt < 256)) {
		if (!clkval) {	// check for falling edge
			// ADC read (note that integer division is 
			//  occurring here for indexing the array)
			line[pixcnt/2] = ADC1VAL;
		}
		pixcnt += 1;
	} else if (pixcnt < 2) {
		if (pixcnt == -1) {
			GPIOB_PSOR |= (1 << 23); // SI = 1
		} else if (pixcnt == 1) {
			GPIOB_PCOR |= (1 << 23); // SI = 0
			// ADC read
			line[0] = ADC1VAL;
		} 
		pixcnt += 1;
	} else {
		GPIOB_PCOR |= (1 << 9); // CLK = 0
		clkval = 0; // make sure clock variable = 0
		pixcnt = -2; // reset counter
		// Disable FTM2 interrupts (until PIT0 overflows
		//   again and triggers another line capture)
		FTM0_SC &= ~FTM_SC_TOIE_MASK;
	
	}
}
/* Set up pins for GPIO
* 	PTB9 		- camera clk
*		PTB23		- camera SI
*		PTB22		- red LED
*/
static void gpio_init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB_PCR9 = PORT_PCR_MUX(1);
	PORTB_PCR22 = PORT_PCR_MUX(1);
	PORTB_PCR23 = PORT_PCR_MUX(1);
	GPIOB_PDDR |= (1 << 9);
	GPIOB_PDDR |= (1 << 22);
	GPIOB_PDDR |= (1 << 23);
	GPIOB_PDOR |= (1 << 9);
	GPIOB_PDOR |= (1 << 22);
	GPIOB_PDOR |= (1 << 23);
}

/* Initialization of PIT timer to control 
* 		integration period
*/
static void pit_init(void)
{
	// Setup periodic interrupt timer (PIT)
	
	// Enable clock for timers
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable timers to continue in debug mode
	PIT_MCR &= ~PIT_MCR_FRZ_MASK; // In case you need to debug
	
	// PIT clock frequency is the system clock
	// Load the value that the timer will count down from
	PIT_LDVAL0 = (DEFAULT_SYSTEM_CLOCK * INTEGRATION_TIME);
	
	// Enable timer interrupts
	PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
	
	// Enable the timer
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;

	// Clear interrupt flag
	PIT_TFLG0 &= ~PIT_TFLG_TIF_MASK;

	// Enable PIT interrupt in the interrupt controller
	NVIC_EnableIRQ(PIT0_IRQn);
	return;
}

static void ftm0_init(void)
{
	// Enable clock
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;

	// Disable Write Protection
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;
	
	// Set output to '1' on init
	FTM0_OUTINIT |= FTM_OUTINIT_CH0OI_MASK;
	
	// Initialize the CNT to 0 before writing to MOD
	FTM0_CNT = 0;
	
	// Set the Counter Initial Value to 0
	FTM0_CNTIN = 0;
	
	// Set the period (~10us)
	FTM0_MOD = 100;//FTM_MOD_MOD(0x64);
	
	// 50% duty
	FTM0_C0V = 50;
	
	// Set edge-aligned mode
	FTM0_C0SC |= FTM_CnSC_MSB_MASK;
	
	// Enable High-true pulses
	// ELSB = 1, ELSA = 0
	FTM0_C0SC |= FTM_CnSC_ELSB_MASK;
	FTM0_C0SC &= ~FTM_CnSC_ELSA_MASK;
	
	// Enable hardware trigger from FTM2
	FTM0_EXTTRIG |= FTM_EXTTRIG_INITTRIGEN_MASK;
	
	// Don't enable interrupts yet (disable)
	FTM0_SC &= ~FTM_SC_TOIE_MASK;
	
	// No prescalar, system clock
	FTM0_SC = FTM_SC_PS(0);
	FTM0_SC |= FTM_SC_CLKS(1);
	
	// Set up interrupt
	NVIC_EnableIRQ(FTM0_IRQn);
    return;
	
}

static void adc1_init(void)
{
    unsigned int calib;
 
    // Turn on ADC1
    SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
 
    // Configure CFG Registers
    // Configure ADC to divide 50 MHz down to 6.25 MHz AD Clock, 16-bit single ended
    ADC1_CFG1 |= ADC_CFG1_ADIV_MASK;
		ADC1_SC1A &= ~ADC_SC1_DIFF_MASK;
		ADC1_CFG1 |= ADC_CFG1_MODE_MASK;
 
    // Do ADC Calibration for Singled Ended ADC. Do not touch.
    ADC1_SC3 = ADC_SC3_CAL_MASK;
    while ( (ADC1_SC3 & ADC_SC3_CAL_MASK) != 0 );
    calib = ADC1_CLP0;
    calib += ADC1_CLP1;
    calib += ADC1_CLP2;
    calib += ADC1_CLP3;
    calib += ADC1_CLP4;
    calib += ADC1_CLPS;
    calib = calib >> 1;
    calib |= 0x8000;
    ADC1_PG = calib;
 
    // Configure SC registers.
    // Select hardware trigger.
    ADC1_SC2 |= ADC_SC2_ADTRG_MASK;
    
    ADC1_SC1A &= ~ADC_SC1_DIFF_MASK;
 
    // Configure SC1A register.
    // Select ADC Channel and enable interrupts. Use ADC1 channel DADP3  in single ended mode.
    ADC1_SC1A |= ADC_SC1_ADCH(0x3);
    ADC1_SC1A |= ADC_SC1_AIEN_MASK;
    
    SIM_SOPT7 |= SIM_SOPT7_ADC1TRGSEL(0x8);
    SIM_SOPT7 |= SIM_SOPT7_ADC1ALTTRGEN_MASK;
    SIM_SOPT7 &= ~(SIM_SOPT7_ADC1PRETRGSEL_MASK);
    
    // Enable NVIC interrupt
    NVIC_EnableIRQ(ADC1_IRQn);
}

static void pdb_init(void)
{
    //Enable PDB Clock
    SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
    //PDB0_CNT = PDB_CNT_CNT(0x0);
    PDB0_MOD = 50000; // 50,000,000 / 50,000 = 1000

    PDB0_SC = PDB_SC_PDBEN_MASK | PDB_SC_CONT_MASK | PDB_SC_TRGSEL(0xf)
                                    | PDB_SC_LDOK_MASK;
    PDB0_CH1C1 = PDB_C1_EN(0x01) | PDB_C1_TOS(0x01);
}