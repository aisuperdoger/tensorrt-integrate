
#include <opencv2/opencv.hpp>
#include <ffhdd/simple-logger.hpp>
#include <ffhdd/ffmpeg-demuxer.hpp>
#include <ffhdd/cuvid-decoder.hpp>
#include <ffhdd/nalu.hpp>
#include <string>

using namespace std;

static void test_hard_decode()
{

    auto demuxer = FFHDDemuxer::create_ffmpeg_demuxer("rtsp://admin:a1234567@ten01.adapdo.com:6002/Streaming/Channels/102");
    if (demuxer == nullptr)
    {
        INFOE("demuxer create failed");
        return;
    }

    auto decoder = FFHDDecoder::create_cuvid_decoder(
        false, FFHDDecoder::ffmpeg2NvCodecId(demuxer->get_video_codec()), -1, 0);

    if (decoder == nullptr)
    {
        INFOE("decoder create failed");
        return;
    }

    uint8_t *packet_data = nullptr;
    int packet_size = 0;
    uint64_t demuxpts = 0;
    uint64_t pts;
    uint8_t *pdata = nullptr;
    char packet_size_buffer[1024] = {0};

    demuxer->get_extra_data(&packet_data, &packet_size);

    decoder->decode(packet_data, packet_size);

    // int i = 0;
    // do
    // {
    //     i++;

    //     demuxer->demux(&packet_data, &packet_size, &demuxpts);
    //     if (i % 3 == 0)
    //     {
    //         demuxpts = (0x01ul << 60) | demuxpts;
    //     }
    //     else if (i % 3 == 1)
    //     {
    //         demuxpts = (0x02ul << 60) | demuxpts;
    //     }
    //     else if (i % 3 == 2)
    //     {
    //         demuxpts = (0x03ul << 60) | demuxpts;
    //     }

    //     if (packet_size == 0)
    //         demuxpts = 0;
    //     INFO("demux pts : %ld", demuxpts);
    //     INFO("demux size : %ld", packet_size);
    //     INFO("demux data : %ld", packet_data);

    //     FILE *outfp = fopen("output.h264", "ab+");
    //     FILE *outsz = fopen("outputsize.txt", "a+");
    //     FILE *outpts = fopen("outputpts.txt", "a+");

    //     // 所有信息写入文件
    //     // buffer_size[0] = packet_size;
    //     // buffer_pts[0] = demuxpts;

    //     size_t size = fwrite(packet_data, 1, packet_size, outfp);
    //     size_t size_sz = fwrite(&packet_size, 1, sizeof(int), outsz);
    //     size_t size_pts = fwrite(&demuxpts, 1, sizeof(uint64_t), outpts);
    //     if (size != packet_size)
    //     {
    //         printf("fwrite failed-> write:%u, pkt_size:%u\n", size, packet_size);
    //     }
    //     fclose(outfp);
    //     fclose(outsz);
    //     fclose(outpts);


    // } while (i < 50);

    int i = 50;
    FILE *outfp_ = NULL;
    outfp_ = fopen("output.h264", "rb");
    FILE *outsz_ = fopen("outputsize.txt", "r");
    FILE *outpts_ = fopen("outputpts.txt", "r");

    if (outfp_ == NULL)
    {
        printf("nothing get in file---------");
    }

    do
    {
        i--;

        // FILE *outfp_ = NULL;
        // outfp_ = fopen("output.h264", "rb");
        // FILE *outsz_ = fopen("outputsize.txt", "r");
        // FILE *outpts_ = fopen("outputpts.txt", "r");

        // if(outfp_ == NULL){
        //     printf("nothing get in file---------");
        // }

        fread(&packet_size, 1, sizeof(int), outsz_);
        cout << "packet_size:" << packet_size << endl;
        fread(&demuxpts, 1, sizeof(uint64_t), outpts_);
        cout << "time_pts:" << demuxpts << endl;
        pdata = new uint8_t[packet_size];
        size_t nnn = fread(pdata, 1, packet_size, outfp_);
        cout << "packet_data:" << pdata << endl;
        cout << "大小：" << nnn << endl;

        int ndecoded_frame = decoder->decode(pdata, packet_size, demuxpts);

        for (int i = 0; i < ndecoded_frame; ++i)
        {
            unsigned int frame_index = 0;

            /* 因为decoder获取的frame内存，是YUV-NV12格式的。储存内存大小是 [height * 1.5] * width byte
            因此构造一个height * 1.5,  width 大小的空间
            然后由opencv函数，把YUV-NV12转换到BGR，转换后的image则是正常的height, width, CV_8UC3
            */
            // cv::Mat image(decoder->get_height() * 1.5, decoder->get_width(), CV_8U, decoder->get_frame(&pts, &frame_index));
            cv::Mat image(decoder->get_height(), decoder->get_width(), CV_8UC3, decoder->get_frame(&pts, &frame_index));
            // cv::Mat image(decoder->get_height(), decoder->get_width(), CV_8UC3);
            // cv::cvtColor(image, image, cv::COLOR_YUV2BGR_NV12);

            // decoder->get_frame(&pts, &frame_index);

            frame_index = frame_index + 1;
            INFO("write imgs/img_%05d.jpg  %dx%d", frame_index, image.cols, image.rows);
            cv::imwrite(cv::format("imgs/img_%06d.jpg", frame_index), image);
            INFO("%d, %ld", frame_index + 1, pts);
        }
        delete (pdata);
    } while (i >= 0);

    fclose(outfp_);
    fclose(outsz_);
    fclose(outpts_);
}

int app_hard_decode()
{

    test_hard_decode();
    return 0;
}