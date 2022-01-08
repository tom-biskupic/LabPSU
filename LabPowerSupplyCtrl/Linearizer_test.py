import unittest

from Linearizer import Linearizer

class LinearizerTest(unittest.TestCase):
    TEST_POINTS = [(0,0.1),(10,11.5),(20,22.5)]

    def test_code_to_value(self):
        fixture = Linearizer(self.TEST_POINTS)
        expected = (11.5-0.1)/(10)*5+0.1
        self.assertEqual(expected,fixture.code_to_value(5))

    def test_code_to_value_past_end(self):
        fixture = Linearizer(self.TEST_POINTS)
        expected = (22.5-11.5)/(20-10)*(25-10)+11.5
        self.assertEqual(expected,fixture.code_to_value(25))

    def test_value_to_code(self):
        fixture = Linearizer(self.TEST_POINTS)
        expected = round((20-10)/(22.5-11.5) * (12-11.5) + 10)
        self.assertEqual(expected,fixture.value_to_code(12))

    def test_value_to_code_before_first(self):
        fixture = Linearizer(self.TEST_POINTS)
        self.assertEqual(0,fixture.value_to_code(0.05))

if __name__ == '__main__':
    unittest.main()
