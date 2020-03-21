// read byte from a certain port
unsigned char inb(unsigned short int port_from) {
	unsigned char c;
	__asm__ __volatile__("inb %w1,%b0":"=a"(c) : "Nd" (port_from));
	return c;

}
// write bypte to a certain port
void outb(unsigned short int port_to, unsigned char value) {
	__asm__ __volatile__("outb %b0,%w1"::"a" (value), "Nd" (port_to));
}