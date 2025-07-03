#include "../../std/all.h"

DEFINE_TUPLE2(IntPair, int, int);
DEFINE_OPTION(IntPair);

Option(IntPair) two_sum(Vec(int) nums, int target) {
  Map(int, int) m;
  map_init(m, 16);

  vec_enum(i, num, nums) {
    int *entry = map_get(m, target - *num);
    if (entry) {
      return OPTION_SOME(IntPair, MAKE_TUPLE2(IntPair, *entry, i));
    }
    map_put(m, *num, i);
  }

  return OPTION_NONE(IntPair);
}

int main(void) {
  TEST("simple test with duplicates") {
    Vec(int) nums;
    vec_init(nums, (int[]){4, 4, 9, 15});

    auto target = 8;
    auto answer = two_sum(nums, target);
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IntPair, 0, 1));
  }

  TEST("test without duplicates") {
    Vec(int) nums;
    vec_init(nums, (int[]){2, 7, 9, 15});

    auto target = 9;
    auto answer = two_sum(nums, target);
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IntPair, 0, 1));
  }

  TEST_SUMMARY();
}
