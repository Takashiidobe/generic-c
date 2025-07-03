#include "../array/vector.h"
#include "../hashmap/hashmap.h"
#include "../option/option.h"
#include "../tuple/tuple.h"
#include <stdio.h>

typedef typeof(make_tuple(0, 0)) IntPair;
DEFINE_OPTION(IntPair);

#define auto __auto_type

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
  Vec(int) nums = NULL;
  vec_push(nums, 2);
  vec_push(nums, 7);
  vec_push(nums, 9);
  vec_push(nums, 15);

  int target = 9;
  auto answer = two_sum(nums, target);

  if (option_is_some(answer)) {
    auto res = unwrap_some(answer);
    printf("Indexes: (%d, %d), Values: (%d, %d)\n", tuple_get(res, 0),
           tuple_get(res, 1), vec_get(nums, tuple_get(res, 0)),
           vec_get(nums, tuple_get(res, 1)));
  }
  return 0;
}
