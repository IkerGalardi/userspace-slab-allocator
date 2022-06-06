#!/bin/python

import sys
import numpy as np

file = open(sys.argv[1], "r")

STATE_MACHINE_BEFORE_PARENTHESIS = 0
STATE_MACHINE_INSIDE_PARENTHESIS = 1
STATE_MACHINE_AFTER_PARENTHESIS = 2

file_lines = file.readlines()
sizes = []
for line in file_lines:
    if "malloc" in line:
        state_machine = STATE_MACHINE_BEFORE_PARENTHESIS
        size_string = ""
        for c in line:
            if state_machine == STATE_MACHINE_BEFORE_PARENTHESIS:
                if c == '(':
                    state_machine = STATE_MACHINE_INSIDE_PARENTHESIS

            elif state_machine == STATE_MACHINE_INSIDE_PARENTHESIS:
                if c == ')':
                    state_machine = STATE_MACHINE_AFTER_PARENTHESIS
                    continue
                
                size_string += c

            elif state_machine == STATE_MACHINE_AFTER_PARENTHESIS:
                break

        try:    
            sizes.append(int(size_string))
        except:
            pass

nparray = np.array(sizes)
print( "Allocation size data:")
print(f"  · Minimum size = {np.amin(nparray)}")
print(f"  · Maximum size = {np.amax(nparray)}")
print(f"  · Mean = {np.mean(nparray)}")
print(f"  · Median = {np.median(nparray)}")
