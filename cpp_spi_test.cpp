#include <iostream>
#include <fstream>
#include <cstring>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define SPI_PATH "/dev/spidev0.0"

using namespace std; 

class SPI {
public:
    SPI(const string& device, uint8_t mode, uint32_t speed) {
        // Open the SPI device
        spi_fd = open(device.c_str(), O_RDWR);
        if (spi_fd < 0) {
            cerr << "Failed to open SPI device" << endl;
            exit(1);
        }

        // Set SPI mode
        if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
            cerr << "Failed to set SPI mode" << endl;
            close(spi_fd);
            exit(1);
        }

        // Set SPI speed
        if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
            cerr << "Failed to set SPI speed" << endl;
            close(spi_fd);
            exit(1);
        }
    }

    ~SPI() {
        close(spi_fd);
    }

    void transfer(uint8_t* tx_data, uint8_t* rx_data, size_t length) {
        struct spi_ioc_transfer spi_transfer;
        memset(&spi_transfer, 0, sizeof(spi_transfer));

        spi_transfer.tx_buf = (unsigned long)tx_data;
        spi_transfer.rx_buf = (unsigned long)rx_data;
        spi_transfer.len = length;

        // Perform SPI transaction
        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0) {
            cerr << "Failed to perform SPI transfer" << endl;
        }
    }

private:
    int spi_fd;
};

int main() {
    // SPI settings
    uint8_t spi_mode = SPI_MODE_0;
    uint32_t spi_speed = 500000;  // 500kHz

    // SPI device object
    SPI spi(SPI_PATH, spi_mode, spi_speed);

    // Data to send (TX)
    uint8_t tx_data[3] = {0xAA, 0xBB, 0xCC};
    // Buffer to store received data (RX)
    uint8_t rx_data[3] = {0};

    // Perform SPI transfer
    spi.transfer(tx_data, rx_data, sizeof(tx_data));

    // Print received data
    cout << "Received: ";
    for (int i = 0; i < 3; ++i) {
        cout << hex << static_cast<int>(rx_data[i]) << " ";
    }
    cout << dec << endl;

    return 0;
}

