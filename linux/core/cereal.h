#ifndef _CEREAL_H_
#define _CEREAL_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <linux/types.h>
#include <sys/ioctl.h>

#define TX_BUFFER_LENGTH        1024
#define RX_BUFFER_LENGTH        1024

#define MAX_DEV_NAME_LENGTH     256

/* Data Structures */
typedef void (*cereal_callback)(uint8_t* buf);
typedef struct cereal_t{
    int32_t fd;
    uint8_t rx_buf[RX_BUFFER_LENGTH];
    uint8_t tx_buf[TX_BUFFER_LENGTH];
    uint8_t has_callback;
    cereal_callback cb;
} cereal_t;
typedef cereal_t* cereal_handle_t;

/**
 * @brief Initializes cereal handler to default values.
 * 
 * @param hcereal Pointer to cereal handler.
*/
void cereal_init(cereal_handle_t hcereal);

/**
 * @brief Shutsdown cereal handler.
 * 
 * @param hcereal Pointer to cereal handler.
*/
void cereal_halt(cereal_handle_t hcereal);

/**
 * @brief Connects to serial device.
 * 
 * @param hcereal Pointer to cereal handler.
 * 
 * @return int Returns 0 on success, -1 on failure.
*/
int32_t cereal_connect(cereal_handle_t hcereal);

/**
 * @brief Transmits data to serial device.
 * 
 * @param hcereal Pointer to cereal handler.
 * @param tx_buf Pointer to transmit buffer.
 * @param nbytes Number of bytes to transmit.
 * 
 * @return Returns number of bytes transmitted, -1 on failure.
*/
int32_t cereal_transmit(cereal_handle_t hcereal, uint8_t* tx_buf, uint32_t nbytes);

/**
 * @brief Receives data from serial device.
 * 
 * @param hcereal Pointer to cereal handler.
 * @param rx_buf Pointer to receive buffer.
 * @param nbytes Number of bytes to receive.
 * 
 * @return Returns number of bytes received, -1 on failure.
*/
int32_t cereal_receive(cereal_handle_t hcereal, uint8_t* rx_buf, uint32_t nbytes);

/**
 * @brief Registers callback function.
 * 
 * @param hcereal Pointer to cereal handler.
 * @param cb Callback function.
*/
void cereal_register_callback(cereal_handle_t hcereal, cereal_callback cb);

#endif /* _CEREAL_H_ */