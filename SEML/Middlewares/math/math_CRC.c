#include "math_crc.h"

const CrcInfoType_t CRC4_ITU = {
		.Width = 4,
		.Poly = 0x03,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC5_EPC = {
		.Width = 5,
		.Poly = 0x09,
		.CrcInit = 0x09,
		.XorOut = 0x00,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC5_ITU = {
		.Width = 5,
		.Poly = 0x15,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC5_USB = {
		.Width = 5,
		.Poly = 0x05,
		.CrcInit = 0x1F,
		.XorOut = 0x1F,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC6_ITU = {
		.Width = 6,
		.Poly = 0x03,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC7_MMC = {
		.Width = 7,
		.Poly = 0x09,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC8 = {
		.Width = 8,
		.Poly = 0x07,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC8_ITU = {
		.Width = 8,
		.Poly = 0x07,
		.CrcInit = 0x00,
		.XorOut = 0x55,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC8_ROHC = {
		.Width = 8,
		.Poly = 0x07,
		.CrcInit = 0xFF,
		.XorOut = 0x00,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC8_MAXIM = {
		.Width = 8,
		.Poly = 0x31,
		.CrcInit = 0x00,
		.XorOut = 0x00,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_IBM = {
		.Width = 16,
		.Poly = 0x8005,
		.CrcInit = 0x0000,
		.XorOut = 0x0000,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_MAXIM = {
		.Width = 16,
		.Poly = 0x8005,
		.CrcInit = 0x0000,
		.XorOut = 0xFFFF,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_USB = {
		.Width = 16,
		.Poly = 0x8005,
		.CrcInit = 0xFFFF,
		.XorOut = 0xFFFF,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_MODBUS = {
		.Width = 16,
		.Poly = 0x8005,
		.CrcInit = 0xFFFF,
		.XorOut = 0x0000,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_CCITT = {
		.Width = 16,
		.Poly = 0x1021,
		.CrcInit = 0x0000,
		.XorOut = 0x0000,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_CCITT_FALSE = {
		.Width = 16,
		.Poly = 0x1021,
		.CrcInit = 0xFFFF,
		.XorOut = 0x0000,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC16_X25 = {
		.Width = 16,
		.Poly = 0x1021,
		.CrcInit = 0xFFFF,
		.XorOut = 0xFFFF,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC16_XMODEM = {
		.Width = 16,
		.Poly = 0x1021,
		.CrcInit = 0x0000,
		.XorOut = 0x0000,
		.RefIn = DISABLE,
		.RefOut = DISABLE};
const CrcInfoType_t CRC16_DNP = {
		.Width = 16,
		.Poly = 0x3D65,
		.CrcInit = 0x0000,
		.XorOut = 0xFFFF,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC32 = {
		.Width = 32,
		.Poly = 0x04C11DB7,
		.CrcInit = 0xFFFFFFFF,
		.XorOut = 0xFFFFFFFF,
		.RefIn = ENABLE,
		.RefOut = ENABLE};
const CrcInfoType_t CRC32_MPEG2 = {
		.Width = 32,
		.Poly = 0x04C11DB7,
		.CrcInit = 0xFFFFFFFF,
		.XorOut = 0x00000000,
		.RefIn = DISABLE,
		.RefOut = DISABLE};

/**
 * @brief   位反转 函数
 * @param   [in] inVal	- 反转前数据
 * @param   [in] bits   - 反转位数
 * @return  反转后数据
 * @note    就是将高位与低位数据顺序反过来。
 */
static uint32_t BitsReverse(uint32_t inVal, uint8_t bits)
{
	uint32_t outVal = 0;
	uint8_t i;

	for (i = 0; i < bits; i++)
	{
		if (inVal & (1 << i))
			outVal |= 1 << (bits - 1 - i);
	}

	return outVal;
}

/**
 * @brief   获取 CRC(循环冗余校验) 函数
 * @param   [in] crc_info		- CRC 类型
 * @param   [in] buf				- 数据块指针
 * @param   [in] bufLen			- 数据长度
 * @return  校验码
 */
uint32_t GetCRC(const CrcInfoType_t *crc_info, uint8_t *buf, uint32_t bufLen)
{
	uint32_t crc = crc_info->CrcInit; // 初始值,这是算法开始时寄存器（crc）的初始化预置值，十六进制表示。
	uint32_t poly = crc_info->Poly;
	uint8_t n;
	uint32_t bits;
	uint32_t data;
	uint8_t i;

	n = (crc_info->Width < 8) ? 0 : (crc_info->Width - 8);
	crc = (crc_info->Width < 8) ? (crc << (8 - crc_info->Width)) : crc; // CRC校验宽度小于8位,CRC初值移到高位计算
	bits = (crc_info->Width < 8) ? 0x80 : (1 << (crc_info->Width - 1));
	poly = (crc_info->Width < 8) ? (poly << (8 - crc_info->Width)) : poly; // CRC校验宽度小于8位,CRC多项式移到高位计算
	while (bufLen--)
	{
		data = *(buf++);
		if (crc_info->RefIn == ENABLE)
			data = BitsReverse(data, 8); // 数据输入是否反转
		crc ^= (data << n);
		for (i = 0; i < 8; i++)
		{
			if (crc & bits)
			{
				crc = (crc << 1) ^ poly;
			}
			else
			{
				crc = crc << 1;
			}
		}
	}
	crc = (crc_info->Width < 8) ? (crc >> (8 - crc_info->Width)) : crc; // CRC校验宽度小于8位,CRC值计算完成后移到原位
	if (crc_info->RefOut == ENABLE)
		crc = BitsReverse(crc, crc_info->Width); // CRC输出是否反转
	crc ^= crc_info->XorOut;									 // CRC结果异或

	return (crc & ((2 << (crc_info->Width - 1)) - 1)); // 获取有效位（也就是宽度位）
}
