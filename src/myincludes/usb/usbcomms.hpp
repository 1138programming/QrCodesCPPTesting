#ifndef USBCOMMS_HPP
#define USBCOMMS_HPP

#define ENABLE_DEBUG_LOGGING
#define LIBUSB_DEBUG LIBUSB_LOG_LEVEL_DEBUG
#include "../../include/libusb.h"
#include "../debugConsole.hpp"

// should only be initialized once per context (basically only once ever)
class USBComms {
        private:
            std::string dataToSend;
        public:
            USBComms(std::string dataToSend) {
                this->dataToSend = dataToSend;
            }

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
                    int errorCode = libusb_open(device, &androidDevice);
                    if (errorCode != 0) {
                        DebugConsole::println(std::format("Device not connected; error: {}", libusb_strerror(errorCode)), DBGC_RED, DBGL_ERROR);
                        return;
                    }
                    
                    libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_IN | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 52, 0, 0, (unsigned char*)"team1138", strlen("team1138")+1, 5000);
                    libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_IN | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 52, 0, 1, (unsigned char*)"tab", strlen("tab")+1, 5000);
                    libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_IN | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 52, 0, 3, (unsigned char*)"1.0", strlen("1.0")+1, 5000);

                    uint16_t aoaVerNum = 0;
                    errorCode = libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_IN | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 51, 0, 0, (unsigned char*)&aoaVerNum, sizeof(aoaVerNum), 5000);
                    if (errorCode < 0) {
                        DebugConsole::println(std::format("Samsung Device Doesn't support AOA(?). Error type: {}", libusb_strerror(errorCode)), DBGC_YELLOW, DBGL_WARNING);
                    }
                    else {
                        DebugConsole::println(std::format("Samsung device supports AOA ver: {}. Bytes transferred: {}", aoaVerNum, errorCode), DBGC_DEFAULT, DBGL_DEVEL);
                    }
                    
                    errorCode = libusb_control_transfer(androidDevice, (uint8_t)LIBUSB_ENDPOINT_OUT | (uint8_t)LIBUSB_REQUEST_TYPE_VENDOR, 53, 0, 0, NULL, 0, 5000);
                    if (errorCode < 0) {
                        DebugConsole::println(std::format("Samsung Device Doesn't support AOA(?). Error type: {}", libusb_strerror(errorCode)), DBGC_YELLOW, DBGL_WARNING);
                    }
                    else {
                        DebugConsole::println(std::format("Samsung device put into AOA mode. Bytes Transferred: {}", errorCode), DBGC_DEFAULT, DBGL_DEVEL);
                    }
                    libusb_close(androidDevice);
                }

                // AOA codes (https://source.android.com/docs/core/interaction/accessories/aoa2)
                else if (deviceDesc.idProduct >= 0x2D00 && deviceDesc.idProduct <= 0x2D05) {
                    DebugConsole::println("USB: Found AOA device", DBGC_GREEN, DBGL_DEVEL);
                    libusb_device_handle* androidAOADevice;
                    libusb_open(device, &androidAOADevice);

                    int errorCode = libusb_claim_interface(androidAOADevice, 0);
                    if (errorCode != 0) {
                        DebugConsole::println(std::format("Failed to claim device's interface; Error: {}", errorCode));
                        libusb_close(androidAOADevice);
                        return;
                    }
                    
                    libusb_config_descriptor* config;
                    libusb_get_active_config_descriptor(device, &config);
                    
                    std::string message("reboot");
                    errorCode = libusb_bulk_transfer(androidAOADevice, config->interface[1].altsetting->endpoint[0].bEndpointAddress, (unsigned char*)message.c_str(), message.length(), NULL, 0);
                    if (errorCode != 0) {
                        DebugConsole::println(std::format("Failed to transfer data; Error: {}", errorCode));
                    }


                    libusb_release_interface(androidAOADevice, 0);
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
                int error = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 0, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, USBComms::hotplugCallback, this, NULL);
                if (error != LIBUSB_SUCCESS) {
                    DebugConsole::println(std::format("Failed to register USB hotplug callback; Error: {}", libusb_strerror(error)), DBGC_RED, DBGL_ERROR);
                }

                scanDevices();
            }

            void cleanupUSBComms() {
                libusb_exit(NULL);
            }
};

#endif
