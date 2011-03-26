// ===========================================
// IMU INTERFACE CODE
// Bob Somers, 2010
// Cal Poly Supermileage Vehicle Team
// California Polytechnic State University
// San Luis Obispo, CA
//
// This code is designed to run on an
// ATmega168 microcontroller with our custom
// mainboard. The microcontroller should be
// clocked at 7.3728 MHz.
//
// This handles the interfacing with the analog
// inertial measurement unit (IMU) and taking,
// 10-bit ADC measurements every 1/10th of a
// second. These measurements are transmitted
//  to the host PC over the ATmega168's UART at
// 115200 baud. Status commands can also be sent
// from the host PC to this chip which allows
// control of 8 status LEDs on the mainboard.
// ===========================================

#include <stdint.h>
#include <avr/io.h>

#define IMU_COMM 0
#define XBEE_COMM 1
#define GPS_LOCK 2
#define HEAD_LTS 3
#define BRAKE_LTS 4
#define LTURN_LTS 5
#define RTURN_LTS 6
#define STARTER 7
#define HORN 8

// WARNING: several raw register settings make the assumption this
// system is running on a system clock of 7.3728 MHz!

// ===========================================
// ADC FUNCTIONS
// ===========================================

void adc_init()
{
	// disable internal pull-ups, since they interfere
	// with the adc
	PORTC = 0x00;
	DDRC = 0x00;

	// take the reference voltage from AVcc, and initially set the mux to sample gnd
	ADMUX = (1 << REFS0) | 0x0f;

	// enable the adc, use a 64 as the clock divider (7.3728 MHz / 64 gives us a
	// 115200 KHz clock, and the adc needs between 50 KHz and 200 KHz), and start a
	// conversion to "prime" the system (the first conversion takes significantly
	// longer than subsequent conversions)
	ADCSRA = (1 << ADEN) | (1 << ADSC) | 0x06;

	// wait for the first (dummy) conversion to complete
	while (ADCSRA & (1 << ADSC));
}

uint16_t adc_sample(uint8_t channel)
{
	uint8_t low, high, mux;

	// set the mux to the selected channel, while preserving the upper 4 bits
	mux = (ADMUX & 0xf0) | (channel & 0x0f);
	ADMUX = mux;

	// start the conversion and wait for it to complete
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));

	// read out the value, LOW BYTE FIRST, so that the data isn't cleared before
	// we finish reading it
	low = ADCL;
	high = ADCH;

	// return the 10-bit sample
	return ((uint16_t)high << 8) | low;
}

// ===========================================
// UART FUNCTIONS
// ===========================================

void uart_init()
{
	// based on 7.3728 MHz clock and a 115200 bps baud rate
	UBRR0H = 0;
	UBRR0L = 3;
	
	// set the frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C = (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

	// enable both the receiver and the transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void uart_send(uint8_t byte)
{
	// wait until the uart is ready
	while ((UCSR0A & (1 << UDRE0)) == 0);
	
	// fire off the data byte
	UDR0 = byte;
}

// asynchronous, returns true/false immediately if data contains a valid byte
uint8_t uart_receive(uint8_t *data)
{
	if (!(UCSR0A & (1 << RXC0)))
	{
		// nothing ready
		return 0;
	}

	// set the data byte
	*data = UDR0;

	// success
	return 1;
}

void uart_print(char *string, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		uart_send(string[i]);
	}
}

// ===========================================
// LED FUNCTIONS
// ===========================================

void leds_init()
{
	// put B2, B1, and B0 in output mode
	DDRB |= 0x7;

	// put D7, D6, D5, D4, D3, D2 in output mode
	DDRD |= (0x3f << 2);
}

