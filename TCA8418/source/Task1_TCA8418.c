#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "time.h"
//#include "Keypad_I2C.h"

//#include "derivative.h"


/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

    uint8_t  data1;
	uint8_t  data2;
	uint8_t  data3;
	uint8_t  data;

		uint8_t REG_INT_STAT_data; //0X02

		uint8_t REG_KEY_LCK_EC_data;

		uint8_t REG_KEY_EVENT_A_data;
		uint8_t REG_KEY_EVENT_B_data;
		uint8_t REG_KEY_EVENT_C_data;
		uint8_t REG_KEY_EVENT_D_data;
		uint8_t REG_KEY_EVENT_E_data;
		uint8_t REG_KEY_EVENT_F_data;
		uint8_t REG_KEY_EVENT_G_data;
		uint8_t REG_KEY_EVENT_H_data;
		uint8_t REG_KEY_EVENT_I_data;
		uint8_t REG_KEY_EVENT_J_data;

		uint8_t GPIO_INT_STAT1_data; //0X11
		uint8_t GPIO_INT_STAT2_data;
		uint8_t GPIO_INT_STAT3_data; // 0X13

		uint8_t GPIO_DAT_STAT1_data;
		uint8_t GPIO_DAT_STAT2_data;
		uint8_t GPIO_DAT_STAT3_data;

		uint8_t GPIO_DAT_OUT1_data;
		uint8_t GPIO_DAT_OUT2_data;
		uint8_t GPIO_DAT_OUT3_data;

		uint8_t GPIO_INT_EN1_data; // 0X1A
		uint8_t GPIO_INT_EN2_data;
		uint8_t GPIO_INT_EN3_data;//0X1C

		uint8_t REG_GPI_EM1_data;
		uint8_t REG_GPI_EM2_data;
		uint8_t REG_GPI_EM3_data;

		uint8_t REG_GPIO_DIR1_data; //0X23
		uint8_t REG_GPIO_DIR2_data;
		uint8_t REG_GPIO_DIR3_data; //0X25

		uint8_t REG_GPIO_INT_LVL1_data;
		uint8_t REG_GPIO_INT_LVL2_data;
		uint8_t REG_GPIO_INT_LVL3_data;

		uint8_t REG_DEBOUNCE_DIS1_data;
		uint8_t REG_DEBOUNCE_DIS2_data;
		uint8_t REG_DEBOUNCE_DIS3_data;

		uint8_t loop_Value;
		uint16_t GPIO_data;
		uint16_t newConfig;
		uint16_t keyData;



typedef struct {
	uint32_t PCR[32];
}PORTRegs_t;

#define PORT_A ((PORTRegs_t *)0x40049000)
#define PORT_B ((PORTRegs_t *) 0x4004A000)

typedef struct{

	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
}GPIORegs_t;

#define GPIO_A ((GPIORegs_t *)0x400FF000)
#define GPIO_B ((GPIORegs_t *) 0x400FF040)

#define Slave_Addr 0x34


/*tca8418 configure Registers */


#define REG_CFG			0x01
#define REG_INT_STAT		0x02
#define REG_KEY_LCK_EC		0x03
#define REG_KEY_EVENT_A		0x04
#define REG_KEY_EVENT_B		0x05
#define REG_KEY_EVENT_C		0x06
#define REG_KEY_EVENT_D		0x07
#define REG_KEY_EVENT_E		0x08
#define REG_KEY_EVENT_F		0x09
#define REG_KEY_EVENT_G		0x0A
#define REG_KEY_EVENT_H		0x0B
#define REG_KEY_EVENT_I		0x0C
#define REG_KEY_EVENT_J		0x0D
#define REG_KP_LCK_TIMER	0x0E
#define REG_UNLOCK1		0x0F
#define REG_UNLOCK2		0x10
#define REG_GPIO_INT_STAT1	0x11
#define REG_GPIO_INT_STAT2	0x12
#define REG_GPIO_INT_STAT3	0x13
#define REG_GPIO_DAT_STAT1	0x14
#define REG_GPIO_DAT_STAT2	0x15
#define REG_GPIO_DAT_STAT3	0x16
#define REG_GPIO_DAT_OUT1	0x17
#define REG_GPIO_DAT_OUT2	0x18
#define REG_GPIO_DAT_OUT3	0x19
#define REG_GPIO_INT_EN1	0x1A
#define REG_GPIO_INT_EN2	0x1B
#define REG_GPIO_INT_EN3	0x1C
#define REG_KP_GPIO1		0x1D
#define REG_KP_GPIO2		0x1E
#define REG_KP_GPIO3		0x1F
#define REG_GPI_EM1		0x20
#define REG_GPI_EM2		0x21
#define REG_GPI_EM3		0x22
#define REG_GPIO_DIR1		0x23
#define REG_GPIO_DIR2		0x24
#define REG_GPIO_DIR3		0x25
#define REG_GPIO_INT_LVL1	0x26
#define REG_GPIO_INT_LVL2	0x27
#define REG_GPIO_INT_LVL3	0x28
#define REG_DEBOUNCE_DIS1	0x29
#define REG_DEBOUNCE_DIS2	0x2A
#define REG_DEBOUNCE_DIS3	0x2B
#define REG_GPIO_PULL1		0x2C
#define REG_GPIO_PULL2		0x2D
#define REG_GPIO_PULL3		0x2E



