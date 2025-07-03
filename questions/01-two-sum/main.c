#include "../../std/all.h"

DEFINE_TUPLE2(IntPair, int, int);
DEFINE_OPTION(IntPair);

static Option(IntPair) two_sum(Vec(int) nums, int target) {
  Map(int, int) m;
  map_init(m, 16);

  Option(IntPair) answer = OPTION_NONE(IntPair);
  vec_enum(i, num, nums) {
    int *entry = map_get(m, target - *num);
    if (entry) {
      answer = OPTION_SOME(IntPair, MAKE_TUPLE2(IntPair, *entry, i));
      break;
    }
    map_put(m, *num, i);
  }

  map_free(m);
  return answer;
}

int main(void) {
  TEST("simple test with duplicates") {
    Vec(int) nums;
    vec_init(nums, (int[]){4, 4, 9, 15});

    auto target = 8;
    auto answer = two_sum(nums, target);
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IntPair, 0, 1));
    vec_free(nums);
  }

  TEST("test without duplicates") {
    Vec(int) nums;
    vec_init(nums, (int[]){2, 7, 9, 15});

    auto target = 9;
    auto answer = two_sum(nums, target);
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IntPair, 0, 1));
    vec_free(nums);
  }

  TEST_SUMMARY();
}
