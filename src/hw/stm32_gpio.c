#include "sysbus.h"

#define NB_PIN 15

typedef struct {
    SysBusDevice busdev;

    /* Registres GPIO (Reference Manual p119 */
    uint32_t mode; /* Mode */
    uint16_t otype; /* Output type */
    uint32_t ospeed; /* Output speed */
    uint32_t pupd; /* Pull-up/Pull-down */
    uint16_t ind; /* Input data */
    uint16_t outd; /* Output data register */
    uint16_t outd_old; /* Output data register */
    uint32_t bsr; /* Bit set/reset */
    uint32_t lck; /* Lock */
    uint32_t afrl; /* Alternate function low */
    uint32_t afrh; /* Alternate function high */
    
    qemu_irq irq_out[NB_PIN];
    unsigned char id;
} stm32_gpio_state;


static const VMStateDescription vmstate_stm32_gpio = {
    .name = "stm32_gpio",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[])
    {
        VMSTATE_UINT32(mode, stm32_gpio_state),
        VMSTATE_UINT16(otype, stm32_gpio_state),
        VMSTATE_UINT32(ospeed, stm32_gpio_state),
        VMSTATE_UINT32(pupd, stm32_gpio_state),
        VMSTATE_UINT16(ind, stm32_gpio_state),
        VMSTATE_UINT16(outd, stm32_gpio_state),
        VMSTATE_UINT16(outd_old, stm32_gpio_state),
        VMSTATE_UINT32(bsr, stm32_gpio_state),
        VMSTATE_UINT32(lck, stm32_gpio_state),
        VMSTATE_UINT32(afrl, stm32_gpio_state),
        VMSTATE_UINT32(afrh, stm32_gpio_state),
        VMSTATE_END_OF_LIST()
    }
};


static void stm32_gpio_update(stm32_gpio_state *s) {
    uint16_t changed;
    uint16_t mask;
    int i;
    
    changed = s->outd_old ^ s->outd; //XOR: Tous les bits à 1 seront les bits changés
    if (!changed)
        return;

    s->outd_old = s->outd;
    for (i = 0; i < 8; i++) {
        mask = 1 << i;
        if (changed & mask) {
            //Conditions de changement
            //--Mode register != 00
            uint32_t modeMask = (1 << (i*2)) | (1 << ((i*2) + 1));
            if((s->mode & modeMask) != 0) {
                //Envoie une IRQ vers le periphérique branché sur la pin du GPIO
                //FIXME: La transmition vers la pin est directe alors qu'elle devrait se produire lors du prochain tick de la RCC
                qemu_set_irq(s->irq_out[i], (s->outd & mask) != 0);
            }
        }
    }
}


static uint32_t stm32_gpio_read(void *opaque, target_phys_addr_t offset) {
    stm32_gpio_state *s = (stm32_gpio_state *) opaque;

    switch (offset) {
        case 0x00: /* Mode */
            return s->mode;
        case 0x04: /* oType */
            return s->otype;
        case 0x08: /* oSpeed */
            return s->ospeed;
        case 0x0C: /* Pull-up / Pull-down */
            return s->pupd;
        case 0x10: /* Input data register */
            return s->ind;
        case 0x14: /* Output data */
            return s->outd;
        case 0x18: /* BSR */
            return 0x0; //Write only
        case 0x1C: /* lock */
            return 0x0; //Non implémenté
        case 0x20: /* AFRL */
            return s->afrl;
        case 0x24: /* AFRH */
            return s->afrh;
        default:
            hw_error("stm32_gpio_read: Bad offset %x\n", (int) offset);
            return 0;
    }
}


static void stm32_gpio_write(void *opaque, target_phys_addr_t offset, uint32_t value) {
    stm32_gpio_state *s = (stm32_gpio_state *) opaque;
    int i;
    uint16_t low = (uint16_t)value;
    uint16_t high = (uint16_t)value >> 16;
    
    switch (offset) {
        case 0x00: /* Mode */
            s->mode = value;
            break;
        case 0x04: /* oType */
            s->otype = value;
            break;
        case 0x08: /* oSpeed */
            s->ospeed = value;
            break;
        case 0x0C: /* Pull-up / Pull-down */
            s->pupd = value;
            break;
        case 0x10: /* Input data register */
            //Read only
            break;
        case 0x14: /* Output data */
            s->outd = value;
            stm32_gpio_update(s);
            break;
            
        case 0x18: /* BSR */
            //set = low
            //reset = high
            for(i=0; i<16; i++) {
                int mask = 1 << i;
                if((high & mask) != 0) { //Si bit reset[i]
                    s->outd &= ~(1 << i); //Mise à 0
                }
                if((low & mask) != 0) { //Si bit reset[i]
                    s->outd |= (1 << i); //Mise à 1
                }
            }
            stm32_gpio_update(s);
            break;
            
        case 0x1C: /* lock */
            //FIXME: Non implémenté
            break;
        case 0x20: /* AFRL */
            s->afrl = value;
            break;
        case 0x24: /* AFRH */
            s->afrh = value;
            break;
        default:
            hw_error("stm32_gpio_write: Bad offset %x\n", (int) offset);
    }
}


