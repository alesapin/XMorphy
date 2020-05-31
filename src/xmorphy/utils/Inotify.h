#ifndef _I_NOTIFY_H
#define _I_NOTIFY_H
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <sys/inotify.h>
namespace utils
{
class INotifyFileError : public std::runtime_error
{
public:
    INotifyFileError(const std::string & event) : std::runtime_error(event) {}
};
enum class Event
{
    unkn = 0x00000000,
    in_access = 0x00000001, /* file was accessed */
    in_modify = 0x00000002, /* file was modified */
    in_attrib = 0x00000004, /* metadata changed */
    in_close_write = 0x00000008, /* writtable file was closed */
    in_close_nowrite = 0x00000010, /* unwrittable file closed */
    in_open = 0x00000020, /* file was opened */
    in_moved_from = 0x00000040, /* file was moved from x */
    in_moved_to = 0x00000080, /* file was moved to y */
    in_create = 0x00000100, /* subfile was created */
    in_delete = 0x00000200, /* subfile was deleted */
    in_delete_self = 0x00000400, /* self was deleted */
    in_ignored = 0x00000800,
};

class INotifyFile
{
private:
    int fd;
    int wd;

    std::deque<inotify_event> q;

public:
    INotifyFile(const std::string & filepath, int flag);
    bool checkEvent() const;
    bool empty() const { return q.empty(); }
    int readEvents();
    Event processLatestEvent();
    void dropQueue() { q.clear(); }
};
}
#endif
