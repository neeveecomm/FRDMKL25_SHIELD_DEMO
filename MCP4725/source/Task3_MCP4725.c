
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

/* TODO: insert other definitions and declarations here. */

#define Slave_Addr 0x64

#define Reg_Addr 0x40

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

 // Fast Mode Command
void setVoltage_Fast_Mode(float volt){

	 uint16_t value = (volt*4096.0)/3.3 ;
//	 printf("  data : %d \n ", value);

	 uint8_t var1 =( (value >> 8) & 0x0f ) ;
//	printf(" data of 0xf00 : %x \n ", var1);

	uint8_t var2 =(( value << 4) & 0x0ff ) ;
//	printf(" data of 0x0ff : %x \n ", var2);

	dac_i2c_write(var1, var2);

}

// Only for DAC Mode

   void setVoltage_DAC_Mode(float volt){

			 uint16_t value = (volt*4096.0)/3.3 ;
//			 printf("  data : %d \n ", value);

			uint8_t var1 =(value & 0xff0 ) >> 4;
//			printf(" data of 0xff0 : %x \n ", var1);

			uint8_t var2 = (value & 0xf ) << 4;
//			printf(" data of 0xf : %x \n ", var2);

			dac_i2c_write_Mode1(Slave_Addr, Reg_Addr, var1, var2);

		}

   // write function for Fast Mode Command

void dac_i2c_write (uint8_t value1 , uint8_t value2 ){

	uint16_t data;



	/* I2C1 Check for Bus Busy */
		while(I2C1->S & (1 << I2C_S_BUSY_SHIFT));

	/* Generate START Condition */
	    I2C1_START;

	/* Send Slave Address */
		I2C1->D = (Slave_Addr << 1);
		I2C1_WAIT;


	/* Send data  */
		I2C1->D = value1;
		I2C1_WAIT;

	 /* Send data  */
		I2C1->D = value2;
		I2C1_WAIT;


	/* Generate STOP Condition */
		I2C1_STOP;


		return data;
}

// write function for only DAC Mode

void dac_i2c_write_Mode1 (uint8_t slaveAddr , uint8_t regAddr, uint8_t value1 , uint8_t value2 ){

	uint16_t data;



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

	/* Send data  */
		I2C1->D = value1;
		I2C1_WAIT;

	 /* Send data  */
		I2C1->D = value2;
		I2C1_WAIT;


	/* Generate STOP Condition */
		I2C1_STOP;


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

void delay(int value)
{
while(value--)
{

}

}

int main(void) {


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



		float volt = 0.0;  //initial value
		float max_Volt = 3.2;




   while(1){

	   	   	   if(volt <= max_Volt){
	   	   		setVoltage_DAC_Mode(volt);
			    volt += 0.1;
				delay(1000000);
	   	   	   }
	   	   	   else{
	   	   		volt = 0.0;
	   	   	   }

   }
	return 0 ;
}
