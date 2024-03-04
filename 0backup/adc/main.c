#include <atmel_start.h>
double fLinearEq(double x1, double y1, double x2, double y2, double x)
{
	double y;

	if(x2 == x1)	y = y1;
	else			y = (((y2 - y1) / (x2 - x1)) * x) + (((x2 * y1) - (x1 * y2)) / (x2 - x1));

	return y;
}
int main(void)
{
	uint8_t  buffer[2];
	uint32_t adc_data = 0;
	uint32_t adc_sum  = 0;
	float	 vtg = 0.;
	float	 rth = 0.;
	float	 rth_old = 0.;
	float	 error = 0.;
	uint32_t temp_cnt = 0;
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	adc_sync_enable_channel(&ADC_0, 0);

	/* Replace with your application code */
	while (1) {
		
		adc_sum = 0;
		temp_cnt = 0;
		
		while(1)	{
			if(temp_cnt > 60000)
				break;
			else
				temp_cnt++;
		}
		
		for(int i=0; i<10; i++)	{
			// Board Thermistor
			adc_sync_read_channel(&ADC_0, 0, buffer, 2);
			adc_data = buffer[0] + (buffer[1] << 8);
			adc_sum = adc_sum + adc_data;
		}

//		adc_data = adc_sum / 10;

		vtg = fLinearEq(0., 0., 4095., 5.0, (double)adc_data);
		// Rb = ((Va * Rb) - (Va * Ra)) / Vin
//		rth = (5100.*(5.- vtg)) / vtg;
//		rth = (5100.*5) / vtg - 5100.;
		
		do 
		{
			rth = ((vtg * 5100.) - (vtg * rth_old)) / 5.0;
			error = rth - rth_old;
			
			rth_old = rth;
		} while (error > 0.00001);
		
		printf("adc data : %d, vtg : %.3f, rth : %.3f \r\n", adc_data, vtg, rth);

	}
}
