
#ifdef uint32_t
typedef uint32_t crc_t;
#else
typedef unsigned long crc_t;
#endif
static crc_t crc_table[256];
static int crc_table_computed = 0;
static void make_crc_table(void)
{
    crc_t c;
    int n, k;
    for (n = 0; n < 256; n++) {
        c = (crc_t) n;
        for (k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xedb88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}
static crc_t update_crc(crc_t crc, const unsigned char *buf, int len)
{
    crc_t c = crc;
    int n;
    if (!crc_table_computed) {
        make_crc_table();
    }
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}
static crc_t crc(const unsigned char *buf, int len)
{
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}