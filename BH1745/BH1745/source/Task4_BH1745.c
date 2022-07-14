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
/* TODO: insert other include files here. */

#define SYSTEM_CONTROL 0x40

#define MODE_CONTROL1 0x41

#define MODE_CONTROL2 0x42

#define MODE_CONTROL3 0x44

#define RED_DATA_LSBs 0x50

#define RED_DATA_MSBs 0x51

#define GREEN_DATA_LSBs 0x52

#define GREEN_DATA_MSBs 0x53

#define BLUE_DATA_LSBs 0x54

#define BLUE_DATA_MSBs 0x55

#define CLEAR_DATA_LSBs 0x56

#define CLEAR_DATA_MSBs 0x57

#define DINT_DATA_LSBs 0x58

#define DINT_DATA_MSBs 0x59

#define INTERRUPT 0x60

#define PERSISTENCE 0x61

#define TH_LSBs 0x62

#define TH_MSBs 0x63

#define TL_LSBs 0x64

#define TL_LSBs 0x65

#define MANUFACTURER_ID 0x92



char reg_Addrs[8] = {0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57};

uint8_t i = 0;

char opData[8] = {0};

/* TODO: insert other definitions and declarations here. */

#define Slave_Addr 0x38

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



#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE

#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 0U

#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 1U


void i2c_write(uint8_t slaveAddr, uint8_t regAddr, uint8_t regVal)
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

	/* Send value to Slave  */
	I2C1->D = regVal;
	I2C1_WAIT;

	/* Generate STOP Condition */
	I2C1_STOP;

	return data;
}

uint8_t i2c_read( uint8_t slaveAddr,uint8_t regAddr)
{
	uint8_t data;
	uint8_t timeDelay = 6;

	/* I2C1 Check for Bus Busy */
	while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* Generate START Condition */
	I2C1_START;

	/* Send Slave Address */
	I2C1->D = (slaveAddr << 1);  //write
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
	I2C1->D = (slaveAddr << 1) | 0x01;  //read
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
	return data;
}



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

void delay(int value)
{

	while(value--)
	{

	}

}

int main(void) {

	    SIM->SCGC5 =  (1 << SIM_SCGC5_PORTE_SHIFT) | (1 << SIM_SCGC5_PORTA_SHIFT);   //clock enable for port E and A

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

        //System Control
		i2c_write(Slave_Addr, SYSTEM_CONTROL, 0x4B);

		//MODE_CONTROL1
		i2c_write(Slave_Addr, MODE_CONTROL1, 0x00);

		//MODE_CONTROL2
		i2c_write(Slave_Addr, MODE_CONTROL2, 0x90);

		//MODE_CONTROL3
		i2c_write(Slave_Addr, MODE_CONTROL3, 0x02);



    while(1) {

				for(i = 0; i < 8; i++){
					opData[i] = i2c_read(Slave_Addr, reg_Addrs[i]);
				}

				for(int j = 0;j<10000;j++){

				}

				int red = (opData[1] * 256 + opData[0]);
				int green = (opData[3] * 256 + opData[2]);
				int blue = (opData[5] * 256 + opData[4]);
				int cData = (opData[7] * 256 + opData[6]);

				// Output data to screen
				printf("Red color luminance : %d lux \n", red);
				printf("Green color luminance : %d lux \n", green);
				printf("Blue color luminance : %d lux \n", blue);
				printf("Clear Data  Luminance : %d lux \n ", cData);
				printf("************************************************\n\n");
				delay(10000000);



    }
    return 0 ;
}
