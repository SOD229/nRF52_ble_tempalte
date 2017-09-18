PROJECT_NAME     = ble_app_template
DEVICE			 = NRF52832
MODEL			 = XXAA
VERSION			 = 0.0.1

BLUESTACK_PATH	 = ./BS
NORDIC_SDK_PATH  = /opt/nRF5_SDK_14.0.0
PROJECT_PATH	 = .


SOURCE_FILES += $(PROJECT_PATH)/main.c

INCLUDE_FOLDERS += $(PROJECT_PATH)/include

include $(BLUESTACK_PATH)/Makefile.bs
