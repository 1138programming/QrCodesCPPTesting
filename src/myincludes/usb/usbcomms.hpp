#ifndef USBCOMMS_HPP
#define USBCOMMS_HPP

#include "../libusb.h"
#include "../debugConsole.hpp"

class USBComms {
        /*********************************************/
        /* INITIALIZING / FREEING FUNCTIONS */
        /*********************************************/
        void initUSBComms() {
            libusb_init_context(NULL, NULL, 0);
            libusb_set_log_cb(NULL, DebugConsole::libUSBCallback, LIBUSB_LOG_CB_GLOBAL);
        }

        void cleanupUSBComms() {
            libusb_exit(NULL);
        }
};

#endif