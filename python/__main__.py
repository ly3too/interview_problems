import unittest
import os


if __name__ == '__main__':
    suit = unittest.TestLoader().discover(os.path.dirname(__file__), "*.py")
    unittest.TextTestRunner().run(suit)
