#ifndef _CEREAL_H_
#define _CEREAL_H_

#include "stdint.h"

#include <linux/types.h>

#define TX_BUFFER_LENGTH    1024
#define RX_BUFFER_LENGTH    1024

/* Data Structures */
typedef void (*cereal_callback)(uint8_t* buf);
typedef struct cereal_t{
    uint32_t fd;
    uint8_t rx_buf[RX_BUFFER_LENGTH];
    uint8_t tx_buf[TX_BUFFER_LENGTH];
    uint8_t has_callback;
    cereal_callback cb;
} cereal_t;
typedef cereal_t* cereal_handle_t;


/* Function Prototypes */
void cereal_init(cereal_handle_t hcereal);
void cereal_connect(cereal_handle_t hcereal);
void cereal_transmit(cereal_handle_t hcereal, uint8_t* tx_buf, uint32_t nbytes);
void cereal_receive(cereal_handle_t hcereal, uint8_t* rx_buf, uint32_t nbytes);
void cereal_register_callback(cereal_handle_t hcereal, cereal_callback cb);

#endif /* _CEREAL_H_ */