//[*]======================================================================================[*]
#include <atmel_start.h>
#include <tgmath.h>
//[*]======================================================================================[*]
#define R_TH 10000.0f
#define B_TH 3435.0f
#define T_0	 25.0f
//[*]======================================================================================[*]
uint16_t adc_result;
//[*]======================================================================================[*]
float steinhart_hart_equation(float r_th)
{
	int		t1, t2, t3;
	float	r1, r2, r3;
	float	a1, a2, a3;
	float	t1k, t2k, t3k;
	float	c1, c2, c3;
	float	c1a, c2a, c3a;
	float x, y, z, w, v, u;
	
	float temp_k;
	float temp;
	
	t1 = -40;		t2 = 25;		t3 = 85;
	r1 = 204.47;	r2 = 10.1;		r3 = 1.667;
	
	t1k = t1 + 273.15;	t2k = t2 + 273.15;	t3k = t3 + 273.15;
	
	a1 = log(r1);		a2 = log(r2);		a3 = log(r3);
	
	z = a1-a2;
	y = a1-a3;
	
	x = 1/t1k - 1/t2k;
	w = 1/t1k - 1/t3k;
	
	v = pow(a1,3) - pow(a2,3);
	
	u = pow(a1,3) - pow(a3,3);
	
	c3a = (x - z * w / y) / (v - z * u / y);
	c2a = (x - c3a * v) / z;
	c1a = 1 / t1k - c3a * pow(a1,3) - c2a * a1;
	
	c3 = c3a * 10000000;
	c2 = c2a * 10000;
	c1 = c1a * 1000;
	
//	1/(c1a+c2a*log(r_th)+c3a*pow(log(r_th),3));	
	
	temp_k = 1 / (c1a + c2a * log(r_th) + c3a * pow(log(r_th),3));
	temp   = temp_k - 273.15;
	
	printf("c1 : %f, c2 : %f, c3 : %f\r\n", c1, c2, c3);
//	printf("temp_k : %f, temp : %f\r\n", temp_k, temp);
	
	return temp;
}
//[*]======================================================================================[*]
float thermistor_temperature(uint16_t adc_value)
{
	float voltage = adc_value * (5.0f / 4095.0f);
	float resistance = (voltage*(5100/(5-voltage))) / 1000.;
//	float temperature = 1.0f / (B_TH / T_0 + log(resistance / R_TH) / log(2.7182818f));
	
	float temperature = steinhart_hart_equation(resistance);
	
	printf("adc data : %d, vtg : %.3f, rth : %.3f, temp : %f \r\n", adc_value, voltage, resistance, temperature);
	
	return temperature;
}
//[*]======================================================================================[*]
double fLinearEq(double x1, double y1, double x2, double y2, double x)
{
	double y;

	if(x2 == x1)	y = y1;
	else			y = (((y2 - y1) / (x2 - x1)) * x) + (((x2 * y1) - (x1 * y2)) / (x2 - x1));

	return y;
}
//[*]======================================================================================[*]
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

		adc_data = adc_sum / 10;
		
		thermistor_temperature(adc_data);
		
//		printf("adc data : %d, vtg : %.3f, rth : %.3f \r\n", adc_data, vtg, rth);

	}
}
