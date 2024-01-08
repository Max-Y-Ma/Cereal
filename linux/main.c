/* Standard Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "signal.h"

/* Library Includes */
#include "core/cereal.h"

/* User callback */
static void myhandler(uint8_t* buf);

cereal_handle_t hcereal1;

/* Signal handler function */
void sigterm_handler(int signum) {
    /* Close device */
    if (hcereal1->fd != 0) {
        cereal_halt(hcereal1);
    }

    /* Exit program */
    printf("Received SIGINT signal %d. Cleaning up...\n", signum);
    exit(EXIT_FAILURE);
}

int main() {
    /* Register the signal handler */
    if (signal(SIGINT, sigterm_handler) == SIG_ERR) {
        perror("Error registering signal handler");
        return EXIT_FAILURE;
    }

    /* Instantiate and initialize handler */
    hcereal1 = (cereal_handle_t) malloc(sizeof(cereal_t));
    cereal_init(hcereal1);
    cereal_register_callback(hcereal1, (cereal_callback)myhandler);

    /* Connect to serial device */
    if (cereal_connect(hcereal1) != 0) {
        fprintf(stderr, "ERROR: Unable to connect to device\n");
        exit(EXIT_FAILURE);
    }

    /* Run test */
    uint8_t buffer[RX_BUFFER_LENGTH];
    while(1) {
        cereal_receive(hcereal1, buffer, 32);
        cereal_transmit(hcereal1, (uint8_t*)"AT!", strlen("AT!"));
    }

    cereal_halt(hcereal1);

    return 0;
}

static void myhandler(uint8_t* buf)
{
    /* Process data in buffer */
    printf("%s", (char*)buf);
}