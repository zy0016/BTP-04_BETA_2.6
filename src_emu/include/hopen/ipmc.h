#ifndef	__IPMC_H
#define	__IPMC_H

#include <sys/ioctl.h> 

#define IPMC_IOC_MAGIC  'K'

#define IPMC_IOCTL_GET_IPM_CONFIG  _IOR(IPMC_IOC_MAGIC, 3, struct ipm_config)
#define IPMC_IOCTL_SET_IPM_CONFIG  _IOW(IPMC_IOC_MAGIC, 4, int)

#define IPMC_IOCTL_GET_DVFM_CONFIG  _IOR(IPMC_IOC_MAGIC, 3, struct ipm_config)
#define IPMC_IOCTL_SET_DVFM_CONFIG  _IOW(IPMC_IOC_MAGIC, 4, struct ipm_config)

#define IPMC_IOCTL_GET_EVENT   _IOR(IPMC_IOC_MAGIC, 5, int)
#define IPMC_IOCTL_SET_EVENT   _IOW(IPMC_IOC_MAGIC, 6, int)

#define IPMC_IOCTL_RESET_UI_TIMER _IOW(IPMC_IOC_MAGIC, 8, int)

#define	IPMC_IOCTL_ENABLE_SLEEP  _IOR(IPMC_IOC_MAGIC, 9, int) 
#define	IPMC_IOCTL_DISABLE_SLEEP  _IOW(IPMC_IOC_MAGIC, 10, int) 

#define	IPMC_IOCTL_GET_WAKETIME  _IOR(IPMC_IOC_MAGIC, 11, int) 
#define	IPMC_IOCTL_SET_WAKETIME  _IOW(IPMC_IOC_MAGIC, 12, int) 

#define	IPMC_IOCTL_GET_UITIME	_IOR(IPMC_IOC_MAGIC, 13, int)
#define	IPMC_IOCTL_SET_UITIME	_IOW(IPMC_IOC_MAGIC, 14, int)

#define	IPMC_IOCTL_GET_SLEEPLEVEL _IOR(IPMC_IOC_MAGIC, 15, int)
#define	IPMC_IOCTL_SET_SLEEPLEVEL _IOW(IPMC_IOC_MAGIC, 16, int)

#define	IPMC_IOCTL_STARTPMU	 _IOW(IPMC_IOC_MAGIC, 17, int)
#define	IPMC_IOCTL_STOPPMU	_IOW(IPMC_IOC_MAGIC, 18, int)//struct pmu_results)

#define	IPMC_IOCTL_CPU    	 _IOW(IPMC_IOC_MAGIC, 19, int)


#define PM_POWER_DOWN 0x1111
#define PM_CPU_OFF	0x2222

#define	IPMC_IOCTL_CLEAR_EVENTLIST _IOW(IPMC_IOC_MAGIC, 20, int)
#define NOVOLCHG    0
#define HIGHER      1
#define LOWER       2
#define IPMC_IOCTL_GET_POWERON_REASON  _IOW(IPMC_IOC_MAGIC, 21, int)
#define IPMC_IOCTL_NORMAL_POWEROFF     _IOW(IPMC_IOC_MAGIC, 22, int)  
#define	IPMC_IOCTL_VIBRATORON          _IOW(IPMC_IOC_MAGIC, 23, int)
#define	IPMC_IOCTL_GET_ERPSTATUS       _IOW(IPMC_IOC_MAGIC, 24, int)
#define	IPMC_IOCTL_GET_WAKEUP_REASON       _IOW(IPMC_IOC_MAGIC, 25, int)
#define	IPMC_IOCTL_SETFLAG       _IOW(IPMC_IOC_MAGIC, 26, int)
#define	IPMC_IOCTL_CLRANFLAG       _IOW(IPMC_IOC_MAGIC, 27, int)
#define	IPMC_IOCTL_SET_INFLED       _IOW(IPMC_IOC_MAGIC, 28, int)
#define	SET_SLEEP_INTERVAL	 _IOW(IPMC_IOC_MAGIC, 29, int)
#define	IPMC_IOCTL_SET_KEYBL	 _IOW(IPMC_IOC_MAGIC, 30, int)
#define	IPMC_IOCTL_GET_KEYBL	 _IOW(IPMC_IOC_MAGIC, 31, int)
#define   IPMC_IOCTL_SHUTDOWN_GSM		_IOW(IPMC_IOC_MAGIC, 32, int)
#define   IPMC_IOCTL_CPU_RESET		_IOW(IPMC_IOC_MAGIC, 33, int)
#define   IPMC_IOCTL_IGNITE_GSM		_IOW(IPMC_IOC_MAGIC, 34, int)

