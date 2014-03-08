#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include "vmem_pci.h"

#define MEM_PCI_VENDOR_ID 0x1234
#define MEM_PCI_DEVICE_ID 0x5678
#define MEM_PCI_REVISION_ID 0x73

typedef unsigned char byte;
typedef unsigned short int uint16;
typedef unsigned int uint32;

static struct pci_device_id ids[] = {
  { PCI_DEVICE(MEM_PCI_VENDOR_ID, MEM_PCI_DEVICE_ID), },
  { 0, }
};

MODULE_DEVICE_TABLE(pci, ids);

static unsigned char skel_get_revision(struct pci_dev *dev)
{
  u8 revision;
  pci_read_config_byte(dev, PCI_REVISION_ID, &revision);
  return revision;
}

//return 0 means success
static int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  /* Do probing type stuff here. Like calling request_region(); */
  //lin: unsigned char revision_id;
  unsigned long len;
  void __iomem *addressio;
  int bar ;

  if(skel_get_revision(dev) != MEM_PCI_REVISION_ID)
    return 1;

  if (pci_enable_device(dev) < 0) {
    return 1;
  }
  bar = 1;
  //lin: resource_size_t start = pci_resource_start(dev, bar);
  len = pci_resource_len(dev, bar);
  //lin: unsigned long flags = pci_resource_flags(dev, bar);
  addressio = pci_iomap(dev,bar,len);
  if (addressio == NULL) {
    return 1;
  }
  //*(byte *)addressio = 0x57;
  //iowrite8(0x89,addressio + 8);
  //printk("%x\n",ioread8(addressio + 8));
  //printk("%x\n",*(byte *)addressio);
  iowrite8(0x89,addressio + 0);
  printk("%x\n",ioread8(addressio + 0));
  iowrite16(0x50,addressio + 8);
  printk("%x\n",ioread16(addressio + 8));
  printk("len=%ld\n",len);
  return 0;
}

static void remove(struct pci_dev *dev) {
  pci_disable_device(dev);
}
static struct pci_driver pci_driver = {
  .name = "mem_pci",
  .id_table = ids,
  .probe = probe,
  .remove = remove,
};

static int __init mem_pci_init(void) {
  printk("mem_pci loaded!\n");
  return pci_register_driver(&pci_driver);
}
static void __exit mem_pci_exit(void) {
  pci_unregister_driver(&pci_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gudujian");
module_init(mem_pci_init);
module_exit(mem_pci_exit);
