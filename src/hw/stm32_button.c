
#include "sysbus.h"

typedef struct {
    SysBusDevice busdev;
    uint8_t buttonState;
    qemu_irq gpio_out;
    CharDriverState* chr; /* Char device */
} stm32_button_state;


static const VMStateDescription vmstate_stm32_button = {
    .name = "stm32_button",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[])
    {
        VMSTATE_UINT8(buttonState, stm32_button_state),
        VMSTATE_END_OF_LIST()
    }
};

static void stm32_button_reset(stm32_button_state *s) {
    s->buttonState = 0;
}

//---------------------------------------------------
//CHARDEV
//---------------------------------------------------
static int stm32_can_receive(void *opaque) {return 1;}
static void stm32_receive(void *opaque, const uint8_t* buf, int size)
{
    stm32_button_state *s = (stm32_button_state *) opaque;
    
    int i=0;
    for(i=0; i<size; i++) {
        //Leve une IRQ
        uint8_t etat = buf[i];
        s->buttonState = etat;
        printf("La BUTTON change d'état->%d\n", (int)etat);
        qemu_set_irq(s->gpio_out, (int)etat);
    }
}
static void stm32_event(void *opaque, int event) {/*NOT USED*/}


static int stm32_button_init(SysBusDevice *dev, const char* id) {
    stm32_button_state *s = FROM_SYSBUS(stm32_button_state, dev);
    
    //Initialisation de la pin de sortie
    qdev_init_gpio_out(&dev->qdev, &s->gpio_out, 1);
    
    //Initialisation du Chardev
    s->chr = qdev_init_chardev(&dev->qdev);
    s->chr = qemu_chr_find(id);
    if (s->chr) {
        qemu_chr_add_handlers(s->chr, stm32_can_receive, stm32_receive, stm32_event, s);
    }
    
    stm32_button_reset(s);
    vmstate_register(&dev->qdev, -1, &vmstate_stm32_button, s);
 
    return 0;
}


static int stm32_button_init_(SysBusDevice *dev) {
    return stm32_button_init(dev, "user0");
}

static SysBusDeviceInfo stm32_button_info = {
    .init = stm32_button_init_,
    .qdev.name = "stm32_button",
    .qdev.size = sizeof (stm32_button_state),
    .qdev.vmsd = &vmstate_stm32_button,
};

static void stm32_button_register_devices(void) {
    sysbus_register_withprop(&stm32_button_info);
}

device_init(stm32_button_register_devices)