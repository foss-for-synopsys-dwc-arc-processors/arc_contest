#ifndef _I2C_DATA_PROCESSING_H_
#define _I2C_DATA_PROCESSING_H_





void Filter(float * data, float * coeff, float * output);
void DownSample(float * data, float * output);
void Normalization(float * data);


#endif /* SCENARIO_APP_SAMPLE_CODE_PERIODICAL_WAKEUP_QUICKBOOT_TFLITEMICRO_ALGO_H_ */


