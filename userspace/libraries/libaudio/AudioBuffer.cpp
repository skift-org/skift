#include <libaudio/AudioBuffer.h>

AudioBuffer::AudioBuffer()
{
    static int buffer_id_counter = 0;
    _id = buffer_id_counter++;
}
