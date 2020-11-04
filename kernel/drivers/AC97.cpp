#include "kernel/drivers/AC97.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task.h"

#include <libsystem/Logger.h>

AC97::AC97(DeviceAddress address) : PCIDevice(address, DeviceClass::SOUND)
{
    // set values
    _volume_PCM = AC97_PCM_OUT_VOLUME;
    _volume_master = AC97_MASTER_VOLUME;

    // get native audio bus master bar and native audio mixer bar
    nabmbar = bar(1).base();
    nambar = bar(0).base();

    //  enable all device interrupts
    out8(nabmbar + AC97_PO_CR, AC97_X_CR_FEIE | AC97_X_CR_IOCE);

    pci_address().write16(PCI_COMMAND, 0x5);

    // default the pcm output to full volume
    out16(nambar + AC97_PCM_OUT_VOLUME, 0x0000);

    initialise_buffers();

    // tell the ac97 where buffer descriptor list is
    out32(nabmbar + AC97_PO_BDBAR, buffer_descriptors_range->physical_base());

    // set last valid index
    lvi = 2;
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
        buffers.push_back(make<MMIORange>(AC97_BDL_BUFFER_LEN * 2));
        buffer_descriptors_list[i].pointer = buffers[i]->physical_base();
        AC97_CL_SET_LENGTH(buffer_descriptors_list[i].cl, AC97_BDL_BUFFER_LEN);
        memset((char *)buffers[i]->base(), 0, AC97_BDL_BUFFER_LEN * 2);

        buffer_descriptors_list[i].cl |= AC97_CL_IOC;
    }
}

AC97::~AC97()
{
}

void AC97::query_from_buffer(void *destination, size_t size)
{
    size_t writtent = _buffer.read((char *)destination, size);

    if (writtent < (AC97_BDL_BUFFER_LEN * 2))
    {
        memset((char *)destination + writtent, 255, (AC97_BDL_BUFFER_LEN * 2) - writtent);
    }
}

void AC97::handle_interrupt()
{
    uint16_t sr = in16(nabmbar + AC97_PO_SR);

    if (!sr)
    {
        return;
    }

    if (sr & AC97_X_SR_BCIS)
    {
        size_t f = (lvi + 2) % AC97_BDL_LEN;

        query_from_buffer((void *)buffers[f]->base(), AC97_BDL_BUFFER_LEN * 2);

        lvi = (lvi + 1) % AC97_BDL_LEN;
        out8(nabmbar + AC97_PO_LVI, lvi);
    }
    else if (sr & AC97_X_SR_LVBCI)
    {
        logger_trace("IRQ is lvbci");
    }
    else if (sr & AC97_X_SR_FIFOE)
    {
        logger_trace("IRQ is fifoe");
    }
    else
    {
        return;
    }

    out16(nabmbar + AC97_PO_SR, sr & 0x1E);
}

bool AC97::can_write(FsHandle &handle)
{
    __unused(handle);
    return !_buffer.full();
}

ResultOr<size_t> AC97::write(FsHandle &handle, const void *buffer, size_t size)
{
    __unused(handle);

    return _buffer.write((char *)buffer, size);
}

Result AC97::call(FsHandle &handle, IOCall request, void *args)
{
    ;
}
