// It is difficult to deal with nested modal dialogs. When our UI thread is
// already running a modal loop, if we run a second modal loop while dispatching
// a message, then the modal loops are nested. We cannot quit the outer loop
// util the inner has quit.
//
// To make the problem simple, we try to avoid the nested modal dialogs.
//
// When we want to run a modal dialog, the AsyncModalRunnerManager creates an
// AsyncModalRunner and then run the modal dialog on it.
//
// When the modal dialog ended, the AsyncModalRunner it ran on will destroy
// itself automatically.
//
// When the application wants to quit, it should call
// AsyncModalRunnerManager::CancelModalThenExit to cancel all running modal
// dialogs.

#ifndef NIM_WIN_GUI_MSG_BOX_ASYNC_MODAL_RUNNER_H_
#define NIM_WIN_GUI_MSG_BOX_ASYNC_MODAL_RUNNER_H_

#include <list>
#include <memory>
#include "base/callback/callback.h"
#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "base/synchronization/waitable_event.h"
#include "base/thread/thread.h"
#include "modal_wnd_base.h"

class AsyncModalRunner : protected nbase::Thread {
public:
    class Delegate {
    public:
        virtual void OnThreadWillExit(AsyncModalRunner* runner) = 0;
    };

    AsyncModalRunner(Delegate* delegate);
    virtual ~AsyncModalRunner();

    // Once this method is called the runner will take
    // the ownership of the dialog
    bool DoModal(ModalWndBase* dlg);
    void CancelModalThenExit();

private:
    friend class AsyncModalRunnerManager;
    void Run();

private:
    bool is_running_;
    bool quit_posted_;
    Delegate* delegate_;
    nbase::WaitableEvent event_;
    std::unique_ptr<ModalWndBase> modal_dlg_;
};

class AsyncModalRunnerManager : public AsyncModalRunner::Delegate, public nbase::SupportWeakCallback {
public:
    SINGLETON_DEFINE(AsyncModalRunnerManager);

    // Once this method is called the runner will take
    // the ownership of the dialog
    bool DoModal(ModalWndBase* dlg);
    void CancelAllThreads();

private:
    AsyncModalRunnerManager();
    virtual ~AsyncModalRunnerManager();

    void OnThreadWillExit(AsyncModalRunner* runner);
    void Deregister(AsyncModalRunner* runner);

    nbase::NLock threads_lock_;
    std::list<std::shared_ptr<AsyncModalRunner> > runners_;
};

#endif  // NIM_WIN_GUI_MSG_BOX_ASYNC_MODAL_RUNNER_H_