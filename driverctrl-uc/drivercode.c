// ===========================================
// DRIVER INTERFACE CODE
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
// This handles the driver interface details,
// such as getting button states from the
// steering wheel, updating the states of the
// lighting systems, starter, and horn, and
// communicating system status to the host PC
// over the ATmega168's UART at 115200 baud.
// ===========================================

// Modified by Bob Somers on 4/8/11 to fix
// the strange "ghost in the machine" bug where
// lights would turn on after certain events
// like running the horn or starter or flicking
// the driver kill switch.

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define HEAD_LTS 0
#define BRAKE_LTS 1
#define LTURN_LTS 2
#define RTURN_LTS 3
#define DRIVER_LEFT 4
#define DRIVER_RIGHT 5
#define STARTER 6
#define HORN 7

#define LEFT_TOP 0
#define LEFT_MID 1
#define LEFT_BOT 2
#define	LEFT_PAD 3
#define RIGHT_TOP 4
#define RIGHT_MID 5
#define RIGHT_BOT 6
#define RIGHT_PAD 7
#define BRAKE_SW 8

// global variable for locking out buttons during debounce period
volatile int buttons_locked_out = 0;

// WARNING: several raw register settings make the assumption this
// system is running on a system clock of 7.3728 MHz!

// ===========================================
// UART FUNCTIONS
// ===========================================

void uart_init() {
	// based on 7.3728 MHz clock and a 115200 bps baud rate
	UBRR0H = 0;
	UBRR0L = 3;
	
	// set the frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C = (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

	// enable both the receiver and the transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void uart_send(uint8_t byte) {
	// wait until the uart is ready
	while ((UCSR0A & (1 << UDRE0)) == 0);
	
	// fire off the data byte
	UDR0 = byte;
}

// asynchronous, returns true/false immediately if data contains a valid byte
uint8_t uart_receive(uint8_t *data) {
	if (!(UCSR0A & (1 << RXC0))) {
		// nothing ready
		return 0;
	}

	// set the data byte
	*data = UDR0;

	// success
	return 1;
}

void uart_print(char *string, int len) {
	int i;

	for (i = 0; i < len; i++) {
		uart_send(string[i]);
	}
}

// ===========================================
// PORT FUNCTIONS
// ===========================================

void ports_init() {
    // B7-5 input, B4-3 output, B2-0 input
	DDRB = (1 << DDB4) | (1 << DDB3);

    // C6-0 input
    DDRC = 0x0;

    // D7-2 output, D1-0 uart
    DDRD = (1 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4) | (1 << DDD3) | (1 << DDD2);
}

// ===========================================
// LED FUNCTIONS
// ===========================================

void led_set(uint8_t which, uint8_t status) {
    static uint8_t port_b = 0x0;
    static uint8_t port_d = 0x0;

    // figure out the bitmask for this led
    uint8_t mask = 0x0;
    switch (which) {
        case STARTER:
            mask = (1 << PB4);
            break;

        case HORN:
            mask = (1 << PB3);
            break;

        case HEAD_LTS:
            mask = (1 << PD7);
            break;

        case BRAKE_LTS:
            mask = (1 << PD6);
            break;
        
        case LTURN_LTS:
            mask = (1 << PD5);
            break;

        case RTURN_LTS:
            mask = (1 << PD4);
            break;

        case DRIVER_LEFT:
            mask = (1 << PD3);
            break;

        case DRIVER_RIGHT:
            mask = (1 << PD2);
            break;

        default:
            break;
    }

    // apply the mask to the current led state
    if (status) {
        if (which == STARTER || which == HORN) {
            port_b |= mask;
            PORTB = port_b;
        } else {
            port_d |= mask;
            PORTD = port_d;
        }
    } else {
        if (which == STARTER || which == HORN) {
            port_b &= ~mask;
            PORTB = port_b;
        } else {
            port_d &= ~mask;
            PORTD = port_d;
        }
    }
}

// ===========================================
// BUTTON FUNCTIONS
// ===========================================

