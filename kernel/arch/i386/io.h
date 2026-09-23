#ifndef ARCH_I386_IO_H
#define ARCH_I386_IO_H

    void outb(unsigned short port, unsigned char value);
    unsigned char inb(unsigned short port);
    void io_wait();
