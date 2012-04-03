
#include "sysbus.h"

typedef struct {
    SysBusDevice busdev;
    
    char id;
    char ledState;
    CharDriverState *chr; /* Char device */
} stm32_led_state;


static const VMStateDescription vmstate_stm32_led = {
    .name = "stm32_led",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[])
    {
        VMSTATE_UINT8(ledState, stm32_led_state),
        VMSTATE_END_OF_LIST()
    }
};

static void stm32_led_reset(stm32_led_state *s) {
    s->ledState = 0;
}

/*
 * Appelé quand une entrée de led reçoit une IT
 */
static void stm32_led_recvirq(void * opaque, int numPin, int level) {
    printf("La LED change d'état->%d\n", level);
    stm32_led_state *s = (stm32_led_state *) opaque;
    
    envoyer chardev
}


static int stm32_can_receive(void *opaque)
{
    return 1;
}


static void stm32_receive(void *opaque, const uint8_t* buf, int size)
{
    //Une LED de reçois pas d'info du chardev
}

static void stm32_event(void *opaque, int event)
{
}


static int stm32_led_init(SysBusDevice *dev, const unsigned char id) {
    stm32_led_state *s = FROM_SYSBUS(stm32_led_state, dev);
    s->id = id;
    
    
    //Initialisation des pins d'entrées
    qdev_init_gpio_in(&dev->qdev, stm32_led_recvirq, 1);

    /*
    s->chr = qdev_init_chardev(&dev->qdev);
    s->chr = qemu_chr_find("B");
    if (s->chr) {
        qemu_chr_add_handlers(s->chr, stm32_can_receive, stm32_receive, stm32_event, s);
    }
    */
    stm32_led_reset(s);
    vmstate_register(&dev->qdev, -1, &vmstate_stm32_led, s);

    return 0;
}


static int stm32_led_init_(SysBusDevice *dev) {
    return stm32_led_init(dev, 'x');
}

static SysBusDeviceInfo stm32_led_info = {
    .init = stm32_led_init_,
    .qdev.name = "stm32_led",
    .qdev.size = sizeof (stm32_led_state),
    .qdev.vmsd = &vmstate_stm32_led,
};

static void stm32_led_register_devices(void) {
    sysbus_register_withprop(&stm32_led_info);
}

device_init(stm32_led_register_devices)