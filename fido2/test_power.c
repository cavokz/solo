// Copyright 2019 SoloKeys Developers
//
// Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
// http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
// http://opensource.org/licenses/MIT>, at your option. This file may not be
// copied, modified, or distributed except according to those terms.
#include APP_CONFIG
#ifdef TEST_POWER

/*
 *  Standalone test that runs through CTAP makeCredential and getAssertion
 *  for each button press.  Used for testing power consumption.
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cbor.h"
#include "device.h"
#include "ctaphid.h"
#include "util.h"
#include "log.h"
#include "ctap.h"

//#define BUTT NRF_GPIO_PIN_MAP(0,11)
//#define TRIG NRF_GPIO_PIN_MAP(0,30)
//
//#define GPIO_CLEAR(PIN)			nrf_gpio_pin_clear(PIN)
//#define GPIO_SET(PIN)			nrf_gpio_pin_set(PIN)
//#define GPIO_OUTPUT(PIN)		nrf_gpio_cfg_output(PIN)
//#define GPIO_INPUT(PIN)	nrf_gpio_cfg_input(PIN,NRF_GPIO_PIN_PULLUP)
//#define GPIO_READ(PIN)			nrf_gpio_pin_read(PIN)

#include "em_gpio.h"
#define BUTT		gpioPortF,6
#define TRIG		gpioPortD,14

#define GPIO_CLEAR(PIN)			GPIO_PinOutClear(PIN)
#define GPIO_SET(PIN)			GPIO_PinOutSet(PIN)
#define GPIO_OUTPUT(PIN)		GPIO_PinModeSet(PIN,gpioModePushPull,1)
#define GPIO_INPUT(PIN)			GPIO_PinModeSet(PIN,gpioModeInputPull,1)
#define GPIO_READ(PIN)			GPIO_PinInGet(PIN)


int main(int argc, char * argv[])
{
    int count = 0;
    uint64_t t1 = 0;
    uint64_t t2 = 0;
    uint64_t accum = 0;
    uint8_t hidmsg[64];

    set_logging_mask(
            0
            /*TAG_GEN|*/
            /*TAG_MC |*/
            /*TAG_GA |*/
            /*TAG_CP |*/
            /*TAG_CTAP|*/
            /*TAG_HID|*/
            /*TAG_U2F|*/
            /*TAG_PARSE |*/
            /*TAG_TIME|*/
            /*TAG_DUMP|*/
            /*TAG_GREEN|*/
            /*TAG_RED|*/
            |TAG_ERR
            );

    device_init();
    ctaphid_init();
    ctap_init();

    GPIO_INPUT(BUTT);
    GPIO_OUTPUT(TRIG);
    GPIO_CLEAR(TRIG);

    memset(hidmsg,0,sizeof(hidmsg));

    printf1(TAG_GEN,"recv'ing hid msg \n");


    while(1)
    {
        if (millis() - t1 > 100)
        {
            /*printf("heartbeat %ld\n", beat++);*/
            heartbeat();
            t1 = millis();
        }

        if (usbhid_recv(hidmsg) > 0)
        {
            printf1(TAG_DUMP,"%d>> ",count++); dump_hex1(TAG_DUMP, hidmsg,sizeof(hidmsg));
            t2 = millis();
            ctaphid_handle_packet(hidmsg);
            accum += millis() - t2;
            printf1(TAG_TIME,"accum: %lu\n", (uint32_t)accum);
            memset(hidmsg, 0, sizeof(hidmsg));
        }
        else
        {
            /*main_loop_delay();*/
        }
        ctaphid_check_timeouts();
    }

    // Should never get here
    usbhid_close();
    printf1(TAG_GREEN, "done\n");
    return 0;
}



void ctaphid_write_block(uint8_t * data)
{
    // Don't actually use usb
    /*usbhid_send(data);*/
}

