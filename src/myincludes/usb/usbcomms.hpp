#ifndef USBCOMMS_HPP
#define USBCOMMS_HPP

#include "../libusb.h"
#include "../debugConsole.hpp"

class USBComms {
        /*********************************************/
        /* INITIALIZING / FREEING FUNCTIONS */
        /*********************************************/
        void handleTabDevices(libusb_device* device) {
            libusb_device_descriptor deviceDesc;
            libusb_get_device_descriptor(device, &deviceDesc);
            deviceDesc.iSerialNumber

            // https://devicehunt.com/search/type/usb/vendor/04E8/device/any (Samsung)
            if (deviceDesc.idVendor == 0x4E8) {
                DebugConsole::println("USB: Found Samsung device", DBGC_DEFAULT, DBGL_DEVEL);
                
                libusb_device_handle* androidDevice;
                libusb_open(device, &androidDevice);
                
                int errorCode = libusb_control_transfer(androidDevice, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR, 53, 0, 0, NULL, 0, 5000);
                if (errorCode < 0) {
                    DebugConsole::println(std::format("Samsung Device Doesn't support AOA(?). Error type: {}", libusb_strerror(errorCode)), DBGC_YELLOW, DBGL_WARNING)
                }
                else {
                    DebugConsole::println("Samsung device put into AOA mode", DBGC_DEFAULT, DBGL_DEVEL);
                }
                libusb_close(androidDevice);
            }
            // AOA codes (https://source.android.com/docs/core/interaction/accessories/aoa2)
            else if (deviceDesc.idProduct >= 0x2D00 && deviceDesc.idProduct <= 0x2D05) {
                DebugConsole::println("USB: Found AOA device", DBGC_DEFAULT, DBGL_DEVEL);
                libusb_device_handle* androidAOADevice;
                libusb_open(androidAOADevice);

                libusb_bulk_transfer(androidAOADevice, 0, "HI!", 4, NULL, 5000);

                libusb_close(androidAOADevice);
            }
        }

        void scanDevices() {
            libusb_device** connectedDevs;
            ssize_t arrSize = libusb_get_device_list(NULL, &connectedDevs);

            for (int i = 0; i < arrSize; i++) {
                DebugConsole::println(std::string("USB DEVICE FOUND: ") + std::to_string(libusb_get_device_address(connectedDevs[i])), DBGC_DEFAULT, DBGL_DEVEL);
                
                libusb_device_descriptor desc;
                libusb_get_device_descriptor(connectedDevs[i], &desc);
                DebugConsole::println(std::format("\tUSB DEVICE VENDOR ID: {:#4x}; USB PRODUCT ID: {:#4x}", desc.idVendor, desc.idProduct), DBGC_DEFAULT, DBGL_DEVEL);

                handleTabDevices(connectedDevs[i]);
            }
        }

        void initUSBComms() {
            libusb_init_context(NULL, NULL, 0);
            libusb_set_log_cb(NULL, DebugConsole::libUSBCallback, LIBUSB_LOG_CB_GLOBAL);

            scanDevices();
        }

        void cleanupUSBComms() {
            libusb_exit(NULL);
        }
};

#endif