#define I2C1_START		I2C1->C1 |= (1 << I2C_C1_MST_SHIFT) | (1 << I2C_C1_TX_SHIFT)

#define I2C1_STOP		I2C1->C1 &= ~((1 << I2C_C1_MST_SHIFT) | (1 << I2C_C1_TX_SHIFT) | (1 << I2C_C1_TXAK_SHIFT))

#define I2C1_RSTART		I2C1->C1 |= (1 << I2C_C1_RSTA_SHIFT) | (1 << I2C_C1_TX_SHIFT)

#define I2C1_XMIT		I2C1->C1 |= (1 << I2C_C1_TX_SHIFT)

#define I2C1_RECV		I2C1->C1 &= ~(1 << I2C_C1_TX_SHIFT)

#define I2C1_MASTER		I2C1->C1 |= (1 << I2C_C1_MST_SHIFT) \

#define I2C1_WAIT		while((I2C1->S & (1 << I2C_S_IICIF_SHIFT)) == 0); \
						I2C1->S |= (1 << I2C_S_IICIF_SHIFT)

#define I2C1_READ_WAIT			while((I2C1->S & (1 << I2C_S_IICIF_SHIFT)) == 0); \
								I2C1->S |= (1 << I2C_S_IICIF_SHIFT)

void i2c_write(uint8_t slaveAddr, uint8_t regAddr, uint8_t regVal)
{
	uint8_t data;

	/* I2C1 Check for Bus Busy */
	while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* Generate START Condition */
	I2C1_START;

	//I2C1->A1 = (slaveAddr << 1);

	/* Send Slave Address */
	I2C1->D = 0x68; ////(slaveAddr<< 1);
	I2C1_WAIT;

	/* Send Register Address */
	I2C1->D = regAddr;
	I2C1_WAIT;

	/* Send value to Slave  */
	I2C1->D = regVal;
	I2C1_WAIT;

	/* Generate STOP Condition */
	I2C1_STOP;

	return data;
}

uint8_t i2c_read(uint8_t slaveAddr, uint8_t regAddr)
{
	uint8_t data;
	uint8_t timeDelay = 6;

	/* I2C1 Check for Bus Busy */
	while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* Generate START Condition */
	I2C1_START;

	/* Send Slave Address */
	I2C1->D = (slaveAddr << 1);
	I2C1_WAIT;

	/* Send Register Address */
	I2C1->D = regAddr;
	I2C1_WAIT;

	/* Generate Repeated Start */
	I2C1_RSTART;

    /* Add some delay to wait the Re-Start signal. */
    while (timeDelay--)
    {
        __NOP();
    }

	/* Send Slave Address */
	I2C1->D = (slaveAddr << 1) | 0x01;
	I2C1_WAIT;

	I2C1_RECV;

	I2C1->C1 |= (1 << I2C_C1_TXAK_SHIFT);

	/* Dummy Read */
	data = I2C1->D;
	I2C1_READ_WAIT;

	/* Generate STOP Condition */
	I2C1_STOP;

	/* Read the value from Data Register */
	data = I2C1->D;
//    printf("\n READ DATA = %d \n",data);
	return data;
}

#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE

#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 0U

#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 1U



static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < 1000; i++)
    {
        __NOP();
    }
}

