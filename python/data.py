import unittest
import re
from names import Names


class Statistics:
    def __init__(self, names):
        self.names = names
        self.count = {}
        self.sum = {}
        self.squared_sum = {}
        self.times_target = {}
        for feature_name in names.feature_list:
            self.count[feature_name] = 0
            if names.feature[feature_name].type == 'continuous':
                self.sum[feature_name] = 0
                self.squared_sum[feature_name] = 0
                self.times_target[feature_name] = 0

    def process_line(self, line, names, map_to_numeric):
        line = re.sub(r"\n", "", line)
        line = re.sub(r"[ ]*,[ ]*", ",", line)
        if line == '':
            return line
        data = re.split(",", line)
        target_value = data[names.target_index()]
        target_numeric = map_to_numeric[names.target_feature][target_value]
        for (feature_name, value) in zip(names.feature_list, data):
            if value == '?':
                continue
            self.count[feature_name] += 1
            if names.feature[feature_name].type == 'continuous':
                self.sum[feature_name] += float(value)
                self.squared_sum[feature_name] += float(value)*float(value)
                self.times_target[feature_name] += float(value) * target_numeric
        return line

    def process_file(self, file, names, map_to_numeric):
        fp = open(file, 'r')
        line = fp.readline()
        while line:
            line = self.process_line(line, names, map_to_numeric)
            print(line)
            line = fp.readline()
        fp.close()


class TestNames(unittest.TestCase):

    def test_names_real_file(self):
        names = Names().from_file('adult.names')
        self.assertEqual(names.size(), 15)
        stat = Statistics(names)
        map_to_numeric = {}
        map_to_numeric[names.target_feature] = {}
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[0]] = 1
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[1]] = 0
        stat.process_file('adult.data', names, map_to_numeric)


if __name__ == "__main__":
    unittest.main()