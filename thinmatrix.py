import numpy as np
import struct



def read_tm(tmfilepath):
    fh = open(tmfilepath, 'rb')
    rows = struct.unpack('L', fh.read(8))[0]
    cols = struct.unpack('L', fh.read(8))[0]
    buffer = fh.read(4*rows*cols)
    data = np.array(struct.unpack_from('f'*(len(buffer)/4), buffer)).reshape(rows,cols)
    return data
        
def write_tm(np_array, fname):
    fh = open(fname, 'wb')
    rowsbuff = struct.pack('L', np_array.shape[0])
    fh.write(rowsbuff)
    colsbuff = struct.pack('L', np_array.shape[1])
    fh.write(colsbuff)
    for x in np_array.flatten():
        buff = struct.pack('f', x)
        fh.write(buff)

    fh.close()
    