void led_set(uint8_t which, uint8_t status)
{
	switch (which)
	{
		case IMU_COMM:
			if (status)
			{
				PORTB |= (1 << 2);
			}
			else
			{
				PORTB &= ~(1 << 2);
			}
			break;

		case XBEE_COMM:
			if (status)
			{
				PORTB |= (1 << 1);
			}
			else
			{
				PORTB &= ~(1 << 1);
			}
			break;

		case GPS_LOCK:
			if (status)
			{
				PORTB |= (1 << 0);
			}
			else
			{
				PORTB &= ~(1 << 0);
			}
			break;
		
		case HEAD_LTS:
			if (status)
			{
				PORTD |= (1 << 7);
			}
			else
			{
				PORTD &= ~(1 << 7);
			}
			break;
			
		case BRAKE_LTS:
			if (status)
			{
				PORTD |= (1 << 6);
			}
			else
			{
				PORTD &= ~(1 << 6);
			}
			break;
		
		case LTURN_LTS:
			if (status)
			{
				PORTD |= (1 << 5);
			}
			else
			{
				PORTD &= ~(1 << 5);
			}
			break;

		case RTURN_LTS:
			if (status)
			{
				PORTD |= (1 << 4);
			}
			else
			{
				PORTD &= ~(1 << 4);
			}
			break;

		case STARTER:
			if (status)
			{
				PORTD |= (1 << 3);
			}
			else
			{
				PORTD &= ~(1 << 3);
			}
			break;

		case HORN:
			if (status)
			{
				PORTD |= (1 << 2);
			}
			else
			{
				PORTD &= ~(1 << 2);
			}
			break;

		default:
			break;
	}
}

// ===========================================
// TIMER FUNCTIONS
// ===========================================

void timer_init()
{
	// set the prescaler on the 16-bit timer to 64
	TCCR1B |= (1 << CS10) | (1 << CS11);
}

uint16_t timer_get()
{
	// return the current count
	return TCNT1;
}

void timer_reset()
{
	// reset the count register
	TCNT1 = 0;
}

// ===========================================
// ENTRY POINT
// ===========================================

int main()
{
	uint16_t adc_value[6];
	uint8_t digit[6][4];
	uint8_t i;
	uint8_t leds[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t imu_comm_ctr = 0;
	uint8_t uart_buffer[3] = {0, 0, 0};
	uint8_t byte;

	// get the subsystems up and running
	uart_init();
	adc_init();
	leds_init();
	timer_init();

	timer_reset();
	while (1)
	{
		// is it time to do a sample (has 1/10th of a second passed?)
		if (timer_get() >= 11520)
		{
			// yes! do the adc sample and send it off down the uart
			
			// sample all 6 channels		
			for (i = 0; i < 6; i++)
			{
				adc_value[i] = adc_sample(i);
			}

			// convert to digits
			for (i = 0; i < 6; i++)
			{
				digit[i][3] = adc_value[i] / 1000;
				adc_value[i] = adc_value[i] % 1000;
				digit[i][2] = adc_value[i] / 100;
				adc_value[i] = adc_value[i] % 100;
				digit[i][1] = adc_value[i] / 10;
				adc_value[i] = adc_value[i] % 10;
				digit[i][0] = adc_value[i];
			}
	
			// output to the uart
			uart_send('$'); // start of line character
			for (i = 0; i < 6; i++)
			{
				uart_send(digit[i][3] + 48);
				uart_send(digit[i][2] + 48);
				uart_send(digit[i][1] + 48);
				uart_send(digit[i][0] + 48);
				uart_send('\t');
			}
			uart_send('\r');
			uart_send('\n');
			
			// increment the imu comm counter
			imu_comm_ctr++;

			// restart the timer
			timer_reset();
		}

		// update imu comm led status
		if (imu_comm_ctr > 0)
		{
			leds[IMU_COMM] = !leds[IMU_COMM];
			led_set(IMU_COMM, leds[IMU_COMM]);
			imu_comm_ctr = 0;
		}

		// try to get a byte from the uart
		if (uart_receive(&byte))
		{
			// shift the buffer
			uart_buffer[0] = uart_buffer[1];
			uart_buffer[1] = uart_buffer[2];
			uart_buffer[2] = byte;

			// middle byte MUST be a '=' for any of these to be true
			if (uart_buffer[1] == '=')
			{
				switch (uart_buffer[0])
				{
					case 'x':
						led_set(XBEE_COMM, uart_buffer[2] - 48);
						break;

					case 'g':
						led_set(GPS_LOCK, uart_buffer[2] - 48);
						break;

					case 'h':
						led_set(HEAD_LTS, uart_buffer[2] - 48);
						break;

					case 'b':
						led_set(BRAKE_LTS, uart_buffer[2] - 48);
						break;

					case 'l':
						led_set(LTURN_LTS, uart_buffer[2] - 48);
						break;

					case 'r':
						led_set(RTURN_LTS, uart_buffer[2] - 48);
						break;

					case 's':
						led_set(STARTER, uart_buffer[2] - 48);
						break;

					case 'n':
						led_set(HORN, uart_buffer[2] - 48);
						break;

					default:
						break;
				}
			}
		}
	}

	return 0;
}
