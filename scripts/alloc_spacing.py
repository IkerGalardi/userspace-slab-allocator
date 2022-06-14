#!/bin/python

import sys
import numpy as np

MAX_ANALYSIS = 1000000

def parse_inside_parenthesis(line):
    if '(' in line and ')' in line:
        str_inside_par = line[line.find("(")+1:line.find(")")]

        # NOTE: this should not be needed, but programs like bash try to free
        #       NULL. So we have to take that into account
        if str_inside_par == 'nil':
            return 0

        return int(str_inside_par, 16)
    else:
        return 0

def find_ptr_in_list(list, ptr):
    for ptr_index_pair in list:
        if ptr_index_pair[0] == ptr:
            return ptr_index_pair

    # NOTE: this should not be needed, but programs like bash try
    #       to free memory not previously allocated with malloc
    #       (maybe allocated with strdup? maybe programming error?)
    return [-1, 0]
    exit(4)


file = open(sys.argv[1], "r")
file_lines = file.readlines()

pointers = []
distances = []

i = 0
for line in file_lines:
    if line.startswith("malloc"):
        splited = line.split('=')
        returned_addr = int(splited[-1].strip(), 16)
        pointers.append([returned_addr, i])
        i = i + 1
    elif line.startswith("free"):
        ptr_int = parse_inside_parenthesis(line)
        if ptr_int == 0:
            continue

        malloc_i = find_ptr_in_list(pointers, ptr_int)[1]

        # NOTE: this should not be needed, but programs like bash try
        #       to free memory not previously allocated with malloc
        #       (maybe allocated with strdup? maybe programming error?)
        if malloc_i == -1:
            continue
        distance = i - malloc_i
        distances.append(distance)

        i = i + 1

        if i > MAX_ANALYSIS:
            break

nparray = np.array(distances)
print( "Allocation distance data:")
print(f"  · Minimum size = {np.amin(nparray)}")
print(f"  · Maximum size = {np.amax(nparray)}")
print(f"  · Mean = {np.mean(nparray)}")
print(f"  · Median = {np.median(nparray)}")
