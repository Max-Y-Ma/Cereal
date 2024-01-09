/* Standard Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "signal.h"

/* Library Includes */
#include "windows.h"
#include "core/cereal.h"

/* User callback */
static void myhandler(uint8_t* buf);

cereal_handle_t hcereal1;

BOOL SigHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        case CTRL_C_EVENT:
            printf("Ctrl+C received. Cleaning up and exiting...\n");
            cereal_halt(hcereal1);
            exit(EXIT_FAILURE);
        default:
            return FALSE;
    }
}

int main() {
    /* Setup signal handler */
    if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)SigHandler, TRUE) == FALSE) {
        fprintf(stderr, "ERROR: Unable to install handler\n");
        exit(EXIT_FAILURE);
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
        cereal_receive(hcereal1, buffer, 2);
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