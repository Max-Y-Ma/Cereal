#include "cereal.h"

void cereal_init(cereal_handle_t hcereal)
{
    /* Check handler */
    if (hcereal == NULL) {
        return;
    }

    /* Initialize handler to default values */
    hcereal->fd = 0;
    hcereal->cb = NULL;
}

void cereal_halt(cereal_handle_t hcereal)
{
    /* Close device */
    if (hcereal->fd != 0) {
        close(hcereal->fd);
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
    DIR *dir;
    struct dirent *entry;

    /* Open the /dev directory */
    int num_devices = 0;
    if ((dir = opendir("/dev")) != NULL) {
        /* Iterate through the directory entries */
        while ((entry = readdir(dir)) != NULL) {
            // Check if the entry starts with "ttyS" or "ttyUSB" (typical serial device prefixes)
            if (strncmp(entry->d_name, "ttyACM", 6) == 0 || strncmp(entry->d_name, "ttyUSB", 6) == 0) {
                printf("Serial device found: /dev/%s\n", entry->d_name);
                num_devices++;
            }
        }

        /* Close the directory */
        closedir(dir);
    } else {
        /* Unable to open directory */
        perror("Error opening /dev directory");
        return -1;
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

    /* Check if the file or device node exists */
    if (access(device_name, F_OK) != -1) {
        return 0;  /* File exists */
    } else {
        return -1;  /* File does not exist */
    }
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
    hcereal->fd = open(user_device_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (hcereal->fd == -1) {
        return 1;
    }

    /* Configure the serial port */
    struct termios serialConfig;
    tcgetattr(hcereal->fd, &serialConfig);

    /* Set baud rate and other serial settings (customize as needed) */
    cfsetispeed(&serialConfig, B115200);
    cfsetospeed(&serialConfig, B115200);
    serialConfig.c_cflag &= ~PARENB;  /* No parity */
    serialConfig.c_cflag &= ~CSTOPB;  /* 1 stop bit */
    serialConfig.c_cflag &= ~CSIZE;
    serialConfig.c_cflag |= CS8;  /* 8 data bits */
    serialConfig.c_cflag |= CREAD;  /* Enable receiver */
    serialConfig.c_cflag |= CLOCAL;  /* Ignore modem control lines */

    tcsetattr(hcereal->fd, TCSANOW, &serialConfig);

    return 0;
}

int32_t cereal_transmit(cereal_handle_t hcereal, uint8_t* tx_buf, uint32_t nbytes)
{
    /* Check handler */
    if (hcereal == NULL) {
        return -1;
    }

    /* Write contents of buffer to device */
    ssize_t bytes_written = write(hcereal->fd, tx_buf, nbytes);
    bytes_written += write(hcereal->fd, '\0', 1);

    return bytes_written;
}

int32_t cereal_receive(cereal_handle_t hcereal, uint8_t* rx_buf, uint32_t nbytes)
{
    /* Check handler */
    if (hcereal == NULL) {
        return -1;
    }

    /* Wait for available bytes */
    int bytes_available;
    do {
        ioctl(hcereal->fd, FIONREAD, &bytes_available);
    } while(bytes_available <= 0);

    /* Read bytes from device */
    memset(rx_buf, '\0', nbytes);
    ssize_t bytes_read = read(hcereal->fd, rx_buf, nbytes - 1);

    /* Execute registered callback */
    if (hcereal->cb != NULL) {
        hcereal->cb(rx_buf);
    }

    return bytes_read;
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
