#include "sysbus.h"

typedef struct {
    uint32_t CR; //Clock control register
    uint32_t ICSCR; //Internal clock sources calibration register
    uint32_t CFGR; //Clock configuration register
    uint32_t CIR; //Clock interrupt register
    uint32_t AHBRSTR; //AHB peripheral reset register
    uint32_t APB2RSTR; //APB2 peripheral reset register
    uint32_t APB1RSTR; //APB1 peripheral reset register
    uint32_t AHBENR; //AHB peripheral clock enable register
    uint32_t APB2ENR; //APB2 peripheral clock enable register
    uint32_t APB1ENR; //APB1 peripheral clock enable register
    uint32_t AHBLPENR; //AHB peripheral clock enable in low power mode register
    uint32_t APB2LPENR; //APB2 peripheral clock enable in low power mode register
    uint32_t APB1LPENR; //APB1 peripheral clock enable in low power mode register
    uint32_t CSR; //Control/status register
    qemu_irq irq;
    unsigned char id;
} stm32_rcc_state;

static const VMStateDescription vmstate_stm32_rcc = {
    .name = "stm32_rcc",
    .version_id = 2,
    .minimum_version_id = 1,
    .fields = (VMStateField[])
    {
        VMSTATE_UINT32(CR, stm32_rcc_state),
        VMSTATE_UINT32(ICSCR, stm32_rcc_state),
        VMSTATE_UINT32(CFGR, stm32_rcc_state),
        VMSTATE_UINT32(CIR, stm32_rcc_state),
        VMSTATE_UINT32(AHBRSTR, stm32_rcc_state),
        VMSTATE_UINT32(APB2RSTR, stm32_rcc_state),
        VMSTATE_UINT32(APB1RSTR, stm32_rcc_state),
        VMSTATE_UINT32(AHBENR, stm32_rcc_state),
        VMSTATE_UINT32(APB2ENR, stm32_rcc_state),
        VMSTATE_UINT32(APB1ENR, stm32_rcc_state),
        VMSTATE_UINT32(AHBLPENR, stm32_rcc_state),
        VMSTATE_UINT32(APB2LPENR, stm32_rcc_state),
        VMSTATE_UINT32(APB1LPENR, stm32_rcc_state),
        VMSTATE_UINT32(CSR, stm32_rcc_state),
        VMSTATE_END_OF_LIST()
    }
};

static void stm32_rcc_update(stm32_rcc_state *s) {
    /* FIXME: Implement interrupts.  */
}

static uint32_t stm32_rcc_read(void *opaque, target_phys_addr_t offset) {
    stm32_rcc_state *s = (stm32_rcc_state *) opaque;
    switch (offset) {
        case 0x00: //Clock control register
            return s->CR;
            break;
        case 0x04:
            return s->ICSCR; //Internal clock sources calibration register
            break;
        case 0x08:
            return s->CFGR; //Clock configuration register
            break;
        case 0x0C:
            return s->CIR; //Clock interrupt register
            break;
        case 0x10:
            return s->AHBRSTR; //AHB peripheral reset register
            break;
        case 0x14:
            return s->APB2RSTR; //APB2 peripheral reset register
            break;
        case 0x18:
            return s->APB1RSTR; //APB1 peripheral reset register
            break;
        case 0x1C:
            return s->AHBENR; //AHB peripheral clock enable register
            break;
        case 0x20:
            return s->APB2ENR; //APB2 peripheral clock enable register
            break;
        case 0x24:
            return s->APB1ENR; //APB1 peripheral clock enable register
            break;
        case 0x28:
            return s->AHBLPENR; //AHB peripheral clock enable in low power mode register
            break;
        case 0x2C:
            return s->APB2LPENR; //APB2 peripheral clock enable in low power mode register
            break;
        case 0x30:
            return s->APB1LPENR; //APB1 peripheral clock enable in low power mode register
            break;
        case 0x34:
            return s->CSR; //Control/status register
        default:
            hw_error("stm32_rcc_read: Bad offset %x\n", (int) offset);
            return 0;
    }
}

