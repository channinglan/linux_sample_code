/**
 * kthread_run - create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread or ERR_PTR(-ENOMEM).
 */
#define kthread_run(threadfn, data, namefmt, ...)      \
({            \
 struct task_struct *__k         \
  = kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
 if (!IS_ERR(__k))         \
  wake_up_process(__k);        \
 __k;           \
})