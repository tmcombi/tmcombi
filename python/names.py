#! /usr/bin/env python3
import unittest
import re


class Feature:
    def __init__(self):
        self.name = 'target'
        self.type = ''
        self.values = []

    def dump(self):
        print(self.name + ': ', end='')
        if self.type == 'categorical':
            print(', '.join(self.values), end='')
            print('.')
        else:
            print(self.type + '.')


class Names:
    def __init__(self):
        self.feature = {}
        self.feature_list = []
        self.target_feature = 'target'

    def size(self):
        return len(self.feature_list)

    def dump(self):
        print(self.target_feature + '. | the target attribute')
        for feature_name in self.feature_list:
            self.feature[feature_name].dump()

    def process_line(line):
        empty = True
        feature = Feature()
        line = re.sub("\n", "", line)
        line = re.sub("[ ]*\|.*", "", line)
        line = re.sub("[\. ]*$", "", line)
        line = re.sub("^[ ]*", "", line)
        if line == '':
            return empty, feature
        empty = False
        data = re.split(":", line, 1)
        data[0] = re.sub("[ ]*$", "", data[0])
        if re.search(",", data[0]):
            data.append(data[0])
        else:
            feature.name = data[0]
            if len(data) < 2:
                return empty, feature
        data[1] = re.sub("^[ ]*", "", data[1])
        if data[1] == '':
            return empty, feature
        if data[1] in ['continuous', 'ignore', 'label']:
            feature.type = data[1]
            return empty, feature
        feature.type = 'categorical'
        for value in re.split(",", data[1]):
            value = re.sub("[ ]*$", "", value)
            value = re.sub("^[ ]*", "", value)
            feature.values.append(value)
        return empty, feature

    def from_file(self, file):
        fp = open(file, 'r')
        empty, target_feature = Names.process_line(fp.readline())
        while empty:
            empty, target_feature = Names.process_line(fp.readline())
            self.target_feature = target_feature.name
        line = fp.readline()
        while line:
            empty, feature = Names.process_line(line)
            if not empty:
                self.feature[feature.name] = feature
                self.feature_list.append(feature.name)
            line = fp.readline()
        fp.close()
        if self.target_feature not in self.feature_list:
            self.feature[self.target_feature] = target_feature
            self.feature_list.append(self.target_feature)
        return self


class TestNames(unittest.TestCase):

    def test_feature(self):
        f = Feature()
        f.name = 'testName'
        self.assertEqual(f.name, 'testName')

    def test_names_basic(self):
        N = Names()
        self.assertTrue(N.target_feature == 'target')
        self.assertFalse(N.size() < 0)

    def test_names_real_file(self):
        N = Names().from_file('adult.names')
        N.dump()
        self.assertFalse(0 > 0)


if __name__ == "__main__":
    unittest.main()
