#include "kernel/drivers/AC97.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task.h"

#include <libsystem/Logger.h>

/*

Tasks

Done
- created driver class
- initialised constants
- allocated buffer memory
- initialised driver
- set max volume
- add logging to functions
- try to initialise driver in qemu
- implement write function that starts playing
- set sample rate in initialise
- handle interrupt and filebuffer memory 
- stop playing once buffer is complete 
- stop playing once call received

Left
- implement record using read
- implement volume control

*/

AC97::AC97(DeviceAddress address) : PCIDevice(address, DeviceClass::SOUND)
{
    // set values
    playback_volume_PCM = AC97_PCM_OUT_VOLUME;
    playback_volume_master = AC97_MASTER_VOLUME;

    read_buffer = new char *[AC97_BDL_BUFFER_LEN];

    // get native audio bus master bar and native audio mixer bar
    nabmbar = bar(1).base();
    nambar = bar(0).base();

    //  enable all device interrupts
    out8(nabmbar + AC97_PO_CR, AC97_X_CR_FEIE | AC97_X_CR_LVBIE | AC97_X_CR_IOCE);
    // out8(nabmbar + AC97_PO_CR, 0 << 4);
    pci_address().write16(PCI_COMMAND, 0x5);

    out8(nabmbar + 0x60, 0x02);
    // default the pcm output to full volume
    out16(nambar + AC97_PCM_OUT_VOLUME, 0x0000);

    initialise_buffers();

    // tell the ac97 where buffer descriptor list is
    out32(nabmbar + AC97_PO_BDBAR, buffer_descriptors_range->physical_base());

    // set last valid index
    lvi = 0;
    out8(nabmbar + AC97_PO_LVI, lvi);

    // detect wheter device supports MSB
    out32(nambar + AC97_MASTER_VOLUME, 0x2020);
    uint16_t t = in32(nambar + AC97_MASTER_VOLUME) & 0x1f;
    if (t == 0x1f)
    {
        logger_trace("This device only supports 5 bits of audio volume.");
        bits = 5;
        mask = 0x1f;
        out32(nambar + AC97_MASTER_VOLUME, 0x0f0f);
    }
    else
    {
        bits = 6;
        mask = 0x3f;
        out32(nambar + AC97_MASTER_VOLUME, 0x1f1f);
    }

    // Enable variable rate audio
    out16(nambar + AC97_EXT_AUDIO_STC, in16(nambar + AC97_EXT_AUDIO_STC) | 1);
    task_sleep(scheduler_running(), 10);

    // General Sample rate: 44100 Hz
    out16(nambar + AC97_FRONT_SPLRATE, AC97_PLAYBACK_SPEED);
    out16(nambar + AC97_LR_SPLRATE, AC97_PLAYBACK_SPEED);

    out8(nabmbar + AC97_PO_CR, in8(nabmbar + AC97_PO_CR) | AC97_X_CR_RPBM);

    logger_trace("AC97 initialised successfully");
}

void AC97::initialise_buffers()
{
    buffer_descriptors_range = make<MMIORange>(sizeof(AC97BufferDescriptor) * AC97_BDL_LEN);
    buffer_descriptors_list = reinterpret_cast<AC97BufferDescriptor *>(buffer_descriptors_range->base());

    for (size_t i = 0; i < AC97_BDL_LEN; i++)
    {
        buffers.push_back(make<MMIORange>(AC97_BDL_BUFFER_LEN));
        buffer_descriptors_list[i].pointer = buffers[i]->physical_base();
        AC97_CL_SET_LENGTH(buffer_descriptors_list[i].cl, 0);
        /* Set all buffers to interrupt */
        buffer_descriptors_list[i].cl |= AC97_CL_IOC;
    }
}

AC97::~AC97() { ; }

