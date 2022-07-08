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


/*
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
*/

#define Slave_Addr 0x40

#define RH_Hold_Master_Mode 0xE5
#define RH_No_Hold_Master_Mode 0xF5
#define Temp_Hold_Master_Mode 0xE3
#define Temp_No_Hold_Master_Mode 0xF3


#define RH_T_User_Reg_1_WR   0xE6
#define RH_T_User_Reg_1_RD   0xE7
#define Heater_CR_WR   0x51
#define Heater_CR_RD   0x11
#define Electronic_ID_1st_Byte_WR   0xFA 0x0F
#define Electronic_ID_1st_Byte_RD   0xFC 0xC9





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

void i2c_write(uint8_t slaveAddr, uint8_t regAddr)  //, uint8_t regVal)
{
	uint8_t data;

	/* I2C1 Check for Bus Busy */
	while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* Generate START Condition */
	I2C1_START;

	//I2C1->A1 = (slaveAddr << 1);

	/* Send Slave Address */
	I2C1->D = (slaveAddr << 1);
	I2C1_WAIT;

	/* Send Register Address */
	I2C1->D = regAddr;
	I2C1_WAIT;

//	/* Send value to Slave  */
//	I2C1->D = regVal;
//	I2C1_WAIT;

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

//	/* Send Register Address */
//	I2C1->D = regAddr;
//	I2C1_WAIT;

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

//	/* Generate Repeated Start */
//		I2C1_RSTART;
//
//	    /* Add some delay to wait the Re-Start signal. */
//	    while (timeDelay--)
//	    {
//	        __NOP();
//	    }
//
//		/* Send Slave Address */
//		I2C1->D = (slaveAddr << 1) | 0x01;
//		I2C1_WAIT;
//
	I2C1_RECV;

	I2C1->C1 |= (1 << I2C_C1_TXAK_SHIFT);

	/* Dummy Read */
	data = I2C1->D;
	I2C1_READ_WAIT;

	/* Generate STOP Condition */
	I2C1_STOP;

	/* Read the value from Data Register */
	data = I2C1->D;

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

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

	uint8_t data[2] = {0};
	uint8_t data1;
	uint8_t data2;
	uint8_t data3;
	uint8_t data4;
//	uint8_t data5;
//	uint8_t data6;
//	uint8_t data7;

	SIM->SCGC5 =  (1 << SIM_SCGC5_PORTE_SHIFT);   //clock enable for port E

	/* Enable clock for I2C1 */
	SIM->SCGC4 = (1 << SIM_SCGC4_I2C1_SHIFT);

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

   //  i2c_write(Slave_Addr, 0xF3);  //, 1);


//     for(int i = 0;i<100000;i++){
//
//     }
   //  i2c_write(Slave_Addr, Temp_No_Hold_Master_Mode,1);
//     data1 = i2c_read(Slave_Addr);  //, RH_No_Hold_Master_Mode);
//     data2 = i2c_read(Slave_Addr);   //, RH_No_Hold_Master_Mode);
//     printf("data 1 is : %d \n",data1);
//     printf("data 2 is : %d \n",data2);
//
//		float cTemp = ((((data1 * 256 )+ data2) * 125) / 65536) - 6;
////		float fTemp = cTemp * 1.8 + 32;
//
//		printf("%02d\n",cTemp);
////		printf("%02d\n",fTemp);

	while(1){

	     i2c_write(Slave_Addr, 0xF5);  //, 1);


	     for(int i = 0;i<100000;i++){

	     }
			 data1 = i2c_read(Slave_Addr,0xF5);  //, RH_No_Hold_Master_Mode);
		     data2 = i2c_read(Slave_Addr,0xF5);   //, RH_No_Hold_Master_Mode);
		     printf("data 1 is : %d \n",data1);
		     printf("data 2 is : %d \n",data2);

//				int cTemp = ( ( ( (data1 * 256 ) + data2)  * 125) / 65536 ) - 6;
//
//				 printf("%02d \n",cTemp);

				int humidity = ((((data1 * 256 )+ data2) * 125) / 65536) - 6;

				printf("Relative Humidity : %02d % \n", humidity);

				 for(int i = 0;i<1000000;i++){

				 }


	 i2c_write(Slave_Addr, 0xF3);  //, 1);


		     for(int i = 0;i<100000;i++){

		     }
				 data3 = i2c_read(Slave_Addr,0xF3);  //, RH_No_Hold_Master_Mode);
			     data4 = i2c_read(Slave_Addr,0xF3);   //, RH_No_Hold_Master_Mode);
			     printf("data 3 is : %d \n",data3);
			     printf("data 4 is : %d \n",data4);

//			     int cTemp = ((((data3 * 256) + data4) * 175.72) / 65536) - 46.85;
//			     int fTemp = ( cTemp * 1.8 )+ 32;
//
//			     // Output data to screen
//
//				printf("Temperature in Celsius : %02d C \n", cTemp);
//				printf("Temperature in Fahrenheit : %02d F \n", fTemp);
//					int cTemp = ( ( ( (data3 * 256 ) + data4)  * 125) / 65536 ) - 6;

//				  printf("%02d \n",cTemp);
			     int data5 = ((data3 * 256 ) + data4);

					int Temperature = ((175.72 * data5) / 65536) - 46.85;


					printf("Temperature : %02d % \n", Temperature);

					 for(int i = 0;i<1000000;i++){

					     }

	}

    return 0 ;
}
