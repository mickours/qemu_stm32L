#include "sysbus.h"

typedef struct {
    SysBusDevice busdev;
    
    /* Registres GPIO (Reference Manual p119 */
    uint32_t mode; /* Mode */
    uint32_t otype; /* Output type */
    uint32_t ospeed; /* Output speed */
    uint32_t pupd; /* Pull-up/Pull-down */
    uint32_t id; /* Input data */
    uint32_t od; /* Output data register */
    uint32_t bsr; /* Bit set/reset */
    uint32_t lck; /* Lock */
    uint32_t afrl; /* Alternate function low */
    uint32_t afrh; /* Alternate function high */
    
    qemu_irq irq;
    qemu_irq out[8];
    const unsigned char *id;
} stm32_gpio_state;

static const VMStateDescription vmstate_stm32_gpio = {
    .name = "stm32_gpio",
    .version_id = 2,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(mode, stm32_gpio_state),
        VMSTATE_UINT32(otype, stm32_gpio_state),
        VMSTATE_UINT32(ospeed, stm32_gpio_state),
        VMSTATE_UINT32(pupd, stm32_gpio_state),
        VMSTATE_UINT32(id, stm32_gpio_state),
        VMSTATE_UINT32(od, stm32_gpio_state),
        VMSTATE_UINT32(bsr, stm32_gpio_state),
        VMSTATE_UINT32(lck, stm32_gpio_state),
        VMSTATE_UINT32(afrl, stm32_gpio_state),
        VMSTATE_UINT32(afrh, stm32_gpio_state),
        VMSTATE_END_OF_LIST()
    }
};

static void stm32_gpio_update(stm32_gpio_state *s)
{
    uint8_t changed;
    uint8_t mask;
    uint8_t out;
    int i;

    /* Outputs float high.  */
    /* FIXME: This is board dependent.  */
    out = (s->data & s->dir) | ~s->dir;
    changed = s->old_data ^ out;
    if (!changed)
        return;

    s->old_data = out;
    for (i = 0; i < 8; i++) {
        mask = 1 << i;
        if (changed & mask) {
            DPRINTF("Set output %d = %d\n", i, (out & mask) != 0);
            qemu_set_irq(s->out[i], (out & mask) != 0);
        }
    }

    /* FIXME: Implement input interrupts.  */
}

static uint32_t stm32_gpio_read(void *opaque, target_phys_addr_t offset)
{
    stm32_gpio_state *s = (stm32_gpio_state *)opaque;

    if (offset >= 0xfd0 && offset < 0x1000) {
        return s->id[(offset - 0xfd0) >> 2];
    }
    if (offset < 0x400) {
        return s->data & (offset >> 2);
    }
    switch (offset) {
    case 0x400: /* Direction */
        return s->dir;
    case 0x404: /* Interrupt sense */
        return s->isense;
    case 0x408: /* Interrupt both edges */
        return s->ibe;
    case 0x40c: /* Interrupt event */
        return s->iev;
    case 0x410: /* Interrupt mask */
        return s->im;
    case 0x414: /* Raw interrupt status */
        return s->istate;
    case 0x418: /* Masked interrupt status */
        return s->istate | s->im;
    case 0x420: /* Alternate function select */
        return s->afsel;
    case 0x500: /* 2mA drive */
        return s->dr2r;
    case 0x504: /* 4mA drive */
        return s->dr4r;
    case 0x508: /* 8mA drive */
        return s->dr8r;
    case 0x50c: /* Open drain */
        return s->odr;
    case 0x510: /* Pull-up */
        return s->pur;
    case 0x514: /* Pull-down */
        return s->pdr;
    case 0x518: /* Slew rate control */
        return s->slr;
    case 0x51c: /* Digital enable */
        return s->den;
    case 0x520: /* Lock */
        return s->locked;
    case 0x524: /* Commit */
        return s->cr;
    case 0x528: /* Analog mode select */
        return s->amsel;
    default:
        hw_error("stm32_gpio_read: Bad offset %x\n", (int)offset);
        return 0;
    }
}

