import csv
import sys



try:
    fileName = sys.argv[1]
except IndexError:
    print("Error: no filename given.")
    quit()

t = [] # column 0
data = [] # column 1

with open(fileName) as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data.append(float(row[1])) # second column

for i in range(len(t)):
    # print the data to verify it was read
    print(str(t[i]) + ", " + str(data[i]))

