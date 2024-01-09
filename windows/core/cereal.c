#include "cereal.h"

void cereal_init(cereal_handle_t hcereal)
{
    /* Check handler */
    if (hcereal == NULL) {
        return;
    }

    /* Initialize handler to default values */
    hcereal->fd = NULL;
    hcereal->cb = NULL;
}

void cereal_halt(cereal_handle_t hcereal)
{
    /* Close device */
    if (hcereal->fd != 0) {
        CloseHandle(hcereal->fd);
    }
}

/**
 * @brief Scans for serial devices in /dev directory.
 * 
 * @return Returns number of serial devices found.
*/
static int scan_devices()
{
    /* Scan for serial devices */
    int num_devices = 0;
    for (int i = 1; i <= 256; i++) {
        char portName[10];
        snprintf(portName, sizeof(portName), "COM%d", i);

        HANDLE hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (hSerial != INVALID_HANDLE_VALUE) {
            // The port is available
            printf("Found COM port: %s\n", portName);
            CloseHandle(hSerial);
            num_devices++;
        }
    }

    return num_devices;
}

/**
 * @brief Checks if a file or device node exists.
 * 
 * @param device_name Name of device to check.
 * 
 * @return int Returns 0 on success, -1 on failure.
*/
static int32_t check_device(char* device_name)
{
    /* Check argument */
    if (device_name == NULL) {
        return -1;
    }

    HANDLE hSerial = CreateFile(device_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        return -1; /* Port is not available */
    }

    /* The port is available */
    CloseHandle(hSerial);
    return 0;
}

int32_t cereal_connect(cereal_handle_t hcereal)
{
    /* Check handler */
    if (hcereal == NULL) {
        return -1;
    }

    /* Prompt user for device */
    char user_device_name[MAX_DEV_NAME_LENGTH];
    if (scan_devices() == 0) {
        return -1;
    }

    printf("Enter device name: ");
    scanf("%s", user_device_name);
    if (check_device(user_device_name) != 0) {
        return -1;
    }

    /* Open device */
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

    /* Open COM port */
    hSerial = CreateFile(user_device_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening COM port\n");
        return -1;
    }

    /* Configure COM port settings */
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error getting COM state\n");
        CloseHandle(hSerial);
        return -1;
    }

    dcbSerialParams.BaudRate = CBR_115200;  // Set your desired baud rate
    dcbSerialParams.ByteSize = 8;         // 8 data bits
    dcbSerialParams.Parity = NOPARITY;    // No parity
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 stop bit

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error setting COM state\n");
        CloseHandle(hSerial);
        return -1;
    }

    /* Set timeouts */
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return -1;
    }

    /* Save handle */
    hcereal->fd = hSerial;

    return 0;
}

int32_t cereal_transmit(cereal_handle_t hcereal, uint8_t* tx_buf, uint32_t nbytes)
{
    /* Check handler */
    if (hcereal == NULL) {
        return -1;
    }

    DWORD bytes_written;
    if (!WriteFile(hcereal->fd, tx_buf, nbytes, &bytes_written, NULL)) {
        fprintf(stderr, "Error writing to COM port\n");
        return -1;
    }

    /* Write contents of buffer to device */
    return bytes_written;
}

int32_t cereal_receive(cereal_handle_t hcereal, uint8_t* rx_buf, uint32_t nbytes)
{
    /* Check handler */
    if (hcereal == NULL) {
        return -1;
    }

    /* Check bytes are available */
    COMSTAT status;
    DWORD errors;
    do {
        ClearCommError(hcereal->fd, &errors, &status);
    } while(status.cbInQue <= 0);

    /* Read available bytes from device */
    DWORD bytesRead;
    memset(rx_buf, '\0', nbytes);
    if (ReadFile(hcereal->fd, rx_buf, nbytes - 1, &bytesRead, NULL)) {
        if (hcereal->cb != NULL) {
            hcereal->cb(rx_buf);
        }
    } else {
        fprintf(stderr, "Error reading from COM port\n");
        return -1;
    }

    return bytesRead;
}

void cereal_register_callback(cereal_handle_t hcereal, cereal_callback cb)
{
    /* Check handler */
    if (hcereal == NULL) {
        return;
    }

    /* Set handler's callback function */
    hcereal->cb = cb;
}
