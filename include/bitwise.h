#ifndef BIT_WISE_H
#define BIT_WISE_H

// Macros bit operations
// #define _BV(bit) (1 << (bit))
#define set_bit(reg, bit) (reg |= _BV(bit))
#define clear_bit(reg, bit) (reg &= ~_BV(bit))
#define invert_bit(reg, bit) (reg ^= _BV(bit))
#define is_set_bit(reg, bit) (reg & _BV(bit))
#define copy_bit(src_reg, dst_reg, bit) (is_set_bit(src_reg, bit) ? set_bit(dst_reg, bit) : clear_bit(dst_reg, bit))

#endif // BIT_WISE_H
