#include "hw/pci/pci.h"
#include "hw/i386/pc.h"
#include "hw/timer/i8254.h"
#include "hw/audio/pcspk.h"
#include "hw/hw.h"

#define MEM_PCI_VENDOR_ID   0x1234
#define MEM_PCI_DEVICE_ID   0x5678
#define MEM_PCI_REVISION_ID 0x73
#define PCI_MEM_SIZE        0x00000010

typedef struct MEMPCIState {
  MemoryRegion mem;
} MEMPCIState;

typedef struct PCIMEMPCIState {
  PCIDevice pci_dev;
  uint32_t mem_pci_base;
  MEMPCIState state;
} PCIMEMPCIState;

static const VMStateDescription vmstate_mem_pci = {
  .name = "mem_pci",
  .version_id = 0,
  .minimum_version_id = 0,
  .fields = (VMStateField[]) {
    VMSTATE_PCI_DEVICE(pci_dev, PCIMEMPCIState), VMSTATE_END_OF_LIST()
  },
};

typedef unsigned char byte;
typedef unsigned short int uint16;
typedef unsigned int uint32;

static void mem_pci_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
  //lin void *pci_mem_addr;
  //lin int  temp, region_size;
  //lin byte buff[8];
  PCIMEMPCIState *dev = opaque;

/*
  pci_mem_addr = dev->mem_pci_base;
  pci_mem_addr = ((char *)pci_mem_addr) + addr;
  region_size = (int)memory_region_size(&dev->state.mem);
  if(addr > region_size)
    return ;
  fprintf(stderr,"%x\n",pci_mem_addr);
  switch (size) {
    case 1:
      sprintf(buff,"%02llx",value);
      sscanf(buff,"%x",&temp);
      *((byte*)pci_mem_addr)  = (byte)temp;
      break;
    case 2:
      sprintf(buff,"%04llx",value);
      sscanf(buff,"%x",&temp);
      *((uint16*)pci_mem_addr)= (uint16)temp;
      break;
    case 4:
      sprintf(buff,"%08llx",value);
      sscanf(buff,"%x",&temp);
      *((uint32*)pci_mem_addr)= (uint32)temp;
      break;
  }
  fprintf(stderr,"%x\n",temp);
*/
/*
  int bits = size * 8;
  printf("bits=%d\n",bits);
  int start_bit = (addr & 3) * 8;
  uint32_t mask = ((uint32_t)-1 >> (32 - bits)) << start_bit;
  dev->mem_pci_base &= ~mask;
  dev->mem_pci_base |= value << start_bit;
*/
  printf("write size=%d\n",(int)size);
  switch (size) {
    case 1:
      value = 17;
      memcpy(&dev->state.mem + 1, &value, size);
      break;
    case 2:
      value = 26;
      memcpy(&dev->state.mem + 2, &value, size);
      break;
  }
}

static uint64_t mem_pci_read(void *opaque, hwaddr addr, unsigned size)
{
  //lin void *pci_mem_addr;
  //lin int temp,region_size;
  //lin byte buff[8];
  uint64_t ret = 0;
  PCIMEMPCIState *dev = opaque;
/*
  pci_mem_addr = dev->mem_pci_base;
  pci_mem_addr = ((char *)pci_mem_addr) + addr;
  region_size = memory_region_size(&dev->state.mem);

  if(addr > region_size)
    return 0;
  switch (size) {
    case 1:
      temp = *((byte *)pci_mem_addr);
      return ((byte)temp);
    case 2:
      temp = *((uint16 *)pci_mem_addr);
      return ((uint16)temp);
    case 4:
      temp = *((uint32 *)pci_mem_addr);
      return ((uint32)temp);
  }
  //fprintf(stderr,"%d",temp);
*/
/*
    int bits = size * 8;
    int start_bit = (addr & 3) * 8;
    uint32_t mask = ((uint32_t)-1 >> (32 - bits)) << start_bit;
    return (dev->mem_pci_base & mask) >> start_bit;
*/
  printf("read: size=%d\n",(int)size);
  switch (size) {
    case 1:
      memcpy(&ret, &dev->state.mem + 1, size);
      break;
    case 2:
      memcpy(&ret, &dev->state.mem + 2, size);
      break;
  }
  return ret;
}

static const MemoryRegionOps mem_pci_ops = {
  .read = mem_pci_read,
  .write = mem_pci_write,
  .endianness = DEVICE_LITTLE_ENDIAN,
};

/* lin
static Property mem_pci_properties[] = {
  DEFINE_PROP_HEX32("membase", PCIMEMPCIState, mem_pci_base, 0xc0000000),
  DEFINE_PROP_END_OF_LIST()
};
*/

static int mem_pci_init(PCIDevice *dev)
{
  PCIMEMPCIState *pci = DO_UPCAST(PCIMEMPCIState, pci_dev, dev);
  MEMPCIState *s = &pci->state;

  //lin pci->mem_pci_base = (uint32_t)malloc(PCI_MEM_SIZE);
  memory_region_init_io(&s->mem, OBJECT(pci), &mem_pci_ops, pci, "mem-pci", PCI_MEM_SIZE);
  pci_register_bar(dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mem);
  return 0;
}

static void mem_pci_class_init(ObjectClass *klass, void *data)
{
  PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
  DeviceClass *dc = DEVICE_CLASS(klass);

  k->init = mem_pci_init;
  k->vendor_id = MEM_PCI_VENDOR_ID;
  k->device_id = MEM_PCI_DEVICE_ID;
  k->revision  = MEM_PCI_REVISION_ID;

  dc->vmsd = &vmstate_mem_pci;
  //lin dc->props = mem_pci_properties;
}

static TypeInfo mem_pci_info = {
  .name = "mem_pci",
  .parent = TYPE_PCI_DEVICE,
  .instance_size = sizeof(PCIMEMPCIState),
  .class_init = mem_pci_class_init,
};

static void mem_pci_register_types(void)
{
  type_register_static(&mem_pci_info);
}

type_init(mem_pci_register_types);
