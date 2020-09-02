"""
https://leetcode.com/problems/valid-palindrome-ii/

Given a non-empty string s, you may delete at most one character. Judge whether you can make it a palindrome.

Example 1:
Input: "aba"
Output: True
Example 2:
Input: "abca"
Output: True
Explanation: You could delete the character 'c'.
Note:
The string will only contain lowercase characters a-z. The maximum length of the string is 50000.
"""

import unittest

class Solution:
    def validPalindrome(self, s: str) -> bool:
        for i in range(int(len(s) / 2)):
            if s[i] != s[- i - 1]:
                return Solution.is_pali(s, i + 1, len(s) - i - 1) or Solution.is_pali(s, i, len(s) - i - 2)

        return True

    @staticmethod
    def is_pali(str, i, j):
        return all(str[k] == str[j - (k - i)] for k in range(i, j))


class TestSo(unittest.TestCase):
    def test_pali(self):
        self.assertTrue(Solution().validPalindrome("abca"))
        self.assertFalse(Solution().validPalindrome("abcda"))
