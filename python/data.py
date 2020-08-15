import unittest
import re
import math
from names import Names


class Transform:
    def __init__(self, names):
        self.names = names
        self.rule_forward = {}
        self.rule_reverse = {}
        self.linear_forward_a = {}
        self.linear_reverse_a = {}
        self.linear_forward_b = {}
        self.linear_reverse_b = {}
        self.forward = {}

    def monotone_from_stat(self, stat):
        for feature_name in self.names.feature_list:
            feature = self.names.feature[feature_name]
            if feature.type == 'continuous':
                self.rule_forward[feature_name] = 'linear'
                self.rule_reverse[feature_name] = 'linear'
                self.linear_forward_b[feature_name] = 0
                self.linear_reverse_b[feature_name] = 0
                if stat.corr_with_target[feature_name] >= 0:
                    self.linear_forward_a[feature_name] = 1
                    self.linear_reverse_a[feature_name] = 1
                else:
                    self.linear_forward_a[feature_name] = -1
                    self.linear_reverse_a[feature_name] = -1
            if feature.type == 'categorical':
                self.rule_forward[feature_name] = 'cat2continuous'
                self.rule_reverse[feature_name] = 'continuous2cat'
                self.forward[feature_name] = {}
                for val in self.names.feature[feature_name].values:
                    self.forward[feature_name][val] = stat.category_goodness[feature_name][val]
                foo = 1



    def forward(self, vector):
        return 0

    def inverse(self):
        return 0


class Statistics:
    def __init__(self, names):
        self.names = names
        self.count = {}
        self.sum = {}
        self.squared_sum = {}
        self.times_target = {}
        self.mean = {}
        self.sigma = {}
        self.corr_with_target = {}
        self.count_vs_target = {}
        self.category_goodness = {}
        for feature_name in names.feature_list:
            self.count[feature_name] = 0
            if names.feature[feature_name].type == 'continuous' or feature_name == names.target_feature:
                self.sum[feature_name] = 0
                self.squared_sum[feature_name] = 0
                self.times_target[feature_name] = 0
            if names.feature[feature_name].type == 'categorical':
                self.count_vs_target[feature_name] = {}
                self.category_goodness[feature_name] = {}
                for cat in names.feature[feature_name].values:
                    self.count_vs_target[feature_name][cat] = {}
                    self.count_vs_target[feature_name][cat][0] = 0
                    self.count_vs_target[feature_name][cat][1] = 0
                    self.category_goodness[feature_name][cat] = {}

    def process_line(self, line, names, map_to_numeric):
        line = re.sub(r"\n", "", line)
        line = re.sub(r"[ ]*,[ ]*", ",", line)
        line = re.sub(r"[ ]*\|.*", "", line)
        line = re.sub(r"[\. ]*$", "", line)
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
                self.squared_sum[feature_name] += float(value) * float(value)
                self.times_target[feature_name] += float(value) * target_numeric
            if feature_name == names.target_feature:
                self.sum[feature_name] += target_numeric
                self.squared_sum[feature_name] += target_numeric * target_numeric
                self.times_target[feature_name] += target_numeric * target_numeric
            if names.feature[feature_name].type == 'categorical':
                self.count_vs_target[feature_name][value][target_numeric] += 1
        return line

    def process_file(self, file, names, map_to_numeric):
        fp = open(file, 'r')
        line = fp.readline()
        while line:
            line = self.process_line(line, names, map_to_numeric)
            print(line)
            line = fp.readline()
        fp.close()
        for feature_name in names.feature_list:
            if names.feature[feature_name].type == 'continuous' or feature_name == names.target_feature:
                self.mean[feature_name] = self.sum[feature_name] / self.count[feature_name]
                self.sigma[feature_name] = math.sqrt(self.squared_sum[feature_name] / self.count[feature_name]
                                                     - self.mean[feature_name]*self.mean[feature_name])
        for feature_name in names.feature_list:
            if names.feature[feature_name].type == 'continuous' or feature_name == names.target_feature:
                self.corr_with_target[feature_name] = (self.times_target[feature_name] / self.count[feature_name]
                                                       - self.mean[feature_name]*self.mean[names.target_feature]) \
                                                        / self.sigma[feature_name] \
                                                        / self.sigma[names.target_feature]
            if names.feature[feature_name].type == 'categorical':
                for cat in names.feature[feature_name].values:
                    if self.count_vs_target[feature_name][cat][0] + self.count_vs_target[feature_name][cat][1] == 0:
                        self.category_goodness[feature_name][cat] = 0
                    else:
                        self.category_goodness[feature_name][cat] = self.count_vs_target[feature_name][cat][1] /\
                                                                    (self.count_vs_target[feature_name][cat][0] +
                                                                     self.count_vs_target[feature_name][cat][1])


class TestData(unittest.TestCase):

    def test_names_real_file(self):
        names = Names().from_file('adult.names')
        self.assertEqual(names.size(), 15)
        # names = Names().from_file('../data/tmc_paper/tmc_paper.names')
        stat = Statistics(names)
        map_to_numeric = {}
        map_to_numeric[names.target_feature] = {}
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[0]] = 1
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[1]] = 0
        stat.process_file('adult.data', names, map_to_numeric)
        # stat.process_file('adult.test', names, map_to_numeric)
        # stat.process_file('../data/tmc_paper/tmc_paper.data', names, map_to_numeric)
        trans = Transform(names)
        trans.monotone_from_stat(stat)


if __name__ == "__main__":
    unittest.main()