#include "u8g2_config.h"

/// @brief u8g2通讯回调函数
/// @param u8x8 u8g2配置结构体
/// @param msg 指令类型
/// @param arg_int 数据大小
/// @param arg_ptr 数据指针
/// @return 运行状态
/// @retval 0 没有该指令
/// @retval 1 执行完成
uint8_t u8x8_byte_seml_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buf_idx;
  uint8_t *data;

  switch (msg)
  {

  case U8X8_MSG_BYTE_SEND:
    data = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
      buffer[buf_idx++] = *data;
      data++;
      arg_int--;
    }
    break;

  case U8X8_MSG_BYTE_INIT:
    /* add your custom code to init i2c subsystem */
    break;

  case U8X8_MSG_BYTE_START_TRANSFER:
    buf_idx = 0;
    break;

  case U8X8_MSG_BYTE_END_TRANSFER:
  #ifdef USE_SEML_LIB
    SEML_I2C_Transmit(&u8x8->I2C_Handle, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 1);
  #else
    // 不使用SEML库的情况下在这添加I2C发送函数
    HAL_I2C_Master_Transmit(&hi2c2, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 1);
  #endif
    break;

  default:
    return 0;
  }
  return 1;
}
/// @brief u8g2延时回调函数
/// @param u8x8 u8g2配置结构体
/// @param msg 指令类型
/// @param arg_int 数据大小
/// @param arg_ptr 数据指针
/// @return 运行状态
/// @retval 0 没有该指令
/// @retval 1 执行完成
uint8_t u8g2_gpio_and_delay_seml(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_GPIO_AND_DELAY_INIT:
    break;

  case U8X8_MSG_DELAY_MILLI:
    SEML_Delay(arg_int);
    break;

  case U8X8_MSG_GPIO_I2C_CLOCK:
    break;

  case U8X8_MSG_GPIO_I2C_DATA:
    break;

  default:
    return 0;
  }
  return 1; // command processed successfully.
}
