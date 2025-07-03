#ifndef TUPLE_H
#define TUPLE_H

// ─── Anonymous N-tuple definitions (up to 12) ──────────────────────────────

// 2-tuple
#define Tuple2(K0, K1)                                                         \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
  }
#define make_tuple2(a0, a1) ((Tuple2(typeof(a0), typeof(a1))){(a0), (a1)})

// 3-tuple
#define Tuple3(K0, K1, K2)                                                     \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
  }
#define make_tuple3(a0, a1, a2)                                                \
  ((Tuple3(typeof(a0), typeof(a1), typeof(a2))){(a0), (a1), (a2)})

// 4-tuple
#define Tuple4(K0, K1, K2, K3)                                                 \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
  }
#define make_tuple4(a0, a1, a2, a3)                                            \
  ((Tuple4(typeof(a0), typeof(a1), typeof(a2), typeof(a3))){(a0), (a1), (a2),  \
                                                            (a3)})

// 5-tuple
#define Tuple5(K0, K1, K2, K3, K4)                                             \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
  }
#define make_tuple5(a0, a1, a2, a3, a4)                                        \
  ((Tuple5(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4))){       \
      (a0), (a1), (a2), (a3), (a4)})

// 6-tuple
#define Tuple6(K0, K1, K2, K3, K4, K5)                                         \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
  }
#define make_tuple6(a0, a1, a2, a3, a4, a5)                                    \
  ((Tuple6(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),         \
           typeof(a5))){(a0), (a1), (a2), (a3), (a4), (a5)})

// 7-tuple
#define Tuple7(K0, K1, K2, K3, K4, K5, K6)                                     \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
  }
#define make_tuple7(a0, a1, a2, a3, a4, a5, a6)                                \
  ((Tuple7(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),         \
           typeof(a5), typeof(a6))){(a0), (a1), (a2), (a3), (a4), (a5), (a6)})

// 8-tuple
#define Tuple8(K0, K1, K2, K3, K4, K5, K6, K7)                                 \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
  }
#define make_tuple8(a0, a1, a2, a3, a4, a5, a6, a7)                            \
  ((Tuple8(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),         \
           typeof(a5), typeof(a6), typeof(a7))){(a0), (a1), (a2), (a3), (a4),  \
                                                (a5), (a6), (a7)})

// 9-tuple
#define Tuple9(K0, K1, K2, K3, K4, K5, K6, K7, K8)                             \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
  }
#define make_tuple9(a0, a1, a2, a3, a4, a5, a6, a7, a8)                        \
  ((Tuple9(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),         \
           typeof(a5), typeof(a6), typeof(a7),                                 \
           typeof(a8))){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8)})

// 10-tuple
#define Tuple10(K0, K1, K2, K3, K4, K5, K6, K7, K8, K9)                        \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
  }
#define make_tuple10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)                   \
  ((Tuple10(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),        \
            typeof(a5), typeof(a6), typeof(a7), typeof(a8), typeof(a9))){      \
      (a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8), (a9)})

// 11-tuple
#define Tuple11(K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, K10)                   \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
    K10 _10;                                                                   \
  }
#define make_tuple11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)              \
  ((Tuple11(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),        \
            typeof(a5), typeof(a6), typeof(a7), typeof(a8), typeof(a9),        \
            typeof(a10))){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7),      \
                          (a8), (a9), (a10)})

// 12-tuple
#define Tuple12(K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, K10, K11)              \
  struct {                                                                     \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
    K10 _10;                                                                   \
    K11 _11;                                                                   \
  }
#define make_tuple12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)         \
  ((Tuple12(typeof(a0), typeof(a1), typeof(a2), typeof(a3), typeof(a4),        \
            typeof(a5), typeof(a6), typeof(a7), typeof(a8), typeof(a9),        \
            typeof(a10), typeof(a11))){(a0), (a1), (a2), (a3), (a4), (a5),     \
                                       (a6), (a7), (a8), (a9), (a10), (a11)})

// shorthand 2-tuple creators
#define Tuple(K0, K1) Tuple2(K0, K1)
#define make_tuple(a0, a1) make_tuple2(a0, a1)

// access the i-th field (i must be literal 0…11)
#define tuple_get(t, i) ((t)._##i)

// ─── Named tuple definitions (up to 12) ────────────────────────────────────

// 2-tuple
#define DEFINE_TUPLE2(Name, K0, K1)                                            \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
  } Name;
#define MAKE_TUPLE2(Name, a0, a1) ((Name){(a0), (a1)})

// 3-tuple
#define DEFINE_TUPLE3(Name, K0, K1, K2)                                        \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
  } Name;
#define MAKE_TUPLE3(Name, a0, a1, a2) ((Name){(a0), (a1), (a2)})

// 4-tuple
#define DEFINE_TUPLE4(Name, K0, K1, K2, K3)                                    \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
  } Name;
#define MAKE_TUPLE4(Name, a0, a1, a2, a3) ((Name){(a0), (a1), (a2), (a3)})

// 5-tuple
#define DEFINE_TUPLE5(Name, K0, K1, K2, K3, K4)                                \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
  } Name;
#define MAKE_TUPLE5(Name, a0, a1, a2, a3, a4)                                  \
  ((Name){(a0), (a1), (a2), (a3), (a4)})

// 6-tuple
#define DEFINE_TUPLE6(Name, K0, K1, K2, K3, K4, K5)                            \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
  } Name;
#define MAKE_TUPLE6(Name, a0, a1, a2, a3, a4, a5)                              \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5)})

// 7-tuple
#define DEFINE_TUPLE7(Name, K0, K1, K2, K3, K4, K5, K6)                        \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
  } Name;
#define MAKE_TUPLE7(Name, a0, a1, a2, a3, a4, a5, a6)                          \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6)})

// 8-tuple
#define DEFINE_TUPLE8(Name, K0, K1, K2, K3, K4, K5, K6, K7)                    \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
  } Name;
#define MAKE_TUPLE8(Name, a0, a1, a2, a3, a4, a5, a6, a7)                      \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7)})

// 9-tuple
#define DEFINE_TUPLE9(Name, K0, K1, K2, K3, K4, K5, K6, K7, K8)                \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
  } Name;
#define MAKE_TUPLE9(Name, a0, a1, a2, a3, a4, a5, a6, a7, a8)                  \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8)})

// 10-tuple
#define DEFINE_TUPLE10(Name, K0, K1, K2, K3, K4, K5, K6, K7, K8, K9)           \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
  } Name;
#define MAKE_TUPLE10(Name, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)             \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8), (a9)})

// 11-tuple
#define DEFINE_TUPLE11(Name, K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, K10)      \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
    K10 _10;                                                                   \
  } Name;
#define MAKE_TUPLE11(Name, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)        \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8), (a9), (a10)})

// 12-tuple
#define DEFINE_TUPLE12(Name, K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, K10, K11) \
  typedef struct Name {                                                        \
    K0 _0;                                                                     \
    K1 _1;                                                                     \
    K2 _2;                                                                     \
    K3 _3;                                                                     \
    K4 _4;                                                                     \
    K5 _5;                                                                     \
    K6 _6;                                                                     \
    K7 _7;                                                                     \
    K8 _8;                                                                     \
    K9 _9;                                                                     \
    K10 _10;                                                                   \
    K11 _11;                                                                   \
  } Name;
#define MAKE_TUPLE12(Name, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)   \
  ((Name){(a0), (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8), (a9), (a10),   \
          (a11)})

#endif // TUPLE_H
