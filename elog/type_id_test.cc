#include <typeinfo>
#include <gtest/gtest.h>
#include "type_id.h"

namespace LOG {

namespace {

class TypeIdTest : public ::testing::Test {
 protected:
  template <typename T>
  static void CheckEquivalenceOfTypeIdsWithBuiltinTypes() {
    CheckEquivalenceOfTypeIds<T, bool>();
    CheckEquivalenceOfTypeIds<T, char>();
    CheckEquivalenceOfTypeIds<T, unsigned char>();
    CheckEquivalenceOfTypeIds<T, short>();
    CheckEquivalenceOfTypeIds<T, unsigned short>();
    CheckEquivalenceOfTypeIds<T, int>();
    CheckEquivalenceOfTypeIds<T, unsigned int>();
    CheckEquivalenceOfTypeIds<T, long>();
    CheckEquivalenceOfTypeIds<T, unsigned long>();
    CheckEquivalenceOfTypeIds<T, long long>();
    CheckEquivalenceOfTypeIds<T, unsigned long long>();
    CheckEquivalenceOfTypeIds<T, float>();
    CheckEquivalenceOfTypeIds<T, double>();
    CheckEquivalenceOfTypeIds<T, long double>();
  }

  template <typename T, typename S>
  static void CheckEquivalenceOfTypeIdsInVariations() {
    CheckEquivalenceOfTypeIds<T, S>();
    CheckEquivalenceOfTypeIds<T, S*>();
    CheckEquivalenceOfTypeIds<T, const S>();
    CheckEquivalenceOfTypeIds<T, const S*>();
    CheckEquivalenceOfTypeIds<T, volatile S>();
    CheckEquivalenceOfTypeIds<T, volatile S*>();
    CheckEquivalenceOfTypeIds<T, const volatile S>();
    CheckEquivalenceOfTypeIds<T, const volatile S*>();
    CheckEquivalenceOfTypeIds<T, S ()>();
    CheckEquivalenceOfTypeIds<T, void (S)>();
    CheckEquivalenceOfTypeIds<T, void (S, S)>();
    CheckEquivalenceOfTypeIds<T, void (const S&)>();
    CheckEquivalenceOfTypeIds<T, void (const S&, const S&)>();
    CheckEquivalenceOfTypeIds<T, S (S)>();
    CheckEquivalenceOfTypeIds<T, S (S, S)>();
    CheckEquivalenceOfTypeIds<T, S (const S&)>();
    CheckEquivalenceOfTypeIds<T, S (const S&, const S&)>();
  }

  template <typename T, typename S>
  static void CheckEquivalenceOfTypeIds() {
    if (typeid(T) == typeid(S)) {
      EXPECT_TRUE(TypeId(Type<T>()) == TypeId(Type<S>()));
    } else {
      EXPECT_FALSE(TypeId(Type<T>()) == TypeId(Type<S>()));
    }
  }
};

class UserDefinedTypeA {};
class UserDefinedTypeB {};

}  // anonymous namespace

TEST_F(TypeIdTest, CheckEquivalenceOfTypeIdsOfBuiltinTypes) {
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<void>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<bool>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<char>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<unsigned char>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<short>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<unsigned short>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<int>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<unsigned int>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<long>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<unsigned long>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<long long>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<unsigned long long>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<float>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<double>();
  CheckEquivalenceOfTypeIdsWithBuiltinTypes<long double>();
}

TEST_F(TypeIdTest, CheckEquivalenceOfIntInVariations) {
  CheckEquivalenceOfTypeIdsInVariations<void, int>();
  CheckEquivalenceOfTypeIdsInVariations<char, int>();
  CheckEquivalenceOfTypeIdsInVariations<int, int>();
}

TEST_F(TypeIdTest, CheckEquivalenceOfUserDefinedTypes) {
  CheckEquivalenceOfTypeIdsInVariations<UserDefinedTypeA, UserDefinedTypeA>();
  CheckEquivalenceOfTypeIdsInVariations<UserDefinedTypeA, UserDefinedTypeB>();
}

}  // namespace LOG