static void stm32_rcc_write(void *opaque, target_phys_addr_t offset,
        uint32_t value) {
    stm32_rcc_state *s = (stm32_rcc_state *) opaque;
    switch (offset) {
        case 0x00: //Clock control register
            s->CR = value;
            break;
        case 0x04:
            s->ICSCR = value; //Internal clock sources calibration register
            break;
        case 0x08:
            s->CFGR = value; //Clock configuration register
            break;
        case 0x0C:
            s->CIR = value; //Clock interrupt register
            break;
        case 0x10:
            s->AHBRSTR = value; //AHB peripheral reset register
            break;
        case 0x14:
            s->APB2RSTR = value; //APB2 peripheral reset register
            break;
        case 0x18:
            s->APB1RSTR = value; //APB1 peripheral reset register
            break;
        case 0x1C:
            s->AHBENR = value; //AHB peripheral clock enable register
            break;
        case 0x20:
            s->APB2ENR = value; //APB2 peripheral clock enable register
            break;
        case 0x24:
            s->APB1ENR = value; //APB1 peripheral clock enable register
            break;
        case 0x28:
            s->AHBLPENR = value; //AHB peripheral clock enable in low power mode register
            break;
        case 0x2C:
            s->APB2LPENR = value; //APB2 peripheral clock enable in low power mode register
            break;
        case 0x30:
            s->APB1LPENR = value; //APB1 peripheral clock enable in low power mode register
            break;
        case 0x34:
            s->CSR = value; //Control/status register
            break;
        default:
            hw_error("stm32_rcc_write: Bad offset %x\n", (int) offset);
    }
    stm32_rcc_update(s);
}

static void stm32_rcc_reset(stm32_rcc_state *s) {
    s->CR = 0x00000300; //Clock control register
    s->ICSCR = 0x0000B000; //Internal clock sources calibration register
    s->CFGR = 0x00000000; //Clock configuration register
    s->CIR = 0x00000000; //Clock interrupt register
    s->AHBRSTR = 0x00000000; //AHB peripheral reset register
    s->APB2RSTR = 0x00000000; //APB2 peripheral reset register
    s->APB1RSTR = 0x00000000; //APB1 peripheral reset register
    s->AHBENR = 0x00008000; //AHB peripheral clock enable register
    s->APB2ENR = 0x00000000; //APB2 peripheral clock enable register
    s->APB1ENR = 0x00000000; //APB1 peripheral clock enable register
    s->AHBLPENR = 0x0101903F; //AHB peripheral clock enable in low power mode register
    s->APB2LPENR = 0x0000521D; //APB2 peripheral clock enable in low power mode register
    s->APB1LPENR = 0xB0E64A37; //APB1 peripheral clock enable in low power mode register
    s->CSR = 0x0C000000; //Control/status register
}


static void stm32_rcc_set_irq(void * opaque, int irq, int level) {
    //TODO gestion des irq
    //stm32_rcc_state *s = (stm32_rcc_state *) opaque;

}

static CPUReadMemoryFunc * const stm32_rcc_readfn[] = {
    stm32_rcc_read,
    stm32_rcc_read,
    stm32_rcc_read
};

static CPUWriteMemoryFunc * const stm32_rcc_writefn[] = {
    stm32_rcc_write,
    stm32_rcc_write,
    stm32_rcc_write
};

static int stm32_rcc_init(SysBusDevice *dev) {
    int iomemtype;
    stm32_rcc_state *s = FROM_SYSBUS(stm32_rcc_state, dev);
    s->id = "stm32_rcc";
    iomemtype = cpu_register_io_memory(stm32_rcc_readfn,
            stm32_rcc_writefn, s,
            DEVICE_NATIVE_ENDIAN);
    sysbus_init_mmio(dev, 0x1000, iomemtype);
    sysbus_init_irq(dev, &s->irq);
    stm32_rcc_reset(s);
    return 0;
}

static SysBusDeviceInfo stm32_rcc_info = {
    .init = stm32_rcc_init,
    .qdev.name = "stm32_rcc",
    .qdev.size = sizeof (stm32_rcc_state),
    .qdev.vmsd = &vmstate_stm32_rcc,
};

static void stm32_rcc_register_devices(void) {
    sysbus_register_withprop(&stm32_rcc_info);
}

device_init(stm32_rcc_register_devices)
