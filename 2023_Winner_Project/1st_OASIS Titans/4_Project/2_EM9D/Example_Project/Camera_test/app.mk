##
# application source dirs
##
APP_TYPE = helloworld
ALGO_TYPE = TFLITE_MICRO_GOOGLE_PERSON

override ALGO_TYPE := $(strip $(ALGO_TYPE))
override LINKER_SCRIPT_FILE := $(strip $(LINKER_SCRIPT_FILE))
override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

APPL_CSRC_DIR = ./src
APPL_CCSRC_DIR = ./src
APPL_CXXSRC_DIR = ./src
APPL_ASMSRC_DIR = ./src
APPL_INC_DIR = ./inc 

#for tlfm start
LIB_SEL += tflitemicro_24
LIB_SEL += sensordp pwrmgmt
LIB_SEL += event
LIB_SEL += i2c_comm ota

EVENTHANDLER_SUPPORT = event_handler
EVENTHANDLER_SUPPORT_LIST += evt_i2ccomm evt_datapath evt_peripheral_cmd
APPL_DEFINES += -DI2C_COMM
APPL_DEFINES += -DEVT_I2CCOMM
APPL_DEFINES += -DEVT_DATAPATH
APPL_DEFINES += -DEVENT_HANDLER_LIB
#for tlfm end

include $(EMBARC_ROOT)/app/scenario_app/scenario_app.mk
