#include "Inotify.h"
namespace utils
{
INotifyFile::INotifyFile(const std::string & filepath, int flag)
{
    fd = inotify_init();
    if (fd < 0)
    {
        throw INotifyFileError("Can't call inotify_init");
    }
    wd = inotify_add_watch(fd, filepath.c_str(), flag);
    if (wd < 0)
    {
        throw INotifyFileError("Can't call inotify_add_watch");
    }
}

bool INotifyFile::checkEvent() const
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    return select(FD_SETSIZE, &rfds, NULL, NULL, NULL);
}

int INotifyFile::readEvents()
{
    char buffer[16384];
    inotify_event * pevent;
    std::size_t eventSize;
    std::size_t r;
    int count = 0;
    r = read(fd, buffer, 16384);
    if (r <= 0)
        return r;
    std::size_t bufferIndex = 0;
    while (bufferIndex < r)
    {
        pevent = (inotify_event *)&buffer[bufferIndex];
        eventSize = offsetof(inotify_event, name) + pevent->len;
        inotify_event event;
        memcpy(&event, pevent, eventSize);
        q.push_back(event);
        bufferIndex += eventSize;
        count++;
    }
    return count;
}

Event INotifyFile::processLatestEvent()
{
    inotify_event ev = q.front();
    q.pop_front();
    switch (ev.mask & (IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED))
    {
        case IN_ACCESS:
            return Event::in_access;
        case IN_MODIFY:
            return Event::in_modify;
        case IN_ATTRIB:
            return Event::in_attrib;
        case IN_CLOSE_WRITE:
            return Event::in_close_write;
        case IN_CLOSE_NOWRITE:
            return Event::in_close_nowrite;
        case IN_OPEN:
            return Event::in_open;
        case IN_MOVED_FROM:
            return Event::in_moved_from;
        case IN_IGNORED:
            return Event::in_ignored;
        default:
            return Event::unkn;
    }
}
}
