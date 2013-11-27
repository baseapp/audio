/**
 * @file
 * Handle feeding input into the SinkPlayer
 */

/******************************************************************************
 * Copyright 2013, doubleTwist Corporation and Qualcomm Innovation Center, Inc.
 *
 *    All rights reserved.
 *    This file is licensed under the 3-clause BSD license in the NOTICE.txt
 *    file for this project. A copy of the 3-clause BSD license is found at:
 *
 *        http://opensource.org/licenses/BSD-3-Clause.
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the license is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the license for the specific language governing permissions and
 *    limitations under the license.
 ******************************************************************************/
/*  Modified Nov 2013 tavish */

#ifndef _MP3DATASOURCE_H_
#define _MP3DATASOURCE_H_

#ifndef __cplusplus
#error Only include Mp3DataSource.h in C++ code.
#endif

#include <stdio.h>
#include <alljoyn/audio/DataSource.h>
extern "C"
{
    #include <alljoyn/audio/minimp3.h>
}

#define WAV_CIRCULAR_BUFFER_SIZE 65536*2

typedef struct {
    int         size;   /* maximum number of elements           */
    int         start;  /* index of oldest element              */
    int         end;    /* index at which to write new element  */
    uint8_t   *elems;  /* vector of elements                   */
} circular_buffer_t;

namespace qcc { class Mutex; }

namespace ajn {
namespace services {

/**
 * A mp3 file data input source.
 */
class Mp3DataSource : public DataSource {
  public:
    Mp3DataSource();
    virtual ~Mp3DataSource();

    /**
     * Opens the file used to read data from.
     *
     * @param[in] inputFile the file pointer.
     *
     * @return true if open.
     */
    bool Open(FILE* inputFile);
    /**
     * Opens the file used to read data from.
     *
     * @param[in] filePath the file path.
     *
     * @return true if open.
     */
    bool Open(const char* filePath);
    /**
     * Closes the file used to read data from.
     */
    void Close();

    double GetSampleRate() { return mSampleRate; }
    uint32_t GetBytesPerFrame() { return mBytesPerFrame; }
    uint32_t GetChannelsPerFrame() { return mChannelsPerFrame; }
    uint32_t GetBitsPerChannel() { return mBitsPerChannel; }
    uint32_t GetInputSize() { return mInputSize; }

    size_t ReadData(uint8_t* buffer, size_t offset, size_t length);

    /**
     * Since we read ondemand from a file always return true that data is ready
     */
    bool IsDataReady() { return true; }

  private:
    bool ReadHeader();
    int addToQueue(circular_buffer_t *q, uint8_t *src, unsigned int count);
    int readFromQueue(circular_buffer_t *q, uint8_t *dst, unsigned int count);
    int initQueue(circular_buffer_t *q, unsigned int size);
    unsigned int lengthQueue(circular_buffer_t *q);

    double mSampleRate;
    uint32_t mBytesPerFrame;
    uint32_t mChannelsPerFrame;
    uint32_t mBitsPerChannel;
    uint32_t mInputSize;
    uint32_t mInputDataStart;
    qcc::Mutex* mInputFileMutex;
    FILE* mInputFile;
    
    mp3_decoder_t mp3;
    mp3_info_t info;
    void *file_data;
    unsigned char *stream_pos;
    /*
        circular buffer(malloced) to allow for 
    */
    circular_buffer_t wav_buffer;


    int16_t sample_buf[MP3_MAX_SAMPLES_PER_FRAME];
    int bytes_left;
    int frame_size;
    int value;
};

}
}

#endif //_MP3DATASOURCE_H_
