#ifndef FIXED_POINT_H
#define FIXED_POINT_H

typedef int fixed_t;

#define F (1 << 14)  // 17.14 fixed-point format

// Convert integer to fixed-point
#define INT_TO_FP(n) ((fixed_t)(n) * F)

// Convert fixed-point to integer (rounding toward zero)
#define FP_TO_INT(x) ((x) / F)

// Convert fixed-point to int (rounded to nearest)
#define FP_TO_INT_ROUND(x) (((x) >= 0) ? (((x) + F / 2) / F) : (((x) - F / 2) / F))

// Add/subtract
#define ADD_FP(x, y) ((x) + (y))
#define SUB_FP(x, y) ((x) - (y))
#define ADD_MIX(x, n) ((x) + (n) * F)
#define SUB_MIX(x, n) ((x) - (n) * F)

// Multiply
#define MULT_FP(x, y) ((fixed_t)(((int64_t)(x)) * (y) / F))
#define MULT_MIX(x, n) ((x) * (n))

// Divide
#define DIV_FP(x, y) ((fixed_t)(((int64_t)(x)) * F / (y)))
#define DIV_MIX(x, n) ((x) / (n))

#endif /* FIXED_POINT_H */