uint8_t hidcmds[][64] = {"\x03\x00\x00\x00\x86\x00\x08\x2d\x73\x95\x80\x2e\xbb\x44\x8d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
"\x03\x00\x00\x00\x90\x00\x01\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
"\x03\x00\x00\x00\x90\x00\x75\x01\xa5\x01\x58\x20\xe5\x64\xed\x22\x01\xda\x3c\x2d\xab\x85\x3c\x1d\x04\x62\x67\x8a\x40\xaa\x1c\x3f\x31\x22\x21\x23\x35\xd5\xf8\xe4\x26\xc8\xd1\xa0\x02\xa2\x62\x69\x64\x6b\x65\x78\x61\x6d\x70\x6c\x6f\x2e\x6f\x72\x67\x64\x6e\x61",
"\x03\x00\x00\x00\x00\x6d\x65\x65\x45\x78\x61\x52\x50\x03\xa2\x62\x69\x64\x47\x75\x73\x65\x65\x5f\x6f\x64\x64\x6e\x61\x6d\x65\x67\x41\x42\x20\x55\x73\x65\x72\x04\x81\xa2\x63\x61\x6c\x67\x26\x64\x74\x79\x70\x65\x6a\x70\x75\x62\x6c\x69\x63\x2d\x6b\x65\x79\x05",
"\x03\x00\x00\x00\x01\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
"\x03\x00\x00\x00\x90\x00\x01\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
"\x03\x00\x00\x00\x90\x00\xf2\x02\xa3\x01\x6b\x65\x78\x61\x6d\x70\x6c\x6f\x2e\x6f\x72\x67\x02\x58\x20\xc4\x74\xef\xcd\xb3\xf9\x61\x18\xdd\xfb\x2f\xe5\x7b\x05\xd2\xf1\x23\xba\x20\x7c\x87\xea\x9e\xfd\x07\xa3\xe3\x21\xdc\x60\x9f\x63\x03\x81\xa2\x62\x69\x64\x58",
"\x03\x00\x00\x00\x00\xa8\x6a\x7e\x7b\xe7\x95\x63\xb1\x38\x2e\x2f\x03\xd6\x6c\xf5\xa3\xa9\x00\x00\x00\x00\x00\x00\x00\x00\x08\x24\x17\x4e\x23\xfd\x17\x7f\xb3\x18\x7c\x74\xe0\xbe\x35\x35\x94\x02\x7f\x61\xe3\x1c\xae\x73\xb4\x02\x7f\xf6\x7e\xc1\x36\x43\x2b\xc2",
"\x03\x00\x00\x00\x01\x60\xdd\xb8\x0b\x4a\xbb\x6f\x61\xac\xc3\xc6\x37\x9d\x33\x71\xe3\xcf\xa2\x4f\x5c\x11\x97\x44\xf8\x87\xb0\xd2\x67\xe8\x9a\xeb\x61\x39\x29\x61\xbf\xf7\xd1\xe1\xa9\x79\x26\x4a\x1a\xe7\x26\x60\xa4\x3d\x9f\x5c\xfe\x57\xbc\x4a\x74\x71\xf5\x67",
"\x03\x00\x00\x00\x02\xdd\x8d\x40\x49\x1e\x2d\x28\x15\x18\xbd\xf2\xe2\xef\x61\xbc\xb6\x04\x6f\x51\xdb\xe6\xc6\xd0\x9f\xbb\x06\xae\xac\xe4\xf6\xbd\x05\x85\xd3\x31\x5b\x98\x75\x2a\x4a\x31\xea\x5d\xc8\x78\x15\xee\xbe\x56\x0b\x43\x64\x74\x79\x70\x65\x6a\x70\x75",
"\x03\x00\x00\x00\x03\x62\x6c\x69\x63\x2d\x6b\x65\x79\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"};

int usbhid_recv(uint8_t * msg)
{
    static int lastval = 0;
    static int reading = 0;
    int val;
    // button1 == p0.11
    // button2 == p0.12
    // button3 == p0.24
    // button4 == p0.25
    if (!reading)
    {
        delay(1);
        GPIO_CLEAR(TRIG);
        val = GPIO_READ(BUTT);
        if (val == 0)
        {
            if (lastval != 0)
            {
                printf1(TAG_GEN, "button!\n");
                /*printf1(TAG_GEN,"size of array: %d elements", sizeof(hidcmds)/64);*/
                reading = 1;
            }
        }
        lastval = val;
    }
    else
    {
    	GPIO_SET(TRIG);
        memmove(msg, hidcmds[reading-1], 64);
        reading++;
        if (reading-1 == sizeof(hidcmds)/64)
        {
            reading = 0;
        }
        return 64;
    }
    return 0;
}

#endif
