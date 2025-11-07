#include <stdio.h>
#include "pico/stdlib.h"

#define LED_PIN 15	
#define LED_PIN2 13	

int main() {
	stdio_init_all();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_init(LED_PIN2);
	gpio_set_dir(LED_PIN2, GPIO_OUT);
	while (true) {
		gpio_put(LED_PIN, true);
		sleep_ms(150);
		gpio_put(LED_PIN, false);	
		sleep_ms(150);
		gpio_put(LED_PIN2, true);
		sleep_ms(300);
		gpio_put(LED_PIN2, false);	
		sleep_ms(300);
	}
}
