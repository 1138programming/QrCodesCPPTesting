#ifndef USBCOMMS_HPP
#define USBCOMMS_HPP

#define ENABLE_DEBUG_LOGGING
#include "../../include/libusb.h"
#include "../debugConsole.hpp"

// should only be initialized once per context (basically only once ever)
class USBComms {
        /*********************************************/
        /* INITIALIZING / FREEING FUNCTIONS */
        /*********************************************/
        static int hotplugCallback(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *userData) {
            USBComms* self = (USBComms*)userData;

            self->handleTabDevices(device);

            return 0; // if we return anything other than 0, we are de-registered
        }

        void handleTabDevices(libusb_device* device) {
            libusb_device_descriptor deviceDesc;
            libusb_get_device_descriptor(device, &deviceDesc);

            // https://devicehunt.com/search/type/usb/vendor/04E8/device/any (Samsung)
            if (deviceDesc.idVendor == 0x4E8) {
                DebugConsole::println("USB: Found Samsung device", DBGC_DEFAULT, DBGL_DEVEL);
                
                libusb_device_handle* androidDevice;
                libusb_open(device, &androidDevice);
                
                int errorCode = libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_OUT | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 53, 0, 0, NULL, 0, 5000);
                if (errorCode < 0) {
                    DebugConsole::println(std::format("Samsung Device Doesn't support AOA(?). Error type: {}", libusb_strerror(errorCode)), DBGC_YELLOW, DBGL_WARNING);
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
                libusb_open(device, &androidAOADevice);

                libusb_bulk_transfer(androidAOADevice, 0, (unsigned char*)"HI!", 4, NULL, 5000);

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
            libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 0, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, USBComms::hotplugCallback, this, NULL);

            scanDevices();
        }

        void cleanupUSBComms() {
            libusb_exit(NULL);
        }
};

#endif
