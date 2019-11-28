#include <circle/memory.h>
#include <circle/startup.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/logger.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/ntpdaemon.h>
#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <circle/multicore.h>
#include <circle/string.h>
#include <atomic>
#include <cstring>
#include "gl.h"

static const char NTPServer[]   = "tempus1.gum.gov.pl";
static const int TimeZone       = 60;		// minutes diff to UTC
static const u16 UdpPort        = 8888;

class myMultiCore :
    public CMultiCoreSupport
{
    public:
        myMultiCore(CScreenDevice &_scr, CMemorySystem *_mem);
        myMultiCore(CMemorySystem *_mem, CScreenDevice &_scr, CNetSubSystem &_net, CScheduler &_sched);
        virtual ~myMultiCore(void) {};
        void Run(unsigned core);
        void GlComm(void);
        void EthComm(void);

    private:
        GL gl;
        CScreenDevice &scr;
        CNetSubSystem &net;
        CScheduler &sched;
        volatile bool boot;
        std::atomic<int> running;
};

class myUDPRecv : public CTask
{
    public:
        myUDPRecv(CNetSubSystem &_net, u16 _port);
        virtual ~myUDPRecv(void);
        void Run(void);

    private:
        CNetSubSystem &net;
        u16 port;
        CSocket sock;
};

class myKernel
{
    public:
        myKernel(void);
        virtual ~myKernel(void) {};

        boolean Initialize(void);
        int Run(void);

    private:
        CMemorySystem mem;
        CActLED led;
        CKernelOptions kopts;
        CDeviceNameService dname;
        CScreenDevice scr;
        CSerialDevice ser;
        CExceptionHandler exh;
        CInterruptSystem irq;
        CTimer tmr;
        CLogger log;
        CScheduler sched;
        CUSBHCIDevice usb_hci;
        CNetSubSystem net;
        myMultiCore mcore;
        unsigned ticks;
};

myMultiCore::myMultiCore(CMemorySystem *_mem, CScreenDevice &_scr, CNetSubSystem &_net, CScheduler &_sched) :
    CMultiCoreSupport(_mem),
    scr(_scr),
    net(_net),
    sched(_sched),
    boot(true),
    running(0x0f)
{
}

void myMultiCore::Run(unsigned core)
{
    if (core == 0)
        boot = false;
    else
        while(boot);

    switch(core) {
        case 0:
            break;
        case 1:
            GlComm();
            break;
        case 2:
            EthComm();
            break;
        case 3:
            break;
        default:
            break;
    }

    running &= ~(1 << core);

    if (core == 0)
        while(running);
}

void myMultiCore::GlComm(void)
{
    CString txt;
    uint32_t memhandle, memaddr;

    if (!gl.Init())
        return;

    txt = "MemAlloc\n";
    memhandle = gl.MemAlloc(0x800000, 0x1000, GL::MemFlag(GL::MemFlag::Coherent | GL::MemFlag::Zero));

    if (!memhandle)
        goto err;

    txt = "MemLock\n";
    memaddr = gl.MemLock(memhandle);

    if (!memaddr)
        goto err;

    txt.Format("%08x %p\n", memaddr, GL::ArmPtr(memaddr));
err:
    scr.Write(txt, txt.GetLength());
}

void myMultiCore::EthComm(void)
{
	new CNTPDaemon(NTPServer, &net);
    new myUDPRecv(net, UdpPort);

    while (1) {
        CString t = *CTimer::Get()->GetTimeString();
        t.Append("\n");
        scr.Write(t, t.GetLength());
        sched.Sleep(1);
    }
}

myUDPRecv::myUDPRecv(CNetSubSystem &_net, u16 _port) :
    net(_net),
    port(_port),
    sock(&net, IPPROTO_UDP)
{
}

myUDPRecv::~myUDPRecv(void)
{
}

void myUDPRecv::Run(void)
{
    char buf[FRAME_BUFFER_SIZE+1];

    if (sock.Bind(port) < 0) {
        CLogger::Get()->Write("network", LogError, "Cannot bind to port %u", port);
        return;
    }

    sock.SetOptionBroadcast(true);

    while(1) {
        int n = sock.Receive(buf, FRAME_BUFFER_SIZE, 0);

        if (n < 0) {
            CLogger::Get()->Write("network", LogError, "Socket error %d", n);
        }
        else if (n > 0) {
            buf[n] = 0;
            CLogger::Get()->Write("network", LogNotice, "Socket data: %s", buf);
        }

        CScheduler::Get()->Yield();
    }
}

myKernel::myKernel(void) :
    scr(kopts.GetWidth(), kopts.GetHeight()),
    tmr(&irq),
    log(kopts.GetLogLevel()),
    usb_hci(&irq, &tmr),
    mcore(&mem, scr, net, sched)
{
    led.Blink(3);
}

boolean myKernel::Initialize(void)
{
    if (!scr.Initialize())
        return false;

    if (!ser.Initialize(115200))
        return false;

    if (!log.Initialize(&ser))
        return false;

    if (!irq.Initialize())
        return false;

    if (!tmr.Initialize())
        return false;

    ticks = tmr.GetClockTicks();

    if (!usb_hci.Initialize())
        return false;

    if (!net.Initialize())
        return false;

    ticks = tmr.GetClockTicks() - ticks;

    if (!mcore.Initialize())
        return false;

    return true;
}

int myKernel::Run(void)
{
    CString txt;
    log.Write("kernel", LogNotice, "Start");
    dname.ListDevices(&scr);
    dname.ListDevices(&ser);
    txt.Format("Network init took: %u us\n", ticks);
    scr.Write(txt, txt.GetLength());
	tmr.SetTimeZone(TimeZone);
    mcore.Run(0);
    return EXIT_HALT;
}

int main(void)
{
    myKernel k;

    if (!k.Initialize()) {
        return EXIT_HALT;
    }

    return k.Run();
}
