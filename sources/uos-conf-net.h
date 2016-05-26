/*
 * uos-conf-net.h
 *  Created on: 11.12.2015
 *      Author: a_lityagin <alexraynepe196@gmail.com>
 *                         <alexraynepe196@hotbox.ru>
 *                         <alexraynepe196@mail.ru>
 *                         <alexrainpe196@hotbox.ru>
 *  
 *  *\~russian UTF8
 *  это дефолтовый конфигурационный файл уОС. здесь сведены настройки модулей 
 *  свзаные с сетевой системой.
 *  для сборки своей оси, скопируйте этот файл в папку своего проекта и 
 *      переопределите настройки.
 */

#ifndef UOS_CONF_NET_H_
#define UOS_CONF_NET_H_

/**************************************************************************
 *                              debug console
 ************************************************************************** */
//#define DEBUG_NET_ETH
//#define DEBUG_NET_ETH_FAIL
//#define DEBUG_NET_PHY
//#define ARP_TRACE
//#define DEBUG_NET_ARPTABLE
//#define DEBUG_NET_ROUTE
//#define DEBUG_NET_IP
//#define DEBUG_NET_ICMP

 /**************************************************************************
 *                              net
 ************************************************************************** */
//#   define ETH_STACKSZ        1000
//#   define ETH_INQ_SIZE       16
//#   define ETH_OUTQ_SIZE      8
/** \~russian этот резерв старается удерживать свободные слоты в буфере передачи
 * отвергая пакеты с оверлапами. позволяет защитить канал передачи от полной загрузки 
 * */
//#   define ETH_OUTQ_PRESERVE  2
//#   define ETH_MTU            1518    /* maximum ethernet frame length */

//#   define ETH_MDIO_KHZ       2500ul
//*   этот таймаут задает время устаревания PHY_STASTUS, после которого eth_phy_poll форсирует запрос статуса
//      с блокированием управляющей нитки 
//#   define ETH_PHY_STASTUS_TOus    10000ul


#define ETH_OPTIMISE_SPEED      1

//#define UDP_CHECKSUM_DISABLE



/* **********************    ELVEES     ************************* */
// облегчаем жизнь просессору за счет устранения циклов ожидания, позволяет отдать 
//  до 10мкс на пакетах более 512 байт
//  но чуть ухудшает скорость запуска передатчика за счет вхождения в прерывание 
#define ETH_TX_USE_DMA_IRQ      0

//*этот параметр задает порог длины приемного/передаваемого буфера, с которой начинаем использовать
//*  ожидание на мутехе, при меньшей длине буфера, жду его передачи поллингом.
//*  позволит избежать лишней нагрузки на переключениях контекста 
#define ETH_DMA_WAITPOLL_TH    0x200
/* **********************    ~ELVEES~     ************************* */



/**************************************************************************
 * TCP options
 */
//#define TCP_WND                         2048
//#define TCP_MAXRTX                      12
//#define TCP_SYNMAXRTX                   6

/// TCP Maximum segment size. 256 /* conservative default
//#define TCP_MSS             1460

/* TCP sender buffer space (bytes). */
//#define TCP_SND_BUF                     2048

/* TCP sender buffer space (pbufs).
 * This must be at least = 2 * TCP_SND_BUF/TCP_MSS for things to work. */
//#define TCP_SND_QUEUELEN                4 * TCP_SND_BUF/TCP_MSS


/* Maximum number of retransmissions of data segments. */

/* Maximum number of retransmissions of SYN segments. */

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
//#define TCP_SNDLOWAT                    TCP_SND_BUF/2

//#define TCP_TMR_INTERVAL    100 /* TCP timer interval in msec. */
//#define TCP_FAST_INTERVAL   200 /* fine grained timeout in msec */
//#define TCP_SLOW_INTERVAL   500 /* coarse grained timeout in msec */
//#define TCP_STUCK_TIMEOUT   5000    /* milliseconds */

//#define TCP_MSL         60000  /* maximum segment lifetime in usec */

//#define TCP_LOCAL_PORT_RANGE_START 4096
//#define TCP_LOCAL_PORT_RANGE_END   0x7fff

//#define TCP_DEBUG

/*
#include <buffers/rtlog_buf.h>
extern rtlog tcp_log;
#define RT_PRINTF(...) rtlog_printf(&tcp_log, __VA_NARG__(__VA_ARGS__), __VA_ARGS__)
#define TCP_PRINTF(...) RT_PRINTF(__VA_ARGS__)
#define IP_PRINTF(...)  RT_PRINTF(__VA_ARGS__)
#define ETH_PRINTF(...)  RT_PRINTF(__VA_ARGS__)
*/

#endif /* UOS_CONF_NET_H_ */
