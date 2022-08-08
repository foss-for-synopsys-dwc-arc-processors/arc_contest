#include "data_processing.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "max86150.h"

void Filter(float * data, float * coeff, float * output){
	int i, j;
	float sum;
	for(j = 0; j < (12000 + extra); j++){
		sum = 0;
		for(i = 0; i < 40; i++){
			if((j - i) < 0){
				break;
			}
			sum += data[j-i] * coeff[i];
		}

		if(j >= extra){
			output[j - extra] = sum;
		}
		
	}
}

void DownSample(float * data, float * output){
    for(int i = 0; i < 6000; i++){
        output[i] =  *(data + i * 2);
    }
}

void Normalization(float * data){

	float max = fabs((*(data)));
	for(int i = 0; i < 6000; i++){
		if(max < fabs((*(data + i))))
			max = fabs((*(data + i)));
	}

    for(int i = 0; i < 6000; i++){
        data[i] = data[i] / max;
    }
}

