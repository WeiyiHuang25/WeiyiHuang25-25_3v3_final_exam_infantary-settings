#include "math_fast.h"

/**
 * @brief 快速exp
 * @param[in] x 输入值
 * @return e^x的估算值(±4%误差)
 * @see https://www.796t.com/content/1550634858.html
 */
float fast_exp(float x)
{
    int n;
    unsigned int temp;

    math_assert_param(!IS_NAN_INF(x));
    
    n = x;
    x -= n;
    if (x == 0.0f)
    {
        x = 1.0f;
    }
    else
    {
        // 计算小数部分exp
        temp = 12102203 * x + 1064866803;
        *((unsigned int *)&x) = temp;
        // 整数部分使用快速幂
        x *= math_pow(e, n);
    }

    return x;
}

/**
 * @brief 快速自然对数
 * @param[in] x 输入值
 * @return lnx的估算值(±6%误差)
 */
float fast_ln(float x)
{
    const integer_t iterations = 3;
    float y, temp;
    int n = *(int *)&x;

    math_assert_param(!IS_NAN_INF(x));

    if (x <= 0)
    {
        // 输入参数错误
        if (x < 0)
        {
            math_errno = MATH_PARAMETERS_ERROR;
        }
        // INF错误
        else
        {
            math_errno = MATH_NANINF;
        }
    }
    // 在(0.6,1.4)内使用泰勒二阶展开
    if (abs(x - 1) < 0.4f)
    {
        x -= 1;
        y = x - x * x * 0.5f;
    }
    // 超出范围约化估值后进行牛顿迭代
    else
    {
        // 估值，用阶码计算出log2的整数位置，然后转换为ln
        n = (n >> 23) - 127;
        y = n * 0.69314718f - 0.0298053f;
        // 牛顿迭代
        for (int i = 0; i < iterations; i++)
        {
            temp = fast_exp(y);
            if (temp == 0)
            {
                math_errno = MATH_NANINF;
                return 0;
            }
            y = y - (1 - x / temp);
        }
    }
    return y;
}

// /// @brief 快速正弦函数
// /// @param x 输入值(弧度)
// /// @return sinx结果(±0.3%误差)
// /// @see https://zhuanlan.zhihu.com/p/585849724
// /// @todo 没泰勒快,吉祥物来着
// float fast_sin(float x)
// {
//     const float hollyst = 0.0174532925f;
//     const float sin_table[] =
//         {
//             0.0,           // sin(0)
//             0.1736481777f, // sin(10)
//             0.3420201433f, // sin(20)
//             0.5,           // sin(30)
//             0.6427876097f, // sin(40)
//             0.7660444431f, // sin(50)
//             0.8660254038f, // sin(60)
//             0.9396926208f, // sin(70)
//             0.9848077530f, // sin(80)
//             1.0            // sin(90)
//         };
//     const float cos_table[] =
//         {
//             1.0,           // cos(0)
//             0.9998476952f, // cos(1)
//             0.9993908270f, // cos(2)
//             0.9986295348f, // cos(3)
//             0.9975640503f, // cos(4)
//             0.9961946981f, // cos(5)
//             0.9945218954f, // cos(6)
//             0.9925461516f, // cos(7)
//             0.9902680687f, // cos(8)
//             0.9876883406f  // cos(9)
//         };
//     int32_t sig = 0;
//     int32_t n;
//     math_assert_param(!IS_NAN_INF(x));

//     x = x * 180 / PI;
//     x = x - (int)(x / 360) * 360;
//     if (x < 0)
//         x += 360.0;
//     if (x >= 180.0)
//     {
//         sig = 1;
//         x = x - 180.0;
//     }

//     x = (x > 90.0) ? (180.0 - x) : x;

//     int a = x * 0.1;
//     float b = x - 10 * a;

//     float y = sin_table[a] * cos_table[(int)b] + b * hollyst * sin_table[9 - a];

//     return (sig > 0) ? -y : y;
// }

/**
 * @brief 快速正切计算
 * @param[in] x 输入值
 * @return arctan估算值(0.1%误差)
 */
float fast_atan(float x)
{
	float absx;
	if (x > 1)
		return PI / 2.0f - fast_atan(1 / x);
	else if (x < -1)
		return -(PI / 2.0f - fast_atan(-1 / x));
	*(int32_t *)&absx = *(int32_t *)&x & 0x7fffffff;
	return x * ((-0.0663f * absx - 0.1784f) * absx + PI / 4.0f + 0.2447f);
}

/**
 * @brief 快速正切计算(全象限)
 * @param[in] x cosx值
 * @param[in] y sinx值
 * @return arctan估算值(0.1%误差)
 */
float fast_atan2(float y,float x)
{
    float temp = math_atan(y/x);
    if (x>0)  
        return temp;  
    else if (y >= 0 && x < 0)  
        return temp + PI;  
    else if (y < 0 && x < 0)  
        return temp - PI;  
    else if (y > 0 && x == 0)  
        return PI / 2;  
    else if (y < 0 && x == 0)  
        return -1 * PI / 2;  
    else  
        return 0;
}  
