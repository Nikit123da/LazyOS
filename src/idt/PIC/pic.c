#include "pic.h"
#include "../../IO/io.h"
#include "../../drivers/VGA/VGA.h"
void PIC_remap(int offset1) {

  print("Remaping PIC...\n");
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  outb(PIC1_DATA, offset1);          // ICW2: Master PIC vector offset
  outb(PIC2_DATA, offset1 + 8);      // ICW2: Slave PIC vector offset
  outb(PIC1_DATA, 1 << CASCADE_IRQ); // ICW3: tell Master PIC that there is a
                                     // slave PIC at IRQ2
  outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
  outb(PIC1_DATA,
       ICW4_8086); // ICW4: have the PICs use 8086 mode (and not 8080 mode)
  outb(PIC2_DATA, ICW4_8086);

  // Unmask both PICs.
  outb(PIC1_DATA, 0);
  outb(PIC2_DATA, 0);
  print("PIC has been remapped\n");
}
