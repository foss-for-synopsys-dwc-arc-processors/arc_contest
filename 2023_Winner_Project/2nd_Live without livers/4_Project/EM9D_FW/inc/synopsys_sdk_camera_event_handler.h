#ifndef SYNOPSYS_SDK_CAMERA_EVENT_HANDLER_H_
#define SYNOPSYS_SDK_CAMERA_EVENT_HANDLER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"


#include "event_handler_evt.h"

#include "synopsys_sdk_camera_drv.h"

void synopsys_camera_evt_handler(EVT_INDEX_E event); 


#endif /* SYNOPSYS_SDK_CAMERA_EVENT_HANDLER_H_ */