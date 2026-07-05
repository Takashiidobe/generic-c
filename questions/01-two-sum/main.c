#include "../../std/all.h"

DEFINE_TUPLE2(IndexPair, size_t, size_t)
DEFINE_OPTION(IndexPair)

static Option(IndexPair) two_sum(Vec(int) nums, int target) {
  Map(int, size_t) m;
  map_init(m, 16);

  Option(IndexPair) answer = OPTION_NONE(IndexPair);
  vec_enum(i, num, nums) {
    size_t *entry = map_get(m, target - *num);
    if (entry) {
      answer = OPTION_SOME(IndexPair, MAKE_TUPLE2(IndexPair, *entry, i));
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
    ASSERT(option_is_some(answer));
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IndexPair, 0, 1));
    vec_free(nums);
  }

  TEST("test without duplicates") {
    Vec(int) nums;
    vec_init(nums, (int[]){2, 7, 9, 15});

    auto target = 9;
    auto answer = two_sum(nums, target);
    ASSERT(option_is_some(answer));
    ASSERT_EQ(answer.as.some, MAKE_TUPLE2(IndexPair, 0, 1));
    vec_free(nums);
  }

  TEST("test without a solution") {
    Vec(int) nums;
    vec_init(nums, (int[]){1, 2, 3});

    auto answer = two_sum(nums, 10);
    ASSERT(option_is_none(answer));
    vec_free(nums);
  }

  TEST_SUMMARY();
}
