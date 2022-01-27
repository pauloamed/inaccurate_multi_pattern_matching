# 1-inaccurate multi pattern matching

Given a set of patterns and a text, find occourences of such patterns inside the text with at most 1 error. 
An error is defined as an inaccurate char. 

The proposed solution combines Aho-chorasick with Sack (union by size on trees).

## Testing
Execute `make_test.py` for test creation. Compile both `aho.cpp` and `z.cpp` scripts. 
The first one is the proposed solution whereas the second is a solution using Z-algorithm, know to be correct.
Generete a test file, execute the scripts using such file as input and compare their outputs.
