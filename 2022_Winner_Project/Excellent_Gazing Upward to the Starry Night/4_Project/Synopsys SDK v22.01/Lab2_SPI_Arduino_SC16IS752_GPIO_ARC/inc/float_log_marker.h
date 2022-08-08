#ifndef _FLOAT_LOG_MARKER_H_
#define _FLOAT_LOG_MARKER_H_

#define NRF_LOG_FLOAT_MARKER "%s%d.%02d"
#define NRF_LOG_FLOAT(val) (uint32_t)(((val) < 0 && (val) > -1.0) ? "-" : ""), \
						   (int32_t)(val),                                     \
						   (int32_t)((((val) > 0) ? (val) - (int32_t)(val)     \
												  : (int32_t)(val) - (val)) *  \
									 100)

#endif
