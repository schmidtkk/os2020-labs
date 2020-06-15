unsigned char inb(unsigned short int port_from)
{
    unsigned char _in_value;
    __asm__ __volatile__ ("inb %w1,%0":"=a"(_in_value):"Nd"(port_from));
    return _in_value;
}

void outb (unsigned short int port_to, unsigned char value)
{
    __asm__ __volatile__ ("outb %b0,%w1"::"a" (value),"Nd" (port_to));
}

void io_wait(void)
{
    __asm__ __volatile__(   "jmp 1f\n\t"
                            "1:jmp 2f\n\t"
                            "2:" );
}