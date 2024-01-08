#include "cereal.h"

void cereal_init(cereal_handle_t hcereal)
{
    /* Initialize handler to default values */
    hcereal->fd = 0;
    memset(hcereal->rx_buf, 0, RX_BUFFER_LENGTH);
    memset(hcereal->tx_buf, 0, TX_BUFFER_LENGTH);
    hcereal->has_callback = false;
    hcereal->cb = NULL;
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
int check_device(char* device_name)
{
    /* Check if the file or device node exists */
    if (access(device_name, F_OK) != -1) {
        return 0;  /* File exists */
    } else {
        return -1;  /* File does not exist */
    }
}

int cereal_connect(cereal_handle_t hcereal)
{
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

    return 0;
}

void cereal_transmit(cereal_handle_t hcereal, uint8_t* tx_buf, uint32_t nbytes)
{
    /* Write contents of buffer to device */
    memcpy(hcereal->tx_buf, tx_buf, nbytes);
}

void cereal_receive(cereal_handle_t hcereal, uint8_t* rx_buf, uint32_t nbytes)
{
    /* Read device data into buffer */
    memcpy(rx_buf, hcereal->rx_buf, nbytes);
}

void cereal_register_callback(cereal_handle_t hcereal, cereal_callback cb)
{
    /* Set handler's callback function */
    hcereal->cb = cb;
}
