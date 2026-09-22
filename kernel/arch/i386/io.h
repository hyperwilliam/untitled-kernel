#ifdef __cplusplus
extern "C" {
    #endif

    void outb(unsigned short port, unsigned char value);
    unsigned char inb(unsigned short port);
    void io_wait();
    #ifdef __cplusplus
}
#endif
