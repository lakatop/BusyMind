/*
11. Container With Most Water
You are given an integer array height of length n. There are n vertical lines drawn such that the two endpoints of the ith line are (i, 0) and (i, height[i]).

Find two lines that together with the x-axis form a container, such that the container contains the most water.

Return the maximum amount of water a container can store.

Notice that you may not slant the container.



Example 1:
Input: height = [1,8,6,2,5,4,8,3,7]
Output: 49
Explanation: The above vertical lines are represented by array [1,8,6,2,5,4,8,3,7]. In this case, the max area of water (blue section) the container can contain is 49.

Example 2:
Input: height = [1,1]
Output: 1


Constraints:

n == height.length
2 <= n <= 105
0 <= height[i] <= 104
*/

#include <vector>

/// @brief Iterate over possible pairs and store the maximum
/// 1st solution iteration - O(n^2) time complexity
class SolutionMatrix
{
public:
    int MaxArea(const std::vector<int> &height) const
    {
        int maxArea = -1;
        for (int i = 0; i < height.size(); ++i)
        {
            for (int j = i + 1; j < height.size(); ++j)
            {
                // Calculate area from selected lines
                // Take minimum height (as that is maximum capacity to which we can fill water)
                const int area = std::min(height[i], height[j]) * (j - i);
                maxArea = maxArea < area ? area : maxArea;
            }
        }

        return maxArea;
    }
};

/// @brief Take 2 pointers (starting on the start and the end of the array)
/// Calculate area, move pointer that points to lower line (in case they equal, move pointer )
/// Repeat until pointers match
/// 2nd solution iteration - O(n) time complexity
class SolutionTwoPointer
{
public:
    int MaxArea(const std::vector<int> &height) const
    {
        std::size_t leftPointer = 0;
        std::size_t rightPointer = height.size() - 1;
        int maxArea = -1;

        // Iterate over array until pointers match
        while (leftPointer != rightPointer)
        {
            // Calculate area and max area so far
            const int area = std::min(height[leftPointer], height[rightPointer]) * (rightPointer - leftPointer);
            maxArea = maxArea < area ? area : maxArea;
            // Move pointers
            if (height[leftPointer] < height[rightPointer])
                ++leftPointer;
            else
                --rightPointer;
        }

        return maxArea;
    }
};

// --------------- TESTS --------------------
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("Test basic heights - dummy solution")
{
    SolutionMatrix s;
    const auto res = s.MaxArea({1, 8, 6, 2, 5, 4, 8, 3, 7});
    CHECK(res == 49);
}

TEST_CASE("Test basic heights - 2Pointer")
{
    SolutionTwoPointer s;
    const auto res = s.MaxArea({1, 8, 6, 2, 5, 4, 8, 3, 7});
    CHECK(res == 49);
}

TEST_CASE("Test 2 lines heights - dummy solution")
{
    SolutionMatrix s;
    const auto res = s.MaxArea({1, 1});
    CHECK(res == 1);
}

TEST_CASE("Test 2 lines heights - 2Pointer")
{
    SolutionTwoPointer s;
    const auto res = s.MaxArea({1, 1});
    CHECK(res == 1);
}

TEST_CASE("Test equal lines - dummy solution")
{
    SolutionMatrix s;
    const auto res = s.MaxArea({5, 5, 5, 5, 5, 5, 5, 5});
    CHECK(res == 35);
}

TEST_CASE("Test equal lines - 2Pointer")
{
    SolutionTwoPointer s;
    const auto res = s.MaxArea({5, 5, 5, 5, 5, 5, 5, 5});
    CHECK(res == 35);
}