uint8_t button_get(uint8_t which) {
    uint8_t mask = 0x0;
    switch (which) {
        case LEFT_TOP:
            mask = (1 << PINC5);
            break;

        case LEFT_MID:
            mask = (1 << PINC4);
            break;

        case LEFT_BOT:
            mask = (1 << PINC3);
            break;

        case LEFT_PAD:
            mask = (1 << PINB2);
            break;

        case RIGHT_TOP:
            mask = (1 << PINC2);
            break;

        case RIGHT_MID:
            mask = (1 << PINC1);
            break;

        case RIGHT_BOT:
            mask = (1 << PINC0);
            break;

        case RIGHT_PAD:
            mask = (1 << PINB1);
            break;

        case BRAKE_SW:
            mask = (1 << PINB0);
            break;

        default:
            break;
    }

    uint8_t value = 1;
    if (which == LEFT_PAD || which == RIGHT_PAD || which == BRAKE_SW) {
        value = PINB & mask;
    } else if (which == LEFT_TOP || which == LEFT_MID || which == LEFT_BOT ||
               which == RIGHT_TOP || which == RIGHT_MID || which == RIGHT_BOT) {
        value = PINC & mask;
    } else {
        value = 1;
    }
    value = !value;

    return value;
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

void debouncer_init()
{
	// set the prescaler on the 8-bit timer to 1024
	TCCR0B |= (1 << CS02) | (1 << CS00);
}

void debouncer_lockout()
{
	// don't lock out buttons if they're already locked out
	if (buttons_locked_out) return;
	
	buttons_locked_out = 1;
	
	// reset the timer
	TCNT0 = 0;

	// enable overflow interrupts
	TIMSK0 |= (1 << TOIE0);
}

void debouncer_clear()
{
	// don't bother if buttons are already enabled
	if (!buttons_locked_out) return;

	// disable overflow interrupts
	TIMSK0 &= ~(1 << TOIE0);

	buttons_locked_out = 0;
}

ISR(TIMER0_OVF_vect)
{
	static int count = 0;

	count++;
	if (count > 15)
	{
		count = 0;
		debouncer_clear();
	}
}

// ===========================================
// ENTRY POINT
// ===========================================

int main()
{
	uint8_t buttons[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t prev[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t leds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t brake_pwm = 0;
	uint8_t turn_pwm = 0;
	uint8_t turn_ctr = 0;
	uint8_t lts_state = 0;
	uint8_t rt_state = 0;
	uint8_t lf_state = 0;
	uint8_t hzd_state = 0;

	// get the subsystems up and running
    ports_init();
	uart_init();
	timer_init();
	debouncer_init();

	// enable interrupts globally
	sei();

	timer_reset();
	while (1)
	{
		// save previous button states
		prev[LEFT_TOP] = buttons[LEFT_TOP];
		prev[LEFT_MID] = buttons[LEFT_MID];
		prev[LEFT_BOT] = buttons[LEFT_BOT];
		prev[LEFT_PAD] = buttons[LEFT_PAD];
		prev[RIGHT_TOP] = buttons[RIGHT_TOP];
		prev[RIGHT_MID] = buttons[RIGHT_MID];
		prev[RIGHT_BOT] = buttons[RIGHT_BOT];
		prev[RIGHT_PAD] = buttons[RIGHT_PAD];
		prev[BRAKE_SW] = buttons[BRAKE_SW];

		// get input from the buttons
		buttons[LEFT_TOP] = button_get(LEFT_TOP);
		buttons[LEFT_MID] = button_get(LEFT_MID);
		buttons[LEFT_BOT] = button_get(LEFT_BOT);
		buttons[LEFT_PAD] = button_get(LEFT_PAD);
		buttons[RIGHT_TOP] = button_get(RIGHT_TOP);
		buttons[RIGHT_MID] = button_get(RIGHT_MID);
		buttons[RIGHT_BOT] = button_get(RIGHT_BOT);
		buttons[RIGHT_PAD] = button_get(RIGHT_PAD);
		buttons[BRAKE_SW] = button_get(BRAKE_SW);

		// generate pwm signals
		if (timer_get() < 100)
		{
			brake_pwm = 1;
		}
		else if (timer_get() > 1000)
		{
			turn_ctr++;
			timer_reset();
		}
		else
		{
			brake_pwm = 0;
		}
		if (turn_ctr > 50)
		{
			turn_pwm = !turn_pwm;
			turn_ctr = 0;
		}

		// pass through starter and horn directly
		leds[STARTER] = button_get(RIGHT_TOP);
		leds[HORN] = button_get(LEFT_TOP);

		// if the previous RIGHT_MID state was high and now it's low, falling edge!
		if (prev[RIGHT_MID] && !buttons[RIGHT_MID] && !buttons_locked_out)
		{
			lts_state = !lts_state; // toggle
			debouncer_lockout();
			uart_send('$'); uart_send('h'); uart_send('='); uart_send(lts_state + 48); uart_send('\r'); uart_send('\n');
		}

		// if the previous LEFT_MID state was high and now it's low, falling edge!
		if (prev[LEFT_MID] && !buttons[LEFT_MID] && !buttons_locked_out)
		{
			hzd_state = !hzd_state; // toggle
			debouncer_lockout();
			uart_send('$'); uart_send('z'); uart_send('='); uart_send(hzd_state + 48); uart_send('\r'); uart_send('\n');

			// start with light on
			turn_pwm = 1;
			turn_ctr = 0;
		}

		// if the previous LEFT_PAD state was high and now it's low, falling edge!
		if (prev[LEFT_PAD] && !buttons[LEFT_PAD] && !buttons_locked_out)
		{
			lf_state = !lf_state; // toggle
			debouncer_lockout();
			uart_send('$'); uart_send('l'); uart_send('='); uart_send(lf_state + 48); uart_send('\r'); uart_send('\n');

			// start with light on
			turn_pwm = 1;
			turn_ctr = 0;
		}

		// if the previous RIGHT_PAD state was high and now it's low, falling edge!
		if (prev[RIGHT_PAD] && !buttons[RIGHT_PAD] && !buttons_locked_out)
		{
			rt_state = !rt_state; // toggle
			debouncer_lockout();
			uart_send('$'); uart_send('r'); uart_send('='); uart_send(rt_state + 48); uart_send('\r'); uart_send('\n');

			// start with light on
			turn_pwm = 1;
			turn_ctr = 0;
		}

		// output uart on every edge change of the brake switch
		if (prev[BRAKE_SW] != buttons[BRAKE_SW])
		{
			// brake switch is not debounced
			uart_send('$'); uart_send('k'); uart_send('='); uart_send(buttons[BRAKE_SW] + 48); uart_send('\r'); uart_send('\n');
		}

		// output uart on every edge change of the starter
		if (prev[RIGHT_TOP] != buttons[RIGHT_TOP])
		{
			// starter is not debounced
			uart_send('$'); uart_send('s'); uart_send('='); uart_send(buttons[RIGHT_TOP] + 48); uart_send('\r'); uart_send('\n');
		}

		// output uart on every edge change of the horn
		if (prev[LEFT_TOP] != buttons[LEFT_TOP])
		{
			// brake switch is not debounced
			uart_send('$'); uart_send('n'); uart_send('='); uart_send(buttons[LEFT_TOP] + 48); uart_send('\r'); uart_send('\n');
		}

        // HACK! try locking out on the falling edge of the starter/horn to get rid
        // of the "ghost in the machine" bug
        if ((prev[RIGHT_TOP] && !buttons[RIGHT_TOP]) ||
            (prev[LEFT_TOP] && !buttons[LEFT_TOP])) {
            debouncer_lockout();
        }

		// output uart on every edge change of a (HACK: no changes while others locked out, GIM bug)
		if ((prev[LEFT_BOT] != buttons[LEFT_BOT]) && !buttons_locked_out)
		{
			uart_send('$'); uart_send('a'); uart_send('='); uart_send(buttons[LEFT_BOT] + 48); uart_send('\r'); uart_send('\n');
		}

		// output uart on every edge change of b (HACK: no changes while others locked out, GIM bug)
		if (prev[RIGHT_BOT] != buttons[RIGHT_BOT] && !buttons_locked_out)
		{
			uart_send('$'); uart_send('b'); uart_send('='); uart_send(buttons[RIGHT_BOT] + 48); uart_send('\r'); uart_send('\n');
		}

		// handle headlights and tail/brake lights
		if (lts_state)
		{
			leds[HEAD_LTS] = 1;
			leds[BRAKE_LTS] = (buttons[BRAKE_SW]) ? 1 : brake_pwm;
			leds[DRIVER_RIGHT] = 1;
			leds[DRIVER_LEFT] = buttons[BRAKE_SW];
		}
		else
		{
			leds[HEAD_LTS] = 0;
			leds[BRAKE_LTS] = buttons[BRAKE_SW];
			leds[DRIVER_RIGHT] = 0;
			leds[DRIVER_LEFT] = buttons[BRAKE_SW];
		}

		// if hazards are engaged, left and right turn are synched
		if (hzd_state)
		{
			leds[LTURN_LTS] = turn_pwm;
			leds[DRIVER_LEFT] = turn_pwm;
			leds[RTURN_LTS] = turn_pwm;
			leds[DRIVER_RIGHT] = turn_pwm;
		}
		else
		{
			// if not, they operate independently based on their status
			if (lf_state)
			{
				leds[LTURN_LTS] = turn_pwm;
				leds[DRIVER_LEFT] = turn_pwm;
			}
			else
			{
				leds[LTURN_LTS] = 0;
			}
			if (rt_state)
			{
				leds[RTURN_LTS] = turn_pwm;
				leds[DRIVER_RIGHT] = turn_pwm;
			}
			else
			{
				leds[RTURN_LTS] = 0;
			}
		}

		// finally set the led outputs
		led_set(HEAD_LTS, leds[HEAD_LTS]);
		led_set(BRAKE_LTS, leds[BRAKE_LTS]);
		led_set(LTURN_LTS, leds[LTURN_LTS]);
		led_set(RTURN_LTS, leds[RTURN_LTS]);
		led_set(DRIVER_LEFT, leds[DRIVER_LEFT]);
		led_set(DRIVER_RIGHT, leds[DRIVER_RIGHT]);
		led_set(STARTER, leds[STARTER]);
		led_set(HORN, leds[HORN]);
	}

	return 0;
}
