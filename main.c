/* missing type */

typedef unsigned int uint32_t;


/* hardware configuration */

#define GPIOB 0x40020400 /* port B */
#define GPIOB_MODER (GPIOB + 0x00) /* port mode register */
#define LED_PORT_ODR (GPIOB + 0x14) /* port output data register */

#define LED_BLUE (1 << 6) /* port B, pin 6 */
#define LED_GREEN (1 << 7) /* port B, pin 7 */

#define delay()						\
do {							\
  register unsigned int i;				\
  for (i = 0; i < 1000000; ++i)				\
    __asm__ __volatile__ ("nop\n\t":::"memory");	\
} while (0)

void main(void)
{
  *(volatile uint32_t*)GPIOB_MODER |= (1 << (7 * 2)) | (1 << (6 * 2));

  while (1)
  {
    *(volatile uint32_t*)LED_PORT_ODR = LED_BLUE | LED_GREEN;
    delay();
    *(volatile uint32_t*)LED_PORT_ODR = 0;
    delay();
  }
}