void BOARD_I2C_ReleaseBus(void)
{
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    uint16_t newValue;

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic = 1U;
    CLOCK_EnableClock(kCLOCK_PortE);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SDA_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();



    /* Send 9 pulses on SCL and keep SDA low */
    for (i = 0; i < 9; i++)
    {
        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

/*
 * @brief   Application entry point.
 */



typedef struct{
	uint32_t iser[1];
	uint32_t rsvd[31];
	uint32_t icer[1];
	uint32_t rsvd1[31];
	uint32_t ispr[1];
	uint32_t rsvd2[31];
	uint32_t icpr[1];
	uint32_t rsvd3[31];
	uint32_t rsvd4[64];
	uint32_t ipr[8];
}NVIC_Regs_t;

#define NVIC_REG ((NVIC_Regs_t *)0xE000E100)

void PORTA_IRQHandler(void){

//if(GPIO_A->PDIR & (1<<5)){
//			/* Make the LED as OFF */
//			GPIO_B->PSOR = (1 << 18);
//	}else{
//			/* Make the LED as ON */
//			GPIO_B->PCOR = (1 << 18);
//	}

GPIO_B->PTOR = (1 << 18);


                REG_INT_STAT_data = i2c_read(Slave_Addr, REG_INT_STAT);
//		 	    printf("REG_INT_STAT Data  : %d \n",REG_INT_STAT_data );

 		 	    REG_KEY_LCK_EC_data = i2c_read(Slave_Addr, REG_KEY_LCK_EC);
// 		 	    printf("REG_KEY_LCK_EC Data  : %2x \n",REG_KEY_LCK_EC_data );

 		 	    REG_INT_STAT_data = i2c_read(Slave_Addr, REG_INT_STAT);
// 		 	    printf("REG_INT_STAT New Data  : %d \n",REG_INT_STAT_data );

 		 	   do {

 		 	         REG_KEY_EVENT_A_data = i2c_read(Slave_Addr, REG_KEY_EVENT_A);
		    	     REG_KEY_LCK_EC_data = i2c_read(Slave_Addr, REG_KEY_LCK_EC);

 		 	      }while( REG_KEY_LCK_EC_data > 1 );

            switch_stat(REG_KEY_EVENT_A_data);

		    i2c_write(Slave_Addr, 0x02, 0x01);



	/* W1C on Interrupt Status Flag of PORTA 5 */
	PORT_A->PCR[5] = PORT_A->PCR[5] | (1 << 24); // reset ke pin

}

void switch_stat(value){

	switch(value)
				{

				case 129:
					printf(" Value : D\n");
					break;


				case 130:
					printf(" Value : C\n");
					break;


				case 131:
					printf(" Value : B\n");
					break;


				case 132:
					printf(" Value : A\n");
					break;


				case 139:
					printf(" Value : #\n");
					break;


				case 140:
					printf(" Value : 9\n");
					break;


				case 141:
					printf(" Value : 6\n");
					break;


				case 142:
					printf(" Value : 3\n");
					break;



				case 149:
					printf(" Value : 0\n");
					break;


				case 150:
					printf(" Value : 8\n");
					break;


				case 151:
					printf(" Value : 5\n");
					break;


				case 152:
					printf(" Value : 2\n");
					break;


				case 159:
					printf(" Value : *\n");
					break;


				case 160:
					printf(" Value : 7\n");
					break;


				case 161:
					printf(" Value : 4\n");
					break;


				case 162:
					printf(" Value : 1\n");
					break;


			}



}

int main(void) {



 	SIM->SCGC5 = (1 << 9) | (1 << 10) | (1 << SIM_SCGC5_PORTE_SHIFT);   //clock enable for Port A and Port B and port E

	/* Enable clock for I2C1 */
	SIM->SCGC4 = (1 << SIM_SCGC4_I2C1_SHIFT);

	PORT_A->PCR[5] = (1 << 8) | (1 << 1) | (1 << 0) | (0xb << 16); //intr

	PORT_B->PCR[18]= (1 << 8); //op led

	BOARD_I2C_ReleaseBus();

		/* PORTE 1 pin as I2C1_SCL */
		PORTE->PCR[1] =  (6 << PORT_PCR_MUX_SHIFT) | (1 << PORT_PCR_PS_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1 <<PORT_PCR_SRE_SHIFT);

		/* PORTE 0 pin as I2C1_SDA */
		PORTE->PCR[0] =  (6 << PORT_PCR_MUX_SHIFT) | (1 << PORT_PCR_PS_SHIFT) | (1<<PORT_PCR_PE_SHIFT) | (1 <<PORT_PCR_SRE_SHIFT);

	    /* I2C1 Frequency Divider */
		I2C1->F = 0x0F;

	/* I2C1 Enable, Master Mode */
	I2C1->C1 = (1 << I2C_C1_IICEN_SHIFT) | (1 << I2C_C1_IICIE_SHIFT);

	I2C1->S |= (1 << I2C_S_IICIF_SHIFT);

	/* I2C1 Check for Bus Busy */
	while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* keypad code*/

	   i2c_write(Slave_Addr, REG_KP_GPIO1, 0x0F);  //r0 - r3 to kp matrix

	   i2c_write(Slave_Addr, REG_KP_GPIO2, 0x0F);  // c0 -c3 to kp matrix

	   i2c_write(Slave_Addr, REG_KP_GPIO3, 0x00); // c8 - c9 to gpio

	   i2c_write(Slave_Addr, REG_CFG, 0x95);

	/* Enable Interrupt for PORTA */

	GPIO_A->PDDR= (0 << 5);

	GPIO_B->PDDR= (1 << 18);

	GPIO_B->PSOR = (1 << 18);

	NVIC_REG->iser[0] = (1 << 30);

	while(1) {

	}

    return 0 ;
}
