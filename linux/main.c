/* Standard Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* Library Includes */
#include "core/cereal.h"

/* User callback */
static void myhandler(uint8_t* buf);

int main() {
    /* Instantiate and initialize handler */
    cereal_handle_t hcereal1 = (cereal_handle_t) malloc(sizeof(cereal_t));
    cereal_init(hcereal1);
    cereal_register_callback(hcereal1, (cereal_callback)myhandler);

    /* Connect to serial device */
    if (cereal_connect(hcereal1) != 0) {
        fprintf(stderr, "ERROR: Unable to connect to device\n");
        exit(EXIT_FAILURE);
    }

    /* Run test */
    cereal_transmit(hcereal1, (uint8_t*)"Hello!", strlen("Hello!"));

    uint8_t buffer[RX_BUFFER_LENGTH];
    cereal_receive(hcereal1, buffer, RX_BUFFER_LENGTH);

    if (strcmp((char*)buffer, "ACK") != 0) {
        fprintf(stderr, "ERROR: No device response\n");
        return -1;
    }

    return 0;
}

static void myhandler(uint8_t* buf)
{
    /* Process data in buffer */
    printf("%s", (char*)buf);
}