/* missing type */
typedef unsigned int uint32_t;


/* hardware configuration */

#define RCC_AHBENR 0x4002381c
#define GPIOB 0x40020400 /* port B */
#define GPIOB_MODER (GPIOB + 0x00) /* port mode register */
#define LED_PORT_ODR (GPIOB + 0x14) /* port output data register */

#define LED_BLUE (1 << 6) /* port B, pin 6 */
#define LED_GREEN (1 << 7) /* port B, pin 7 */
#define LED_ORANGE 0
#define LED_RED 0

static inline void setup_leds(void)
{
  *(volatile uint32_t*)RCC_AHBENR |= (1 << 1); /*Activation de l'horloge du port B*/
  /* configure port 6 and 7 as output */
  *(volatile uint32_t*)GPIOB_MODER |= (1 << (7 * 2)) | (1 << (6 * 2));
}

static inline void switch_leds_on(void)
{
  *(volatile uint32_t*)LED_PORT_ODR = LED_BLUE | LED_GREEN;
}

static inline void switch_leds_off(void)
{
  *(volatile uint32_t*)LED_PORT_ODR = 0;
}

#define delay() \
do { \
register unsigned int i; \
for (i = 0; i <1000000; ++i) \
__asm__ __volatile__ ("nop\n\t":::"memory"); \
} while (0)

/* static void __attribute__((naked)) __attribute__((used)) main(void) */
void main(void)
{
  setup_leds();

  while (1)
  {
    switch_leds_on();
    delay();
    switch_leds_off();
    delay();
  }
}

void SystemInit(void){
}
