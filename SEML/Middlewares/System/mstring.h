/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : mstring.h
 * @author : SY7_yellow
 * @brief  : 字符串处理模块
 * @todo   : 增加对浮点数的处理
 * @bug    : 字符串处理有bug
 * @see    : https://www.cnblogs.com/smartlife/articles/10237090.html
 * @date   : 2023-9-20
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-09-12     <td>1.0         <td>SY7_yellow  <td>实现sprintf
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 *
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef _MSTRING_H_
#define _MSTRING_H_
#include "SEML_common.h"

int SEML_sprintf(char *buf, const char *fmt, ...);

#endif