static void stm32_gpio_reset(stm32_gpio_state *s) {
    switch (s->id) {
        case 'A':
            s->mode = 0xA8000000;
            s->pupd = 0x64000000;
            s->ospeed = 0x00000000;
            break;
        case 'B':
            s->mode = 0x00000280;
            s->pupd = 0x00000100;
            s->ospeed = 0x000000C0;
            break;
        default:
            s->mode = 0x00000000;
            s->pupd = 0x00000000;
            s->ospeed = 0x00000000;
    }
    
    //Valeur commune
    s->ind =    0x00000000;
    s->otype =  0x00000000;
    s->outd =   0x00000000;
    s->outd_old = s->outd;
    s->bsr =    0x00000000;
    s->lck =    0x00000000;
    s->afrh =   0x00000000;
    s->afrl =   0x00000000;

}

/*
 * Appelé quand une entrée de GPIO reçois une IT
 */
static void stm32_gpio_in_recv(void * opaque, int numPin, int level) {
    assert(numPin>=0 && numPin<NB_PIN);
    stm32_gpio_state *s = (stm32_gpio_state *) opaque;
    
    
    uint32_t mask = (1 << (numPin*2)) | (1 << ((numPin*2) + 1));
    //Test si la pin est configurée en input
    //--Pull-up Pull-Down -> Doit être différent de 00
    if((s->pupd & mask) == 0) {return;} 
    //--Mode register -> 00
    if((s->mode & mask) != 0) {return;} 
    
    //Ecriture dans le registre input data register
    printf("GPIO_in[%d]->%d\n", numPin, level);
    if(level) {
        s->ind |= (1 << numPin); //Mise à 1
    } else {
        s->ind &= ~(1 << numPin); //Mise à 0
    }
}


static CPUReadMemoryFunc * const stm32_gpio_readfn[] = {
    stm32_gpio_read,
    stm32_gpio_read,
    stm32_gpio_read
};

static CPUWriteMemoryFunc * const stm32_gpio_writefn[] = {
    stm32_gpio_write,
    stm32_gpio_write,
    stm32_gpio_write
};


static int stm32_gpio_init(SysBusDevice *dev, const unsigned char id) {
    int iomemtype;
    stm32_gpio_state *s = FROM_SYSBUS(stm32_gpio_state, dev);
    s->id = id;
    
    //Initialisation de la plage mémoire
    iomemtype = cpu_register_io_memory(stm32_gpio_readfn, stm32_gpio_writefn, s, DEVICE_NATIVE_ENDIAN);
    sysbus_init_mmio(dev, 0x24, iomemtype);
    
    //Initialisation des pins
    qdev_init_gpio_in(&dev->qdev, stm32_gpio_in_recv, NB_PIN);
    qdev_init_gpio_out(&dev->qdev, s->irq_out, NB_PIN);

    //Initialisation
    stm32_gpio_reset(s);
    vmstate_register(&dev->qdev, -1, &vmstate_stm32_gpio, s);

    return 0;
}




static int stm32_gpio_init_B(SysBusDevice *dev) {
    return stm32_gpio_init(dev, 'B');
}
static SysBusDeviceInfo stm32_gpioB_info = {
    .init = stm32_gpio_init_B,
    .qdev.name = "stm32_gpio_B",
    .qdev.size = sizeof (stm32_gpio_state),
    .qdev.vmsd = &vmstate_stm32_gpio,
};


static int stm32_gpio_init_A(SysBusDevice *dev) {
    return stm32_gpio_init(dev, 'A');
}
static SysBusDeviceInfo stm32_gpioA_info = {
    .init = stm32_gpio_init_A,
    .qdev.name = "stm32_gpio_A",
    .qdev.size = sizeof (stm32_gpio_state),
    .qdev.vmsd = &vmstate_stm32_gpio,
};


static void stm32_gpio_register_devices(void) {
    sysbus_register_withprop(&stm32_gpioA_info);
    sysbus_register_withprop(&stm32_gpioB_info);
}
device_init(stm32_gpio_register_devices)