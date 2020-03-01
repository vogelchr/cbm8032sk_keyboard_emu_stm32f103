#ifndef RINGBUF_H
#define RINGBUF_H

#include <string.h> /* memcpy */

static inline int
ringbuf_put(void *start, void *end, size_t elem_size, void * readp, void * volatile * writep, const void *data) {
        void *wp_old, *wp_new;

        wp_old = *writep;
        wp_new = wp_old + elem_size;

        if (wp_new == end)
                wp_new = start;

        if (wp_new == readp)
                return -1;

        memcpy(wp_old, data, elem_size);
        *writep = wp_new;
        return 0;
}

static inline int
ringbuf_get(void *start, void *end, size_t elem_size, void * volatile * readp, void * writep, void *data) {
        void *rp_old, *rp_new;

        rp_old = *readp;

        if (rp_old == writep)
                return -1;

        rp_new = rp_old + elem_size;
        if (rp_new == end)
                rp_new = start;

        memcpy(data, rp_old, elem_size);
        *readp = rp_new;
        return 0;
}


#define RINGBUF_END(buf) (((void *)(buf))+sizeof(buf)/sizeof((buf)[0]))
#define RINGBUF_PUT(buf, readp, writep, data) ringbuf_put(buf, RINGBUF_END(buf), sizeof(buf[0]), (void *)readp, (void * volatile *)&writep, data)
#define RINGBUF_GET(buf, readp, writep, data) ringbuf_get(buf, RINGBUF_END(buf), sizeof(buf[0]), (void * volatile *)&readp, (void *)writep, data)

#endif