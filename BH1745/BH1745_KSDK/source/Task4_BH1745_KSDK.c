/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define ACCEL_I2C_CLK_SRC I2C1_CLK_SRC
#define ACCEL_I2C_CLK_FREQ CLOCK_GetFreq(I2C1_CLK_SRC)
#define I2C_BAUDRATE 100000U
#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 0U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 1U
#define I2C_RELEASE_BUS_COUNT 100U

#define SLAVE_ADDRESS 0x38



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
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static bool I2C_Init(void);
static bool I2C_Write(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value);
static bool I2C_Read(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);

/*******************************************************************************
 * Variables
 ******************************************************************************/
i2c_master_handle_t g_m_handle;
volatile bool completionFlag = false;
volatile bool nakFlag = false;

uint8_t newValues[8]={0};
/*******************************************************************************
 * Code
 ******************************************************************************/

static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++)
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

static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        completionFlag = true;
    }
    /* Signal transfer success when received success status. */
    if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak))
    {
        nakFlag = true;
    }
}
static bool I2C_Init(void)
{
    uint32_t sourceClock = 0;

    i2c_master_config_t masterConfig;

    I2C_MasterGetDefaultConfig(&masterConfig);

    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    sourceClock = ACCEL_I2C_CLK_FREQ;

    I2C_MasterInit(I2C1, &masterConfig, sourceClock);
}

static bool I2C_Write(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t value)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = device_addr;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data = &value;
    masterXfer.dataSize = 1;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

    I2C_MasterTransferNonBlocking(base, &g_m_handle, &masterXfer);

    /*  wait for transfer completed. */
    while ((!nakFlag) && (!completionFlag))
    {
    }

    nakFlag = false;

    if (completionFlag == true)
    {
        completionFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}

static bool I2C_Read(I2C_Type *base, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    i2c_master_transfer_t masterXfer;
    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = device_addr;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data = rxBuff;
    masterXfer.dataSize = rxSize;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=recive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

    I2C_MasterTransferNonBlocking(I2C1, &g_m_handle, &masterXfer);

    /*  wait for transfer completed. */
    while ((!nakFlag) && (!completionFlag))
    {
    }

    nakFlag = false;

    if (completionFlag == true)
    {
        completionFlag = false;
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * @brief Main function
 */

int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_I2C_ReleaseBus();
    BOARD_I2C_ConfigurePins();
    BOARD_InitDebugConsole();

    I2C_MasterTransferCreateHandle(I2C1, &g_m_handle, i2c_master_callback, NULL);

    I2C_Init();

    I2C_Write(I2C1, SLAVE_ADDRESS, SYSTEM_CONTROL, 0x4B);
	I2C_Write(I2C1, SLAVE_ADDRESS, MODE_CONTROL1, 0x00);
	I2C_Write(I2C1, SLAVE_ADDRESS, MODE_CONTROL2, 0x90);
	I2C_Write(I2C1, SLAVE_ADDRESS, MODE_CONTROL3, 0x02);




    while(1){



    	I2C_Read(I2C1, SLAVE_ADDRESS, 0x50, newValues, 8);

    	int red = (newValues[1] * 256 + newValues[0]);
		int green = (newValues[3] * 256 + newValues[2]);
		int blue = (newValues[5] * 256 + newValues[4]);
		int cData = (newValues[7] * 256 + newValues[6]);

		// Output data to screen
		printf("Red color luminance : %d lux \n", red);
		printf("Green color luminance : %d lux \n", green);
		printf("Blue color luminance : %d lux \n", blue);
		printf("Clear Data  Luminance : %d lux \n ", cData);

		//delay
    	for(int i=0; i<10000000;i++){

    	}

    }

}
