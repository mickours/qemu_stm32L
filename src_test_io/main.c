#define delay() \
do { \
	register unsigned int i; \
	for (i = 0; i < 1000000; ++i) \
	__asm__ __volatile__ ("nop\n\t":::"memory"); \
} while (0)

typedef unsigned int uint32_t;






//----------------------------------------------------------------
// Constantes: Adresse des registres
//----------------------------------------------------------------

//GPIO_B - LEDs
#define RCC_AHBENR 0x4002381c /* Reset and clock control enbable register */
#define GPIOB 0x40020400 /* GPIO_B */
#define GPIOB_MODER (GPIOB + 0x00) /* Mode register */
#define GPIOB_ODR (GPIOB + 0x14) /* Output data register */

//LED pins
#define GPIOB_PIN_BLUE_LED (1 << 6) /* port B, pin 6 */
#define GPIOB_PIN_GREEN_LED (1 << 7) /* port B, pin 7 */

//GPIO_A - Button
#define GPIOA 0x40020000 /* port A */
#define GPIOA_PUPD (GPIOA + 0x0c) /* Pull-up / Pull-Down register*/
#define GPIOA_IDR (GPIOA + 0x10) /* Input data register */
#define GPIOA_MODER (GPIOA + 0x00) /* port mode register */
#define GPIOA_PIN_USER_BUTTON (1 << 0) /* port A, pin 0*/






//----------------------------------------------------------------
// Fonctions de gestion des LEDs
//----------------------------------------------------------------

//Allume la led
static inline void switch_leds_on(void) {
	*(volatile uint32_t*)GPIOB_ODR = GPIOB_PIN_BLUE_LED | GPIOB_PIN_GREEN_LED;
}

//Eteint les leds
static inline void switch_leds_off(void) {
	*(volatile uint32_t*)GPIOB_ODR = 0;
}





//----------------------------------------------------------------
// Fonctions d'initialisation
//----------------------------------------------------------------

//Initialise les leds
static inline void setup_leds(void)
{
	*(volatile uint32_t*)RCC_AHBENR |= (1 << 1); /* Activation de l'horloge du port B */
	*(volatile uint32_t*)GPIOB_MODER |= (1 << (7 * 2)) | (1 << (6 * 2)); /* Configure port 6 and 7 as output */
}

//Initialise le bouton
static inline void setup_button(void){
	*(volatile uint32_t*)RCC_AHBENR |= (1 << 0);
	*(volatile uint32_t*)GPIOA_PUPD |= (1 << 1);
}

//Initialise le système
void SystemInit(void){
	setup_leds();
	setup_button();
	switch_leds_off();
}





void main(void) {
	SystemInit();
	uint32_t etat = *(volatile uint32_t*)GPIOA_IDR;


	while (1){
		//Test si le registre de valeur du bouton a changé de valeur
		if (etat != *(volatile uint32_t*)GPIOA_IDR) {
			etat = *(volatile uint32_t*)GPIOA_IDR;
			if ((etat & GPIOA_PIN_USER_BUTTON) == 0)
				switch_leds_off();
			else
				switch_leds_on();
		}
	}
}


