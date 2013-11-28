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

#include <alljoyn/audio/Mp3DataSource.h>

#include <qcc/Debug.h>
#include <qcc/Util.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#define QCC_MODULE "ALLJOYN_AUDIO"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace ajn {
namespace services {

Mp3DataSource::Mp3DataSource() : DataSource(), mInputFileMutex(new qcc::Mutex()), mInputFile(NULL) {
}

Mp3DataSource::~Mp3DataSource() {
    Close();
    delete mInputFileMutex;
}

bool Mp3DataSource::Open(FILE* inputFile) {
    if (mInputFile) {
        QCC_LogError(ER_FAIL, ("already open"));
        return false;
    }

    mInputFile = inputFile;
    mp3_decoder_t temp; // for total frames
    
    bytes_left = lseek(fileno(mInputFile), 0, SEEK_END);
    file_data = mmap(0, bytes_left, PROT_READ, MAP_PRIVATE, fileno(mInputFile), 0);
    //fclose(mInputFile);
    stream_pos = (unsigned char *) file_data;
    bytes_left -= 100;
    mp3 = mp3_create();
    frame_size = mp3_decode((void**)mp3, stream_pos, bytes_left, sample_buf, &info);
    
    if (!frame_size)
    {
        QCC_LogError(ER_FAIL, ("file is not an mp3 file"));
        Close();
        return false;
    }
    else
    {
        mSampleRate=info.sample_rate;
        mBitsPerChannel=16;
        mChannelsPerFrame=info.channels;
        mBytesPerFrame = (mBitsPerChannel >> 3) * mChannelsPerFrame;
        mInputDataStart=0;
        temp = mp3_create();
        
        mInputSize=mp3_total_frames((void**)temp, file_data, bytes_left+100, NULL, &info);
        printf("\nTotal size=%u\n", mInputSize);
        //add to queue
        initQueue(&wav_buffer, WAV_CIRCULAR_BUFFER_SIZE);
        addToQueue(&wav_buffer, (uint8_t *)sample_buf, info.audio_bytes);
    }

    {
        if( !(info.channels==2 || info.channels==1) ||
            !(info.sample_rate == 44100 || 
            info.sample_rate == 48000) )
        {
            QCC_LogError(ER_FAIL, ("file is not 44100|48000, 1|2"));
            Close();
            return false;
        }
    }

    return true;
}

bool Mp3DataSource::Open(const char* filePath) {
    if (mInputFile) {
        QCC_LogError(ER_FAIL, ("already open"));
        return false;
    }

    FILE*inputFile = fopen(filePath, "rb");
    if (inputFile == NULL) {
        QCC_LogError(ER_FAIL, ("can't open file '%s'", filePath));
        return false;
    }

    return Open(inputFile);
}

void Mp3DataSource::Close() {
    if (mInputFile != NULL) {
        fclose(mInputFile);
        mInputFile = NULL;
    }
}

size_t Mp3DataSource::ReadData(uint8_t* buffer, size_t offset, size_t length) {
    mInputFileMutex->Lock();
    size_t r = 0;
    if(mInputFile)
    {
        while( lengthQueue(&wav_buffer) < length )
        {
            //printf("ReadData length=%d lengthQueue=%d, adding to q\n", length, lengthQueue(&wav_buffer));
            stream_pos += frame_size;
            bytes_left -= frame_size;
            frame_size = mp3_decode((void**)mp3, stream_pos, bytes_left, sample_buf, &info);
            if(frame_size==0)
            {
                //printf("ReadData frame_size=0 break\n");
                break;
            }
            addToQueue(&wav_buffer, (uint8_t *)sample_buf, info.audio_bytes);
        }
        
        if( lengthQueue(&wav_buffer) >= length )
        {
            readFromQueue(&wav_buffer, buffer, length);
            r=length;
        }
        else
        {
            readFromQueue(&wav_buffer, buffer, lengthQueue(&wav_buffer));
            r=lengthQueue(&wav_buffer);
        }
    }
    mInputFileMutex->Unlock();

    //printf("Read offset %u     length %u      r %u\n", offset, length, r);

    return r;
}


int Mp3DataSource::initQueue(circular_buffer_t *q, unsigned int size)
{
    q->size=size;
    q->start=0;
    q->end=0;
    q->elems=(uint8_t *)malloc(size);
    return size;
}

unsigned int Mp3DataSource::lengthQueue(circular_buffer_t *q)
{
    unsigned int len;
    if( q->start <= q->end )
    {
        len=q->end - q->start;
    }
    else
    {
        len=q->size - q->start+q->end;
    }

    return len;
}

int Mp3DataSource::addToQueue(circular_buffer_t *q, uint8_t *src, unsigned int count)
{
    //printf("Addtoqueue count %d\n", count);
    // add to end, read from start
    // first case, start < end, normal
    if(q->start <= q->end)
    {
        // if there is enough space above end, till q->size, it fits
        if((q->size - q->end) > count) 
        {
            memcpy(q->elems + q->end, src, count);
            q->end+=count;
        }
        // half here half there
        else
        {
            // if total space left is enough
            if((q->size - q->end + q->start) >= count)
            {
                memcpy(q->elems + q->end, src, q->size - q->end);
                memcpy(q->elems, src + q->size - q->end, count-(q->size - q->end));
                q->end=count-(q->size - q->end);
            }
            else
            {
                return -1; //not enough space
            }
        }
    }
    // end is before start
    else
    {
        // if enough space
        if((q->start - q->end) >= count)
        {
            memcpy(q->elems + q->end, src, count);
            q->end+=count;
        }
        // not enough space
        else
        {
            return -1;
        }

    }

    if(q->end == q->size)
    {
        q->end = 0;
    }

    // to not let start and end cross ever. start==end menas empty list after a read from queue
    if(q->start==q->end)
    {
        q->start=0;
        q->end=0;
    }

    return count;
}

int Mp3DataSource::readFromQueue(circular_buffer_t *q, uint8_t *dst, unsigned int count)
{
    //printf("readFromQueue %d\n", count);
    // normal case, includes start
    if(q->start <= q->end)
    {
        // enough data
        if((q->end - q->start) >= count)
        {
            memcpy(dst, q->elems + q->start, count);
            q->start += count;
        }
        // else not enough space
        else
        {
            return -1;
        }
    }
    // else end before start
    else
    {
        // if enough data before warping back to start
        if( (q->size - q->start) >= count)
        {
            memcpy(dst, q->elems + q->start, count);
            q->start+=count;
        }
        else
        {
            // if total data is enough
            if( (q->size - q->start + q->end) >= count )
            {
                memcpy(dst, q->elems + q->start, q->size - q->start);
                memcpy(dst, q->elems, count - (q->size - q->start));
            }
        }
    }

    if(q->start == q->size)
    {
        q->start = 0;
    }

    // to not let start and end cross ever. start==end menas empty list after a read from queue
    if(q->start==q->end)
    {
        q->start=0;
        q->end=0;
    }

    return count;
}

}
}
