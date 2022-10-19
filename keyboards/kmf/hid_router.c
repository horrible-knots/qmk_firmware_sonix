#ifdef VIA_OPENRGB_HYBRID
#include <stdint.h>
#include "raw_hid.h"
#include "kmf.h"

extern void orgb_raw_hid_receive(uint8_t *data, uint8_t length);
extern void via_raw_hid_receive(uint8_t *data, uint8_t length);

void custom_hid_receive(uint8_t *data, uint8_t length);

void raw_hid_receive(uint8_t *data, uint8_t length) {
	switch (hybrid_hid_mode) {
		case _HID_ORGB:
			orgb_raw_hid_receive(data, length);
			break;
		case _HID_VIA:
			via_raw_hid_receive(data, length);
			break;
		case _HID_RAW:
			custom_hid_receive(data, length);
			break;
		default:
			break;
	}
}

void custom_hid_receive(uint8_t *data, uint8_t length) {
	raw_hid_send(data, length);
	memset(data, 0, RAW_EPSIZE);
}

#endif
