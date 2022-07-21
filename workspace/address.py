import ctypes
import struct
import struct
import sys
import _ctypes
 
value_='hello world' #定义一个字符串变量
address=id(value_) #获取value的地址，赋给address
print('address:', type(address))

# get_value=ctypes.cast(address, ctypes.py_object).value #读取地址中的变量
get_value = _ctypes.PyObj_FromPtr(address)
print(get_value)

# bytes = struct.pack('5s6sif', get_value)
# binfile = open('binfile.bin','wb')
# binfile.write(bytes)