#define MAX_IPME_NUM 25	/*	20 IPM event max */
/*      IPM events queue */

struct  ipm_config {
    /*  Below  items must be set to set configurations. */
    unsigned int    cpu_mode;
    unsigned int    core_freq;  /*  in khz, run mode.*/
                                /*  If CPU is in Turbo mode. plus N. */
    unsigned int    core_vltg;    /*  in mV.  */
    unsigned int    turbo_mode;     /*  specify the N value.    */
    unsigned int    fast_bus_mode;
    /*  Below items may need to get system DPM configurations.  */
    unsigned int    sys_bus_freq;
    unsigned int    mem_bus_freq;
    unsigned int    lcd_freq;
    unsigned int    enabled_device;
};


struct fv_table {
    unsigned int freq;
    unsigned int voltage;
};

struct ipm_event {
	unsigned int type;	/*	What type of IPM events.	*/
	unsigned int kind;	/*	What kind, or sub-type of events.*/
	unsigned int info;
};

#define	IPM_EVENTS_TYPE(x)   ((x&0xFF000000)>>24)
#define	IPM_EVENTS_KIND(x)   ((x&0x00F00000)>>16)
#define	IPM_EVENTS_INFO(x)   (x&0xFF)

/*	IPM event types.	*/
#define	IPM_EVENT_PROFILER	0x0	
#define	IPM_EVENT_PMU		0x1	
#define	IPM_EVENT_DEVICE	0x2	
#define	IPM_EVENT_POWER		0x3	
#define	IPM_EVENT_TIMER		0x4	
#define	IPM_EVENT_CPU		0x5	
#define	IPM_EVENT_SYSTEM	0x6

#define	IPM_EVENT_DEVICE_TIMEOUT	0x0
#define	IPM_EVENT_POWER_LOW		0x1
#define	IPM_EVENT_POWER_FAULT	0x2
#define	IPM_EVENT_POWER_OK		0x3
#define	IPM_EVENT_SYSTEM_WAKEUP	0x4

#define	IPM_EVENT_IDLE_PROFILER	0x0
#define	IPM_EVENT_PERF_PROFILER	0x1

#define	IPM_EVENT_CPUVERYBUSY		0x0	
#define	IPM_EVENT_CPUBUSY		0x1	
#define	IPM_EVENT_CPUFREE		0x2	

#define	IPM_EVENT_NULLINFO	0x0
#define	IPM_WAKEUPBYRTC		0x1
#define	IPM_WAKEUPBYUI		0x2

enum
{
   CPUMODE_RUN,
   CPUMODE_IDLE,
   CPUMODE_STANDBY,
   CPUMODE_SLEEP,
   CPUMODE_RESERVED,
   CPUMODE_SENSE,
   CPUMODE_RESERVED2,
   CPUMODE_DEEPSLEEP,
   CPUMODE_13M,
   CPUMODE_RUNPMU,
};
#define	SLEEP_SAVE_SIZE  5
#define	SLEEP_SAVE_CKSUM 5

#define CPU_FREQ_208M	0x2
#define CPU_FREQ_312M	0x3
#define CPU_FREQ_416M	0x4
//unsigned int  pm_sleeptime=58;	/*	In seconds.	*/

#endif
