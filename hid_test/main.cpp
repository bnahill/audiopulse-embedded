#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdint.h>

#define VERSION "0.1.0"
static constexpr uint16_t VENDOR_ID = 0x15A2;
static constexpr uint16_t PRODUCT_ID = 0xBEEF;
static constexpr uint16_t INTERFACE = 0;

static constexpr uint16_t PACKET_CTRL_LEN = 64;
static constexpr uint16_t PACKET_INT_LEN = 64;
static constexpr uint8_t EP_IN  = 0x81;
static constexpr uint8_t EP_OUT = 0x01;

static constexpr uint16_t TIMEOUT = 1000;



// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

static constexpr uint32_t CTRL_IN = LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE;
static constexpr uint32_t CTRL_OUT = LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE;

static struct libusb_device_handle *devh = NULL;



static int find_lvr_hidusb(void) {
	devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
	return devh ? 0 : -EIO;
}

static int ben_test(){
	int r,i;
	uint8_t answer[PACKET_CTRL_LEN];
	uint8_t question[PACKET_CTRL_LEN];
	int transferred;
	
	for (i=0;i<PACKET_CTRL_LEN; i++) question[i]=0x20+i;
	
	r = libusb_control_transfer(
			devh,
			CTRL_OUT ,
			HID_SET_REPORT,
			(HID_REPORT_TYPE_FEATURE<<8)|0x00,
			INTERFACE,
			question,
			63,
			TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Control SET_REPORT failed %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Control SET_REPORT succeeded %d\n", r);
	}
	
	r = libusb_control_transfer(devh,CTRL_IN,HID_GET_REPORT,(HID_REPORT_TYPE_INPUT<<8)|0x00,0, answer,PACKET_CTRL_LEN, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Control GET_REPORT error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Control GET_REPORT success! %d\n", r);
		printf("\n");
		for(auto &a : answer)
			printf("0x%02X\n", a);
	}
	
	r = libusb_interrupt_transfer(
				devh,
				EP_IN,
				answer,
				64,
				&transferred,
				TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Interrupt GET_REPORT error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Interrupt GET_REPORT success! %d\n", r);
	}
}

static int test_control_transfer(void)
{
	int r,i;
	uint8_t answer[PACKET_CTRL_LEN];
	uint8_t question[PACKET_CTRL_LEN];
	for (i=0;i<PACKET_CTRL_LEN; i++) question[i]=0x20+i;

	r = libusb_control_transfer(devh,CTRL_IN,HID_GET_REPORT,(HID_REPORT_TYPE_INPUT<<8)|0x00,0, answer,PACKET_CTRL_LEN, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Control IN error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Control IN success! %d\n", r);
	}
	
	r = libusb_control_transfer(devh, CTRL_OUT, HID_SET_REPORT, (HID_REPORT_TYPE_OUTPUT<<8)|0x00, 0, question, PACKET_CTRL_LEN,TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Control Out error %d\n", r);
		return r;
	} else {
		
	}
	
	for(i = 0;i < PACKET_CTRL_LEN; i++) {
		if(i%8 == 0)
			printf("\n");
		printf("%02x, %02x; ",question[i],answer[i]);
	}
	printf("\n");

	return 0;
}

static int test_interrupt_transfer(void)
{
	int r,i;
	int transferred;
	uint8_t answer[PACKET_INT_LEN];
	uint8_t question[PACKET_INT_LEN];
	for (i=0;i<PACKET_INT_LEN; i++) question[i]=0x40+i;

	r = libusb_interrupt_transfer(devh, EP_IN, answer,PACKET_INT_LEN,
								  &transferred, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Interrupt read error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Successfully read %d\n", r);
	}
	
	if (transferred < PACKET_INT_LEN) {
		fprintf(stderr, "Interrupt transfer short read (%d)\n", r);
		return -1;
	}

	r = libusb_interrupt_transfer(devh, EP_OUT, question, PACKET_INT_LEN,
								  &transferred, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Interrupt write error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Interrupt write succeeded %d\n", r);
	}

	// Do it again!
	r = libusb_interrupt_transfer(devh, EP_OUT, question, PACKET_INT_LEN,
								  &transferred, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Interrupt write error %d\n", r);
		return r;
	} else {
		fprintf(stderr, "Interrupt write succeeded %d\n", r);
	}

	r = libusb_interrupt_transfer(devh, EP_IN, answer,PACKET_INT_LEN,
								  &transferred, TIMEOUT);
	if (r < 0) {
		fprintf(stderr, "Interrupt read error %d\n", r);
		return r;
	}
	if (transferred < PACKET_INT_LEN) {
		fprintf(stderr, "Interrupt transfer short read (%d)\n", r);
		return -1;
	}

	for(i = 0;i < PACKET_INT_LEN; i++) {
		if(i%8 == 0)
			printf("\n");
		printf("%02x, %02x; ",question[i],answer[i]);
	}
	printf("\n");

	return 0;
}

int main(int argc, char **argv) {
	int r = 1;

	r = libusb_init(NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to initialise libusb\n");
		exit(1);
	}

	r = find_lvr_hidusb();
	if (r < 0) {
		fprintf(stderr, "Could not find/open LVR Generic HID device\n");
		goto out;
	}
	printf("Successfully find the LVR Generic HID device\n");

	r = libusb_detach_kernel_driver(devh, 0);
	if((r < 0) && (r != LIBUSB_ERROR_NOT_FOUND)){
		fprintf(stderr, "libusb_detach_kernel_driver error %d\n", r);
		goto out;
	}

	r = libusb_set_configuration(devh, 1);
	if (r < 0) {
		fprintf(stderr, "libusb_set_configuration error %d\n", r);
		goto out;
	}
	printf("Successfully set usb configuration 1\n");
	r = libusb_claim_interface(devh, 0);
	if (r < 0) {
		fprintf(stderr, "libusb_claim_interface error %d\n", r);
		goto out;
	}
	printf("Successfully claimed interface\n");

	//printf("Testing control transfer using loop back test of feature report");
	//test_control_transfer();
	printf("Testing control transfer using loop back test of input/output report");
	ben_test();

	libusb_release_interface(devh, 0);
out:
	libusb_reset_device(devh);
	libusb_close(devh);
	libusb_exit(NULL);
	return r >= 0 ? r : -r;
}
