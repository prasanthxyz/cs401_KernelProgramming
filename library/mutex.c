#include <linux/mutex.h>

static DEFINE_MUTEX(var_lock);

int main()
{
    if( mutex_trylock(&var_lock) != 1 )
        return -1;
    mutex_unlock(&var_lock);
    return 0;
}
