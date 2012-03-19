/* Allumage de la LED verte,
située sur la broche PB7 */

#include "stm32l1xx.h" /*Inclusion du fichier qui contient
                   toutes les définitions des registres du microcontrôleur
                   STM32L152RB*/

void ConfigPortB(void) /*Fonction qui permet de configurer le port B*/
   {
      RCC->AHBENR |= (1 << 1); /*Activation de l'horloge du port B*/
      GPIOB->MODER = ((GPIOB->MODER & 0xffff3fff) | (0x00004000)); /*Configuration de la broche PB7 en
                                                    sortie à usage général*/
      GPIOB->OTYPER = ((GPIOB->OTYPER & 0xffffff7f) | (0x00000000)); /*Configuration de la broche PB7 en
                                                      sortie push-pull*/
      GPIOB->OSPEEDR = ((GPIOB->OSPEEDR & 0xffff3fff) | (0x00000c000)); /*Configuration de la broche PB7 en
                                                        sortie 40 MHz*/
      GPIOB->PUPDR = ((GPIOB->PUPDR & 0xffff3fff) | (0x00000000)); /*Désactivation des résistances
                                                      de pull-up et de pull-down
                                                    de la broche PB7*/
   }

int main(void) /*Fonction principale*/
   {
      ConfigPortB(); /*Appel de la fonction qui permet de configurer
                       le port B*/
         while(1) /*Boucle sans fin*/
            {
               GPIOB->ODR   = ((GPIOB->ODR & 0xffffff7f) | (0x00000080)); /*Envoi d'un un logique sur
                                                         la broche PB7*/
            }
   }

void SystemInit (void) /*Fonction qui permet l'initialisation du microcontrôleur STM32L152RB*/
   {
       
   }
