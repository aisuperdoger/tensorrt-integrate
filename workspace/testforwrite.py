
# from matplotlib import lines
# import pickle
# import os
a = 4
b = 2
c = 3

with open("save.txt", "a") as f:
    f.write(str(a))
    f.write(' ')

    f.write(str(b))
    f.write(' ')
    
    f.write(str(c))
    f.write('\r\n')

# with open("save.txt") as f:
#     for item in f.readlines():
#         a, b, c = item.split()
#         print("a: ", a)
#         print("b: ", b)
#         print("c: ", c)
    

# f.close()
