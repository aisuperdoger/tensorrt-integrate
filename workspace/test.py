
import struct
from builtins import bytearray
import libffhdd as ffhdd
import cv2
import numpy as np
import ctypes
import os



demuxer = ffhdd.FFmpegDemuxer("/home/xd2/tensorrt-integrate/workspace/exp/number100.mp4")
# rtsp://admin:a1234567@ten01.adapdo.com:6002/Streaming/Channels/102
# /home/xd2/tensorrt-integrate/workspace/exp/number100.mp4
if not demuxer.valid:
    print("Load failed")
    exit(0)

output_type = ffhdd.FrameType.BGR
decoder = ffhdd.CUVIDDecoder(
    bUseDeviceFrame=False,
    codec=demuxer.get_video_codec(),
    max_cache=-1,
    gpu_id=0,
    output_type=output_type
)
if not decoder.valid:
    print("Decoder failed")
    exit(0)

pps, size = demuxer.get_extra_data()

decoder.decode(pps, size, 0)
os.makedirs("imgs", exist_ok=True)

nframe = 0
flag =0
breakflag=1
while True:
    pdata, pbytes, time_pts, iskey, ok = demuxer.demux()
    # print("pdata:",pdata)
    # nframe_decoded = decoder.decode(pdata, pbytes, time_pts)

    # print("nframe_decoded:", nframe_decoded)
    # for i in range(nframe_decoded):
    #     image, pts, idx = decoder.get_numpy()
    #     nframe += 1

    #     if output_type == ffhdd.FrameType.YUV_NV12:
    #         image = cv2.cvtColor(image, cv2.COLOR_YUV2BGR_NV12)

    #     print(image.shape)
    #     cv2.imwrite(f"imgs/data_{nframe:05d}.jpg", image)
    # buf = bytearray(pbytes)
    get_value = ctypes.cast(pdata, ctypes.py_object).value
    print('error print')
    # print("value type: ", type(get_value))
    if flag<100:
        flag+=1
        with open("save.bin", "wb") as f:
            print("-----------")
            print("pdata: ",pdata)
            print("value: ",get_value)
            save_value = struct.pack('B', get_value)
            # f.write(save_value)
                # f.write(' ')

                # f.write(str(time_pts))
                # f.write('\r\n')
    # if flag == 100:
    #     with open("save.txt") as f:
    #         for item in f.readlines():
    #             pdata, pbytes, time_pts = item.split()
    #             pdata, pbytes, time_pts = int(pdata),int(pbytes),int(time_pts)
                
    #             nframe_decoded = decoder.decode(pdata, pbytes, time_pts)

    #             print("nframe_decoded:", nframe_decoded)
    #             for i in range(nframe_decoded):
    #                 image, pts, idx = decoder.get_numpy()
    #                 nframe += 1

    #                 if output_type == ffhdd.FrameType.YUV_NV12:
    #                     image = cv2.cvtColor(image, cv2.COLOR_YUV2BGR_NV12)

    #                 print(image.shape)
    #                 cv2.imwrite(f"imgs/data_{nframe:05d}.jpg", image)
    #     break
    if pbytes <= 0:
        break

print(f"Frame. {nframe}")
