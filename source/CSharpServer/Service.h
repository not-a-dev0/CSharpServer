#pragma once

#pragma managed(push,off)
#include <yvals.h>
#ifdef _M_CEE
  #undef _M_CEE
  #include <future>
  #include <mutex>
  #include <shared_mutex>
  #include <thread>
  #define _M_CEE 001
#else
  #include <future>
  #include <mutex>
  #include <thread>
  #include <shared_mutex>
#endif
#pragma managed(pop)

#include <server/asio/service.h>

#include "Embedded.h"
#include "Protocol.h"

#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace msclr::interop;

using namespace System;

namespace CSharpServer {

    ref class Service;

    class ServiceEx : public CppServer::Asio::Service
    {
    public:
        using CppServer::Asio::Service::Service;

        gcroot<CSharpServer::Service^> root;

        void onThreadInitialize() override;
        void onThreadCleanup() override;
        void onStarted() override;
        void onStopped() override;
        void onIdle() override;
        void onError(int error, const std::string& category, const std::string& message) override;
    };

    //! Service
    public ref class Service
    {
    public:
        //! Initialize service with single working threads
        Service() : Service(1, false) {}
        //! Initialize service with multiple working threads
        /*!
            \param threads - Working threads count
        */
        Service(int threads) : Service(threads, false) {}
        //! Initialize service with multiple working threads and thread pool flag
        /*!
            \param threads - Working threads count
            \param pool - Service thread pool flag
        */
        Service(int threads, bool pool);
        ~Service() { this->!Service(); }

        //! Get the number of working threads
        property int Threads { int get() { return (int)_service->get()->threads(); } }

        //! Is the service started with polling loop mode?
        property bool IsPolling { bool get() { return _service->get()->IsPolling(); } }
        //! Is the service started?
        property bool IsStarted { bool get() { return _service->get()->IsStarted(); } }

        //! Start the service
        /*!
            \return 'true' if the service was successfully started, 'false' if the service failed to start
        */
        bool Start() { return Start(false); }
        //! Start the service with polling loop mode
        /*!
            \param polling - Polling loop mode with idle handler call (default is false)
            \return 'true' if the service was successfully started, 'false' if the service failed to start
        */
        bool Start(bool polling) { return _service->get()->Start(polling); }
        //! Stop the service
        /*!
            \return 'true' if the service was successfully stopped, 'false' if the service is already stopped
        */
        bool Stop() { return _service->get()->Stop(); }
        //! Restart the service
        /*!
            \return 'true' if the service was successfully restarted, 'false' if the service failed to restart
        */
        bool Restart() { return _service->get()->Restart(); }

        //! Generate data size string
        /*!
            Will return a pretty string of bytes, KiB, MiB, GiB, TiB based on the given bytes.

            \param bytes - Data size in bytes
            \return String with data size representation
        */
        static String^ GenerateDataSize(double bytes);
        //! Generate time period string
        /*!
            Will return a pretty string of ns, mcs, ms, s, m, h based on the given nanoseconds.

            \param milliseconds - Milliseconds
            \return String with time period representation
        */
        static String^ GenerateTimePeriod(double milliseconds);

    protected:
        //! Initialize thread handler
        /*!
             This handler can be used to initialize priority or affinity of the service thread.
        */
        virtual void OnThreadInitialize() {}
        //! Cleanup thread handler
        /*!
             This handler can be used to cleanup priority or affinity of the service thread.
        */
        virtual void OnThreadCleanup() {}

        //! Handle service started notification
        virtual void OnStarted() {}
        //! Handle service stopped notification
        virtual void OnStopped() {}

        //! Handle service idle notification
        virtual void OnIdle() { Thread::Yield(); }

        //! Handle error notification
        /*!
            \param error - Error code
            \param category - Error category
            \param message - Error message
        */
        virtual void OnError(int error, String^ category, String^ message) {}

    internal:
        void InternalOnThreadInitialize() { OnThreadInitialize(); }
        void InternalOnThreadCleanup() { OnThreadCleanup(); }
        void InternalOnStarted() { OnStarted(); }
        void InternalOnStopped() { OnStopped(); }
        void InternalOnIdle() { OnIdle(); }
        void InternalOnError(int error, String^ category, String^ message) { OnError(error, category, message); }

    protected:
        !Service() { _service.Release(); };

    internal:
        Embedded<std::shared_ptr<ServiceEx>> _service;
    };

}
