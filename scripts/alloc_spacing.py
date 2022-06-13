#!/bin/python

import sys
import numpy as np

def parse_inside_parenthesis(line):
    if '(' in line and ')' in line:
        str_inside_par = line[line.find("(")+1:line.find(")")]
        return int(str_inside_par, 16)
    else:
        return 0

def find_ptr_in_list(list, ptr):
    for ptr_index_pair in list:
        if ptr_index_pair[0] == ptr:
            return ptr_index_pair
    print("DID NOT FIND")
    exit(4)


file = open(sys.argv[1], "r")
file_lines = file.readlines()

pointers = []
distances = []

i = 0
for line in file_lines:
    if "malloc" in line:
        splited = line.split('=')
        returned_addr = int(splited[-1].strip(), 16)
        pointers.append([returned_addr, i])
        i = i + 1
    elif "free" in line:
        ptr_int = parse_inside_parenthesis(line)
        if ptr_int == 0:
            continue

        malloc_i = find_ptr_in_list(pointers, ptr_int)[1]
        distances.append(i - malloc_i)
        print(distances[-1])

        i = i + 1

nparray = np.array(distances)
print( "Allocation distance data:")
print(f"  · Minimum size = {np.amin(nparray)}")
print(f"  · Maximum size = {np.amax(nparray)}")
print(f"  · Mean = {np.mean(nparray)}")
print(f"  · Median = {np.median(nparray)}")
