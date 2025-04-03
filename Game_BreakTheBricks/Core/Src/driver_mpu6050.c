#include "driver_mpu6050.h"
#include "i2c.h"
#include "driver_oled_hardware.h"


static QueueHandle_t g_xQueueMPU6050; /* MPU6050队列 */
static EventGroupHandle_t g_xEventGroupMPU6050 = NULL;
static SemaphoreHandle_t g_xI2C2Mutex;

/**
  * 函    数：MPU6050写寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 参    数：Data 要写入寄存器的数据，范围：0x00~0xFF
  * 返 回 值：无
  */
static int MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data){
    uint8_t tempBuf[2];
    tempBuf[0] = RegAddress;
    tempBuf[1] = Data;

    return HAL_I2C_Master_Transmit(&hi2c2, MPU6050_ADDRESS, tempBuf, 2, MPU6050_TIMEOUT);
}

/**
  * 函    数：MPU6050读寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 返 回 值：读取寄存器的数据，范围：0x00~0xFF
  */
static int MPU6050_ReadReg(uint8_t RegAddress, uint8_t *pData){
    return HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDRESS, RegAddress, 1, pData, 1, MPU6050_TIMEOUT);
}

/**
  * 函    数：MPU6050初始化
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_Init(void){
    // 电源管理寄存器1,取消睡眠模式,选择时钟源为内部集成的晶振
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);
    // 电源管理寄存器2,保持默认值0,所有轴均不待机
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    // 采样率分频寄存器,配置采样率
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    // 配置寄存器,配置DLPF
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    // 陀螺仪配置寄存器,选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    // 加速度计配置寄存器,选择满量程为±16g
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);

    /* 中断配置  */
    // MPU6050_WriteReg(MPU6050_INT_PIN_CFG, 0);
    /* 中断使能  */
    // MPU6050_WriteReg(MPU6050_INT_ENABLE, 0xff);

    g_xQueueMPU6050 = xQueueCreate(MPU6050_QUEUE_LEN, sizeof(MPU6050Data));
    g_xI2C2Mutex = xSemaphoreCreateMutex();
    // g_xEventGroupMPU6050 = xEventGroupCreate();

}

/**
  * 函    数：MPU6050获取ID号
  * 参    数：无
  * 返 回 值：MPU6050的ID号
  */
uint8_t MPU6050_GetID(void){
    uint8_t id;
    if (0 == MPU6050_ReadReg(MPU6050_WHO_AM_I, &id))
    {
        return id;
    }else{
        return -1;
    }
}

/**
  * 函    数：MPU6050获取数据
  * 参    数：AccX AccY AccZ 加速度计X、Y、Z轴的数据，使用输出参数的形式返回，范围：-32768~32767
  * 参    数：GyroX GyroY GyroZ 陀螺仪X、Y、Z轴的数据，使用输出参数的形式返回，范围：-32768~32767
  * 返 回 值：无
  */
int MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                        int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t DataH,DataL;
    int err = 0;

    if (AccX)
    {
        err |= MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L, &DataL);
        *AccX = (DataH << 8) | DataL;
    }
    
    if (AccY)
    {
        err |= MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L, &DataL);
        *AccY = (DataH << 8) | DataL;
    }

    if (AccZ)
    {
        err |= MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L, &DataL);
        *AccZ = (DataH << 8) | DataL;
    }

    if (GyroX)
    {
        err |= MPU6050_ReadReg(MPU6050_GYRO_XOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_GYRO_XOUT_L, &DataL);
        *GyroX = (DataH << 8) | DataL;
    }

    if (GyroY)
    {
        err |= MPU6050_ReadReg(MPU6050_GYRO_YOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_GYRO_YOUT_L, &DataL);
        *GyroY = (DataH << 8) | DataL;
    }

    if (GyroZ)
    {
        err |= MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H, &DataH);
        err |= MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L, &DataL);
        *GyroZ = (DataH << 8) | DataL;
    }
    return err;
}


void CaptureI2C2(void){
    xSemaphoreTake(g_xI2C2Mutex, portMAX_DELAY);
};
void ReleaseI2C2(void){
    xSemaphoreGive(g_xI2C2Mutex);
}

void MPU6050_Task(void *params){
    int16_t AccY;
    MPU6050Data result;
    extern volatile int bInUsed;
    int ret;

    while (1)
    {
        // xEventGroupWaitBits(g_xEventGroupMPU6050, (1<<0), pdTRUE, pdFALSE, portMAX_DELAY);
        CaptureI2C2();
        // while (bInUsed);       
        // bInUsed = 1;
        ret = MPU6050_GetData(NULL, &AccY, NULL, NULL, NULL, NULL);
        // bInUsed = 0;
        ReleaseI2C2();

        if (0 == ret)
        {
            result.angle_x = AccY;
            xQueueSend(g_xQueueMPU6050, &result, 0);
        }
        vTaskDelay(50);
    }
}

QueueHandle_t GetQueueMPU6050(void){
    return g_xQueueMPU6050;
}

void MPU6050_Test(void){
    int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
    MPU6050_Init();

    while (1)
    {
        MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

        OLED_ShowString(1, 1, "X:");
        OLED_ShowSignedNum(1, 3, AccX, 6);

        OLED_ShowString(2, 1, "Y:");
        OLED_ShowSignedNum(2, 3, AccY, 6);

        OLED_ShowString(3, 1, "Z:");
        OLED_ShowSignedNum(3, 3, AccZ, 6);

        HAL_Delay(100);
        OLED_Clear();
    }
    
}

// void MPU6050_Callback(void){
	
//     xEventGroupSetBitsFromISR(g_xEventGroupMPU6050, (1<<0), NULL);
    
// }