void AC97::handle_interrupt()
{
    // logger_trace("interrupt received");
    uint16_t sr = in16(nabmbar + AC97_PO_SR);
    if (!sr)
    {
        return;
    }

    if (sr & AC97_X_SR_BCIS)
    {

        logger_trace("interupt buffer finished playing");
        size_t size = _buffer.used();
        if (size >= AC97_BDL_BUFFER_LEN)
        {
            logger_trace("inhere %d", lvi);

            _buffer.read((char *)read_buffer, AC97_BDL_BUFFER_LEN);

            buffers[lvi]->write(0, read_buffer, AC97_BDL_BUFFER_LEN);
            AC97_CL_SET_LENGTH(buffer_descriptors_list[lvi].cl, AC97_BDL_BUFFER_LEN >> 1);
            buffer_descriptors_list[(lvi) % AC97_BDL_LEN].cl |= AC97_CL_IOC;

            out8(nabmbar + AC97_PO_LVI, lvi);
            out8(nabmbar + AC97_PO_CR, AC97_PO_LVI);
            lvi = (lvi + 2) % AC97_BDL_LEN;
        }
        else if (size > 0)
        {

            logger_trace("here %d %d", lvi, size);

            _buffer.read((char *)read_buffer, size);

            buffers[lvi]->write(0, read_buffer, size);

            AC97_CL_SET_LENGTH(buffer_descriptors_list[lvi].cl, (size - 2) >> 1);
            buffer_descriptors_list[(lvi) % AC97_BDL_LEN].cl |= (AC97_CL_IOC | AC97_CL_BUP);

            out32(nabmbar + AC97_PO_BDBAR, buffer_descriptors_range->physical_base());
            out8(nabmbar + AC97_PO_LVI, lvi);
            out8(nabmbar + AC97_PO_CR, AC97_PO_LVI);
            lvi = (lvi + 2) % AC97_BDL_LEN;
            // playing = false;
        }
        else
        {
            buffer_descriptors_list[lvi].cl |= AC97_CL_BUP;
            // The last valid buffer is this one
            logger_trace("here lvi %d", lvi);
            playing = false;
        }
    }
    else if (sr & AC97_X_SR_LVBCI)
    {
        logger_trace("ac97 irq is lvbci");
        // playing = false;
    }
    else if (sr & AC97_X_SR_FIFOE)
    {
        logger_trace("ac97 irq is fifoe");
    }
    else
    {
        /* don't handle it */
        return;
    }
    logger_trace("ac97 status register: %08x, %d", sr, sr);

    // clear interrupt bits
    out16(nabmbar + AC97_PO_SR, sr & 0x1E);

    return;
}

// ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) { ; };

bool AC97::can_write(FsHandle &handle)
{
    __unused(handle);
    return !_buffer.full();
}

ResultOr<size_t> AC97::write(FsHandle &handle, const void *buffer, size_t size)
{
    __unused(handle);

    size_t return_size = 0;

    if (!playing && (_buffer.used() <= 2 * AC97_BDL_BUFFER_LEN))
    {
        return_size = _buffer.write((char *)buffer, size);
        logger_trace("inital sound buffer fill");
    }
    else if (!playing && (_buffer.used() > 2 * AC97_BDL_BUFFER_LEN))
    {
        return_size = _buffer.write((char *)buffer, size);

        logger_trace("start playing buffer");
        _buffer.read((char *)read_buffer, AC97_BDL_BUFFER_LEN);
        buffers[(lvi) % AC97_BDL_LEN]->write(0, read_buffer, AC97_BDL_BUFFER_LEN);

        AC97_CL_SET_LENGTH(buffer_descriptors_list[(lvi) % AC97_BDL_LEN].cl, AC97_BDL_BUFFER_LEN >> 1);
        buffer_descriptors_list[lvi % AC97_BDL_LEN].cl |= AC97_CL_IOC;

        out32(nabmbar + AC97_PO_BDBAR, buffer_descriptors_range->physical_base());
        out8(nabmbar + AC97_PO_LVI, (lvi) % AC97_BDL_LEN);
        out8(nabmbar + AC97_PO_CR, AC97_PO_LVI);

        logger_trace("out for ac97 %d %x", lvi, buffers[lvi]->physical_base());

        playing = true;
        lvi = (lvi + 2) % AC97_BDL_LEN;
    }
    else
    {
        return_size = _buffer.write((char *)buffer, size);
        logger_trace("in here");
    }

    return return_size;
}

// Result call(FsHandle &handle, IOCall request, void *args) { ; };
