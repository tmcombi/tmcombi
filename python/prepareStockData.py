import argparse


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data", action="store",
                        dest="data",
                        help="data file")

    args = parser.parse_args()
    file = open(args.data, 'r')
    for line in file:
        line = line.strip().split(",")
        # print(line)
        # print(line[:-2])
        weight = float(line[-2])
        if line[-1] == "0":
            weight = - weight
        # print(weight)
        weight /= 2
        # print(weight)
        weight += 0.5
        pos_weight = min(max(  weight, 0), 1)
        neg_weight = min(max(1-weight, 0), 1)
        # print("{} / {}".format(neg_weight, pos_weight))
        print(",".join(line[:-2]) + "," + str(neg_weight) + ",0")
        print(",".join(line[:-2]) + "," + str(pos_weight) + ",1")
    file.close()
