
#include "sysbus.h"
#include "exec-memory.h"
#include "arm-misc.h"
#include "boards.h"

#define NB_GPIO 6

#define GPIO_A 0 //0x4002 0000 - 0x4002 03FF
#define GPIO_B 1 //0x4002 0400 - 0x4002 07FF
#define GPIO_C 2 //0x4002 0800 - 0x4002 0BFF
#define GPIO_D 3 //0x4002 0C00 - 0x4002 0FFF
#define GPIO_E 4 //0x4002 1000 - 0x4002 13FF
#define GPIO_H 5 //0x4002 1400 - 0x4002 17FF

typedef const struct {
    const char *name;
    uint16_t f_size;
} stm32_board_info;

static stm32_board_info stm32_board = {
    "stm32l152rbt6",
    0x0080 //128kb
};

static void stm32l152rbt6_init(ram_addr_t ram_size,
        const char *boot_device,
        const char *kernel_filename, const char *kernel_cmdline,
        const char *initrd_filename, const char *cpu_model) {

    static const uint32_t gpio_addr[NB_GPIO] = {
        0x40020000, //GPIO_A
        0x40020400, //GPIO_B
        0x40020800, //GPIO_C
        0x40020C00, //GPIO_D
        0x40021000, //GPIO_E
        0x40021400  //GPIO_H
    };

    static const int gpio_irq[NB_GPIO] = {0,1,2,3,4,5};


    qemu_irq *pic;
    DeviceState * gpio_dev[NB_GPIO];
    //qemu_irq gpio_in[NB_GPIO][8];
    qemu_irq gpio_out[NB_GPIO][8];
    uint16_t flash_size;
    uint16_t sram_size;
    int i,j;


    MemoryRegion *address_space_mem = get_system_memory();

    flash_size = stm32_board.f_size;
    sram_size = 0x0010; //16 kbits
    pic = armv7m_init(address_space_mem,
            flash_size, sram_size, kernel_filename, cpu_model);

    for (i = 0; i < NB_GPIO; i++) {
        gpio_dev[i] = sysbus_create_simple("pl061", gpio_addr[i],
                pic[gpio_irq[i]]);
        for (j = 0; j < 8; j++) {
            //gpio_in[i][j] = qdev_get_gpio_in(gpio_dev[i], j);
            gpio_out[i][j] = NULL;
        }
    }

    for (i = 0; i < NB_GPIO; i++) {
        for (j = 0; j < 8; j++) {
            if (gpio_out[i][j]) {
                qdev_connect_gpio_out(gpio_dev[i], j, gpio_out[i][j]);
            }
        }
    }
}

static QEMUMachine stm32l152rbt6_machine = {
    .name = "stm32l152rbt6",
    .desc = "STM32L Discovery",
    .init = stm32l152rbt6_init,
};

static void stm32l_machine_init(void) {
    qemu_register_machine(&stm32l152rbt6_machine);
}

machine_init(stm32l_machine_init);

static void stm32l_register_devices(void) {

}

device_init(stm32l_register_devices)