static void stm32_gpio_write(void *opaque, target_phys_addr_t offset,
                        uint32_t value)
{
    stm32_gpio_state *s = (stm32_gpio_state *)opaque;
    uint8_t mask;

    if (offset < 0x400) {
        mask = (offset >> 2) & s->dir;
        s->data = (s->data & ~mask) | (value & mask);
        stm32_gpio_update(s);
        return;
    }
    switch (offset) {
    case 0x400: /* Direction */
        s->dir = value & 0xff;
        break;
    case 0x404: /* Interrupt sense */
        s->isense = value & 0xff;
        break;
    case 0x408: /* Interrupt both edges */
        s->ibe = value & 0xff;
        break;
    case 0x40c: /* Interrupt event */
        s->iev = value & 0xff;
        break;
    case 0x410: /* Interrupt mask */
        s->im = value & 0xff;
        break;
    case 0x41c: /* Interrupt clear */
        s->istate &= ~value;
        break;
    case 0x420: /* Alternate function select */
        mask = s->cr;
        s->afsel = (s->afsel & ~mask) | (value & mask);
        break;
    case 0x500: /* 2mA drive */
        s->dr2r = value & 0xff;
        break;
    case 0x504: /* 4mA drive */
        s->dr4r = value & 0xff;
        break;
    case 0x508: /* 8mA drive */
        s->dr8r = value & 0xff;
        break;
    case 0x50c: /* Open drain */
        s->odr = value & 0xff;
        break;
    case 0x510: /* Pull-up */
        s->pur = value & 0xff;
        break;
    case 0x514: /* Pull-down */
        s->pdr = value & 0xff;
        break;
    case 0x518: /* Slew rate control */
        s->slr = value & 0xff;
        break;
    case 0x51c: /* Digital enable */
        s->den = value & 0xff;
        break;
    case 0x520: /* Lock */
        s->locked = (value != 0xacce551);
        break;
    case 0x524: /* Commit */
        if (!s->locked)
            s->cr = value & 0xff;
        break;
    case 0x528:
        s->amsel = value & 0xff;
        break;
    default:
        hw_error("stm32_gpio_write: Bad offset %x\n", (int)offset);
    }
    stm32_gpio_update(s);
}

static void stm32_gpio_reset(stm32_gpio_state *s)
{
  s->locked = 1;
  s->cr = 0xff;
}

static void stm32_gpio_set_irq(void * opaque, int irq, int level)
{
    stm32_gpio_state *s = (stm32_gpio_state *)opaque;
    uint8_t mask;

    mask = 1 << irq;
    if ((s->dir & mask) == 0) {
        s->data &= ~mask;
        if (level)
            s->data |= mask;
        stm32_gpio_update(s);
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

static int stm32_gpio_init(SysBusDevice *dev, const unsigned char *id)
{
    int iomemtype;
    stm32_gpio_state *s = FROM_SYSBUS(stm32_gpio_state, dev);
    s->id = id;
    iomemtype = cpu_register_io_memory(stm32_gpio_readfn,
                                       stm32_gpio_writefn, s,
                                       DEVICE_NATIVE_ENDIAN);
    sysbus_init_mmio(dev, 0x1000, iomemtype);
    sysbus_init_irq(dev, &s->irq);
    qdev_init_gpio_in(&dev->qdev, stm32_gpio_set_irq, 8);
    qdev_init_gpio_out(&dev->qdev, s->out, 8);
    stm32_gpio_reset(s);
    return 0;
}

static int stm32_gpio_init_luminary(SysBusDevice *dev)
{
    return stm32_gpio_init(dev, stm32_gpio_id_luminary);
}

static int stm32_gpio_init_arm(SysBusDevice *dev)
{
    return stm32_gpio_init(dev, stm32_gpio_id);
}

static SysBusDeviceInfo stm32_gpio_info = {
    .init = stm32_gpio_init_arm,
    .qdev.name = "stm32_gpio",
    .qdev.size = sizeof(stm32_gpio_state),
    .qdev.vmsd = &vmstate_stm32_gpio,
};

static SysBusDeviceInfo stm32_gpio_luminary_info = {
    .init = stm32_gpio_init_luminary,
    .qdev.name = "stm32_gpio_luminary",
    .qdev.size = sizeof(stm32_gpio_state),
    .qdev.vmsd = &vmstate_stm32_gpio,
};

static void stm32_gpio_register_devices(void)
{
    sysbus_register_withprop(&stm32_gpio_info);
    sysbus_register_withprop(&stm32_gpio_luminary_info);
}

device_init(stm32_gpio_register_devices)
