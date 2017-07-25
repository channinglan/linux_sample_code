/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2009-2013 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 * [utils.h]
 *
 * This file contains a miscelaneous set of utility routines and macros.
 *
 * @(#) $Id: utils.h 587 2013-11-17 15:51:22Z ertl-honda $  
 */
#ifndef _SAFEG_MONITOR_COMMON_UTILS_H_
#define _SAFEG_MONITOR_COMMON_UTILS_H_

/******************************
 * [Useful macros]            *
 * Macros that may be useful. *
 ******************************/
#define UNUSED(x) (x) __attribute__((__unused__))
#define REG(x) (*((volatile unsigned int *)(x)))

/********************************************************
 * [String utils]                                       *
 * String manipulation utilities (not UNIX-compatible). *
 ********************************************************/
/* Copy a string, until a '\0' is found or all bytes are copied. */
static inline void string_copy(uint8_t *dest,
                               const uint8_t *src,
                               uint32_t bytes)
{
    for (uint32_t i=0; i<bytes; i++) {
        dest[i] = src[i];
        if (dest[i] == '\0') return; // no padding
    }
}

/* Compare to strings until a '\0' is found or all is compared. */
static inline uint32_t string_is_equal(const uint8_t *s1,
                                       const uint8_t *s2,
                                       uint32_t bytes)
{
    for (uint32_t i=0; i<bytes; i++) {
        if (s1[i] != s2[i]) return false;
        if (s1[i] == '\0') break;
    }
    return true;
}

/* Calculate the lenght of a string */
static inline uint32_t string_length(const char *string)
{
    const char *s;

    s = string;
    while (*s) s++;

    return s - string;
}

/* Reverse the characters in a string */
static inline void string_reverse(char *str) {
    char *p1, *p2;

    if (!str || !*str) return;

    for (p1 = str, p2 = str + string_length(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
}

/* Convert unsigned integer to string */
static inline void uint32_to_string(uint32_t number, char *s, uint32_t base)
{
    static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0;

    do {
        s[i++] = digits[number % base];
    } while ((number /= base) > 0);

    s[i] = '\0';

    string_reverse(s);
}

#endif /* _SAFEG_MONITOR_COMMON_UTILS_H_ */
