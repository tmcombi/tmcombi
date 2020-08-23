import unittest
import re
import math
from names import Names


class Transform:
    def __init__(self, names):
        self.names = names
        self.rule = {}
        self.forward = {}
        self.backward = {}

    def monotone_from_stat(self, stat):
        for feature_name in self.names.feature_list:
            feature = self.names.feature[feature_name]
            self.forward[feature_name] = {}
            self.backward[feature_name] = {}
            if feature.type == 'continuous':
                self.rule[feature_name] = 'linear'
                self.forward[feature_name]['b'] = 0
                self.backward[feature_name]['b'] = 0
                if stat.corr_with_target[feature_name] >= 0:
                    self.forward[feature_name]['a'] = 1
                    self.backward[feature_name]['a'] = 1
                else:
                    self.forward[feature_name]['a'] = -1
                    self.backward[feature_name]['a'] = -1
                if abs(stat.corr_with_target[feature_name]) <= 0.1:
                    self.rule[feature_name] = 'ignore'
            if feature.type == 'categorical':
                self.rule[feature_name] = 'cat2continuous'
                for val in self.names.feature[feature_name].values:
                    self.forward[feature_name][val] = stat.category_goodness[feature_name][val]
                    self.backward[feature_name][stat.category_goodness[feature_name][val]] = val

    def transform_line(self, line):
        line = re.sub(r"\n", "", line)
        line = re.sub(r"[ ]*,[ ]*", ",", line)
        line = re.sub(r"[ ]*\|.*", "", line)
        line = re.sub(r"[\. ]*$", "", line)
        if line == '':
            return line
        data = re.split(",", line)
        data_out = []
        for feature_name, val in zip(self.names.feature_list, data):
            if val == '?':
                data_out.append('?')
                continue
            if self.rule[feature_name] == 'ignore':
                data_out.append(val)
                continue
            if self.rule[feature_name] == 'linear':
                val_out = self.forward[feature_name]['a']*float(val) + self.forward[feature_name]['b']
                if val_out == round(val_out):
                    val_out = round(val_out)
                data_out.append(str(val_out))
                continue
            if self.rule[feature_name] == 'cat2continuous':
                val_out = self.forward[feature_name][val]
                if val_out == round(val_out):
                    val_out = round(val_out)
                data_out.append(str(val_out))
                continue
            print('Transformation object does not have a rule for feature ' + feature_name)
            exit(-1)
        line_out = ','.join(data_out)
        return line_out

    def transform_file(self, file, file_out):
        fp = open(file, 'r')
        fp_out = open(file_out, 'w')
        line = fp.readline()
        while line:
            line_out = self.transform_line(line)
            line = fp.readline()
            fp_out.write(line_out + '\n')
        fp.close()
        fp_out.close()

    def print_names(self, out_file):
        fp = open(out_file, 'w')
        fp.write(self.names.target_feature + '.' + '\n')
        fp.write('\n')
        for feature_name in self.names.feature_list:
            if feature_name == self.names.target_feature:
                fp.write(feature_name + ': 0,1.' + '\n')
            elif self.rule[feature_name] == 'ignore':
                fp.write(feature_name + ': ignore.' + '\n')
            else:
                fp.write(feature_name + ': continuous.' + '\n')
        fp.close()

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
            self.process_line(line, names, map_to_numeric)
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
        stat = Statistics(names)
        map_to_numeric = {}
        map_to_numeric[names.target_feature] = {}
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[0]] = 1
        map_to_numeric[names.target_feature][names.feature[names.target_feature].values[1]] = 0
        stat.process_file('adult.data', names, map_to_numeric)
        trans = Transform(names)
        trans.monotone_from_stat(stat)
        trans.print_names('adult_transformed.names')
        trans.transform_file('adult.data', 'adult_transformed.data')
        trans.transform_file('adult.test', 'adult_transformed.test')


if __name__ == "__main__":
    unittest.main()
