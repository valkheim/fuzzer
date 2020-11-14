# break program after input has been loaded into memory
break print_input

# run demo program
run ../data/initial.bin

# get input location
print input.data

# get code coverage checkpoints
print check1
print check2
print check3
print vulnerable

# get last checkpoint
print 'exit@plt'
