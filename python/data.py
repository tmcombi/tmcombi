import unittest
import re
from names import Names


class Statistics:
    def __init__(self, names):
        self.names = names
        self.count = {}
        self.sum = {}
        self.squared_sum = {}
        for feature_name in names.feature_list:
            self.count[feature_name] = 0
            if names.feature[feature_name].type == 'continuous':
                self.sum[feature_name] = 0
                self.squared_sum[feature_name] = 0

    def process_line(self, line, names):
        line = re.sub(r"\n", "", line)
        line = re.sub(r"[ ]*,[ ]*", ",", line)
        data = re.split(",", line)
        for (feature_name, value) in zip(names.feature_list, data):
            if value == '?':
                continue
            self.count[feature_name] += 1
            if names.feature[feature_name].type == 'continuous':
                self.sum[feature_name] += float(value)
                self.squared_sum[feature_name] += float(value)*float(value)
        return line

    def process_file(self, file, names):
        fp = open(file, 'r')
        line = fp.readline()
        while line:
            line = self.process_line(line, names)
            print(line)
            line = fp.readline()
        fp.close()


class TestNames(unittest.TestCase):

    def test_names_real_file(self):
        names = Names().from_file('adult.names')
        self.assertEqual(names.size(), 15)
        stat = Statistics(names)
        map_to_number = {}
        map_to_number[names.target_feature] = {}
        map_to_number[names.target_feature][names.feature[names.target_feature].values[0]] = 0
        map_to_number[names.target_feature][names.feature[names.target_feature].values[1]] = 1
        stat.process_file('adult.data', names)


if __name__ == "__main__":
    unittest.main()