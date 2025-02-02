/*
1752. Check if Array Is Sorted and Rotated

Given an array nums, return true if the array was originally sorted in non-decreasing order, then rotated some number of positions (including zero). Otherwise, return false.
There may be duplicates in the original array.
Note: An array A rotated by x positions results in an array B of the same length such that A[i] == B[(i+x) % A.length], where % is the modulo operation.


Example 1:
Input: nums = [3,4,5,1,2]
Output: true
Explanation: [1,2,3,4,5] is the original sorted array.
You can rotate the array by x = 3 positions to begin on the the element of value 3: [3,4,5,1,2].

Example 2:
Input: nums = [2,1,3,4]
Output: false
Explanation: There is no sorted array once rotated that can make nums.

Example 3:
Input: nums = [1,2,3]
Output: true
Explanation: [1,2,3] is the original sorted array.
You can rotate the array by x = 0 positions (i.e. no rotation) to make nums.


Constraints:

1 <= nums.length <= 100
1 <= nums[i] <= 100
*/

#include <vector>

class Solution
{
public:
    bool check(const std::vector<int> &nums)
    {
        if (nums.size() == 1 || nums.empty())
        {
            return true;
        }

        for (int i = 0; i < nums.size() - 1; i++)
        {
            if (nums[i] > nums[i + 1])
            {
                // Check that I already found breaking point previously
                // We cannot have 2 breaking points -> return false
                if (breakingPoint)
                {
                    return false;
                }
                else
                {
                    breakingPoint = true;
                }
            }
            // Else it is true that nums[i] <= nums[i+1]
            // => non-decreasing
        }

        if (breakingPoint)
        {
            // If there was a breaking point, check that
            // start of the array is continuing in non-decreasing order
            if (nums[0] >= nums[nums.size() - 1])
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    void Clear()
    {
        breakingPoint = false;
    }

private:
    bool breakingPoint = false;
};

// --------------- TESTS --------------------
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("Test basic increasing arrays")
{
    Solution s;
    CHECK(s.check({1, 2, 3}) == true);
    s.Clear();
    CHECK(s.check({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) == true);
}

TEST_CASE("Test corner cases on array size")
{
    Solution s;
    CHECK(s.check({}) == true); // empty array
    s.Clear();
    CHECK(s.check({1}) == true); // single element array
}

TEST_CASE("Test same element array")
{
    Solution s;
    CHECK(s.check({1, 1, 1, 1, 1, 1}) == true);
    s.Clear();
    CHECK(s.check({5, 5, 5, 5, 5}) == true);
}

TEST_CASE("Test non-decreasing array")
{
    Solution s;
    CHECK(s.check({1, 1, 2, 3, 3, 4}) == true);
    s.Clear();
    CHECK(s.check({5, 5, 5, 5, 5, 6, 6, 9, 15}) == true);
}

TEST_CASE("Test rotated arrays")
{
    Solution s;
    CHECK(s.check({3, 3, 4, 1, 1, 2}) == true);
    s.Clear();
    CHECK(s.check({5, 5, 6, 6, 9, 15, 5, 5, 5}) == true);
}

TEST_CASE("Test negative results")
{
    Solution s;
    CHECK(s.check({1, 3, 4, 1, 1, 2}) == false);
    s.Clear();
    CHECK(s.check({5, 5, 6, 6, 9, 15, 5, 6, 5}) == false);
}