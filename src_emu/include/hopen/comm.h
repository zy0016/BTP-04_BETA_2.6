/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/comm.h,v $
 * $Name:  $
 *
 * $Revision: 1.5 $     $Date: 2003/06/18 03:19:38 $
 * 
\**************************************************************************/

#ifndef _HOPEN_COMMDEV_H_
#define	_HOPEN_COMMDEV_H_

#include <sys/types.h>
#include <sys/ioctl.h>

/* line disciplines */
#define N_TTY		0
#define N_SLIP		1
#define N_MOUSE		2
#define N_PPP		3
#define N_STRIP		4
#define N_AX25		5
#define N_X25		6	/* X.25 async */
#define N_6PACK		7
#define N_MASC		8	/* Reserved for Mobitex module <kaz@cafe.net> */
#define N_R3964		9	/* Reserved for Simatic R3964 module */
#define N_PROFIBUS_FDL	10	/* Reserved for Profibus <Dave@mvhi.com> */
#define N_IRDA		11	/* Linux IrDa - http://www.cs.uit.no/~dagb/irda/irda.html */
#define N_SMSBLOCK	12	/* SMS block mode - for talking to GSM data cards about SMS messages */
#define N_SYNCPPP	13
#define N_WMUX		14	/* Wavecom multiplexer displine */
#define N_SMUX		15	/* Siemens multiplexer displine */

/* Number of line discplines */
#define NR_LDISCS	16

/*
 * Set baudrate.
 *
 *      arg --------- baudrate value.
 *
 * return:
 *      0 --- succress.
 */
#define SIOSETBAUDRATE		_IO('S', 0x01)

/*
 * Get baudrate.
 *
 *      arg --------- not used.
 *
 * return:
 *      value of baudrate.
 */
#define SIOGETBAUDRATE		_IO('S', 0x02)

/*
 * Set data transfer mode.
 *
 *      arg --------- compact field:
 *
 *        7       6       5       4       3      2      1      0
 *    ------------------------------------------------------------
 *    | stop   |      parity          |     data bits            |
 *    | bits   |                      |                          |
 *    ------------------------------------------------------------
 *
 * data bits:
 *              Number of data bits, valid value is 5, 6, 7, 8.
 *
 * parity:
 *              000          No parity
 *              100          Odd parity
 *              101          Even parity
 *              110          Mark parity
 *              111          Space parity
 *
 * stop bits:
 *              0            one stop bit
 *              1            two stop bit
 *
 * return:
 *      0 --- succress.
 */
#define SIOSETMODE			_IO('S', 0x03)

/*
 * Get data transfer mode.
 *
 *      arg --------- not used
 *
 * return:
 *      Data transfer mode, see upper.
 */
#define SIOGETMODE			_IO('S', 0x04)

/*
 * Set modem control bits.
 *
 *      arg --------- modem control bits
 *
 * return:
 *      Data transfer mode, see upper.
 */
#define SIOSETMODEMCTRL		_IO('S', 0x05)



#	define	MCR_DTR			0x01	/* Data Terminal Ready */
#	define	MCR_RTS			0x02	/* Request to Send */
#	define	MCR_OUT1		0x04	/* Out 1 (not used) */
#	define	MCR_OUT2		0x08	/* OUT 2 (not used) */
#	define	MCR_LOOP		0x10	/* Loopback test mode */

#define	MCR_FLOW		0x20	/* Auto flow control */


/*
 * Set modem status bits.
 *
 *      arg --------- notused
 *
 * return:
 *      Modem status bit.
 */
#define SIOGETMODEMSTAT		_IO('S', 0x06)



#	define	MSR_CTS			0x10	/* Clear to send */
#	define	MSR_DSR			0x20	/* Data set ready */
#	define	MSR_RI			0x40	/* Ring indicator */
#	define	MSR_RLSD		0x80	/* Rx line signal detect */

/*
 * Get buffer size.
 *
 *      arg:
 *               1 --- get input buffer size;
 *               2 --- get output buffer size;
 *
 * return:
 *      Buffer size.
 */
#define SIOGETBUFSIZE		_IO('S', 0x07)

/*
 * Set input buffer size.
 *
 *      arg ----- input buffer size in bytes.
 *
 * return:
 *      0 ----- success.
 */
#define SIOSETINBUFSIZE		_IO('S', 0x08)

/*
 * Set output buffer size.
 *
 *      arg ----- output buffer size in bytes.
 *
 * return:
 *      0 ----- success.
 */
#define SIOSETOUTBUFSIZE	_IO('S', 0x09)

/*
 * Set I/O mode with or without modem control.
 *
 *      arg --------- 0 without modem control.
 *                    1 with modem control.
 *
 * return:
 *      0 ----- success.
 */
#define SIOSETMODEMMODE		_IO('S', 0x0A)

/*
 * Get line discpline.
 *
 *	arg ------- not used
 *
 * return:
 *      line discpline number.
 *
 */
#define SIOGETLDISC		_IO('S', 0x10)

/*
 * Set line discpline.
 *
 *	arg ------- line discpline number.
 *
 * return:
 *      old line discpline number.
 *
 */
#define SIOSETLDISC		_IO('S', 0x11)

/*
 * Set irda mode.
 *
 *	arg ------- on/off(1,0).
 *
 * return:
 *      0 ----- success.
 *
 */

#define SIOSETIRDA		_IO('S', 0x12)


#define SIOGETREVQUEUEIN		_IO('S', 0x13)

 /*
 * Set receive delay
 *
 *      arg ----- 0 for no receive delay.
 *                positive value is mini seconds of receive delay.
 *
 * return:
 *      0 ----- success.
 *
 *   If receive delay is not 0, the driver delay some mini seconds
 *  wakeup receive thread.
 *
 */
#define SIORECVDELAY		_IO('S', 0x81)

/*
 * Wait for ring
 *
 *      arg --------- notused
 *
 * return:
 *      modem state.
 *
 */
#define SIOWAITRING			_IO('S', 0x82)

#define SIOFLUSHBUF			_IO('S', 0x83)

#define SIOCIOFLUSH			1	// flush I/O buffer
#define SIOCOFLUSH			2	// flush output buffer


#ifdef __KERNEL__

#include <hopen/tqueue.h>

/*
 * This structure defines the interface between the low-level comm
 * driver and the comm routines.  The following routines can be
 * defined; unless noted otherwise, they are optional, and can be
 * filled in with a null pointer.
 *
 * int  (*open)(struct comm_struct * comm);
 *
 * 	This routine is called when a particular comm device is opened.
 * 	This routine is mandatory; if this routine is not filled in,
 * 	the attempted open will fail with ENODEV.
 *     
 * void (*close)(struct comm_struct * comm);
 *
 * 	This routine is called when a particular comm device is closed.
 *
 * int (*write)(struct comm_struct * comm, int from_user,
 * 		 const unsigned char *buf, int count);
 *
 * 	This routine is called by the kernel to write a series of
 * 	characters to the comm device.  The characters may come from
 * 	user space or kernel space.  This routine will return the
 *	number of characters actually accepted for writing.  This
 *	routine is mandatory.
 *
 * void (*put_char)(struct comm_struct * comm, unsigned char ch);
 *
 * 	This routine is called by the kernel to write a single
 * 	character to the comm device.  If the kernel uses this routine,
 * 	it must call the flush_chars() routine (if defined) when it is
 * 	done stuffing characters into the driver.  If there is no room
 * 	in the queue, the character is ignored.
 *
 * void (*flush_chars)(struct comm_struct * comm);
 *
 * 	This routine is called by the kernel after it has written a
 * 	series of characters to the comm device using put_char().  
 * 
 * int  (*write_room)(struct comm_struct * comm);
 *
 * 	This routine returns the numbers of characters the comm driver
 * 	will accept for queuing to be written.  This number is subject
 * 	to change as output buffers get emptied, or if the output flow
 *	control is acted.
 * 
 * int  (*ioctl)(struct comm_struct * comm, int isuser,
 * 	    unsigned long cmd, unsigned long arg);
 *
 * 	This routine allows the comm driver to implement
 *	device-specific ioctl's.  If the ioctl number passed in cmd
 * 	is not recognized by the driver, it should return ENOIOCTLCMD.
 * 
 * void (*stop)(struct comm_struct * comm);
 *
 * 	This routine notifies the comm driver that it should stop
 * 	outputting characters to the comm device.  
 * 
 * void (*start)(struct comm_struct * comm);
 *
 * 	This routine notifies the comm driver that it resume sending
 *	characters to the comm device.
 * 
 * void (*hangup)(struct comm_struct * comm);
 *
 * 	This routine notifies the comm driver that it should hangup the
 * 	comm device.
 *
 * void (*send_xchar)(struct comm_struct * comm, char ch);
 *
 * 	This routine is used to send a high-priority XON/XOFF
 * 	character to the device.
 */
struct comm_struct;
struct comm_driver{
    const char	*   driver_name;
    const char  *   name;	    /* Name of this device */
    unsigned long   devtype; 
    unsigned long   attrib;
    unsigned short  minor_start, num;
    int  (*open)(struct comm_struct * comm);
    void (*close)(struct comm_struct * comm);
    int  (*write)(struct comm_struct * comm, int from_user, const unsigned char * buf, int count);
    void (*put_char)(struct comm_struct *comm, unsigned char ch);
    void (*flush_chars)(struct comm_struct *comm);
    int  (*write_room)(struct comm_struct *comm);
    int  (*ioctl)(struct comm_struct *comm, int isuser, unsigned long cmd, unsigned long arg);
    void (*stop)(struct comm_struct *comm);
    void (*start)(struct comm_struct *comm);
    void (*send_xchar)(struct comm_struct *comm, char ch);
    void (*read_notify)(struct comm_struct *comm);
}; 

/*
 * This structure defines the interface between the comm line discpline
 * implementation and the comm routines.  The following routines can be
 * defined; unless noted otherwise, they are optional, and can be
 * filled in with a null pointer.
 *
 * int	(*open)(struct comm_struct * comm);
 *
 * 	This function is called when the line discpline is associated
 * 	with the comm.  The line discpline can use this as an
 * 	opportunity to initialize any state needed by the ldisc routines.
 * 
 * void	(*close)(struct comm_struct * comm);
 *
 *	This function is called when the line discpline is being
 * 	shutdown, either because the comm is being closed or because
 * 	the comm is being changed to use a new line discpline
 * 
 * void	(*flush_buffer)(struct comm_struct * comm);
 *
 * 	This function instructs the line discipline to clear its
 * 	buffers of any input characters it may have queued to be
 * 	delivered to the user mode process.
 * 
 * ssize_t (*chars_in_buffer)(struct comm_struct * comm);
 *
 * 	This function returns the number of input characters the line
 *	discpline may have queued up to be delivered to the user mode
 *	process.
 * 
 * ssize_t (*read)(struct comm_struct * comm, struct file * file,
 *		   unsigned char * buf, size_t nr);
 *
 * 	This function is called when the user requests to read from
 * 	the comm.  The line discpline will return whatever characters
 * 	it has buffered up for the user.  If this function is not
 * 	defined, the user will receive an EIO error.
 * 
 * ssize_t (*write)(struct comm_struct * comm, struct file * file,
 * 		    const unsigned char * buf, size_t nr);
 *
 * 	This function is called when the user requests to write to the
 * 	comm.  The line discpline will deliver the characters to the
 * 	low-level comm device for transmission, optionally performing
 * 	some processing on the characters first.  If this function is
 * 	not defined, the user will receive an EIO error.
 * 
 * int	(*ioctl)(struct comm_struct * comm, struct file * file,
 * 		 unsigned long cmd, unsigned long arg);
 *
 *	This function is called when the user requests an ioctl which
 * 	is not handled by the comm layer or the low-level comm driver.
 * 	It is intended for ioctls which affect line discpline
 * 	operation.  Not that the search order for ioctls is (1) comm
 * 	layer, (2) comm low-level driver, (3) line discpline.  So a
 * 	low-level driver can "grab" an ioctl request before the line
 * 	discpline has a chance to see it.
 * 
 * int	(*poll)(struct comm_struct * comm, struct file * file, unsigned mask,
 * 		  poll_table *wait);
 *
 * 	This function is called when a user attempts to select/poll on a
 * 	comm device.  It is solely the responsibility of the line
 * 	discipline to handle poll requests.
 *
 * void	(*receive_buf)(struct comm_struct * comm, const unsigned char *cp,
 * 		       char *fp, int count);
 *
 * 	This function is called by the low-level comm driver to send
 * 	characters received by the hardware to the line discpline for
 * 	processing.  <cp> is a pointer to the buffer of input
 * 	character received by the device.  <fp> is a pointer to a
 * 	pointer of flag bytes which indicate whether a character was
 * 	received with a parity error, etc. <fp> may be NULL.
 * 
 * int	(*receive_room)(struct comm_struct * comm);
 *
 * 	This function is called by the low-level comm driver to
 * 	determine how many characters the line discpline can accept.
 * 	The low-level driver must not send more characters than was
 * 	indicated by receive_room, or the line discpline may drop
 * 	those characters.
 * 
 * void	(*write_wakeup)(struct comm_struct * comm);
 *
 * 	This function is called by the low-level comm driver to signal
 * 	that line discpline should try to send more characters to the
 * 	low-level driver for transmission.  If the line discpline does
 * 	not have any more data to send, it can just return.
 *
 * void	(*line_wakeup)(struct comm_struct * comm, int line_state);
 *
 * 	This function is called by the low-level comm driver to signal
 * 	that line discpline that line state is changed.
 */
enum line_state { PPP_LINEUP, PPP_LINEDOWN };

struct poll_table_struct;

struct comm_ldisc {
     char	*name;
     int	num;	/* discpline number */
     /*
      * The following routines are called from above.
      */
     int (*open)(struct comm_struct *);
     int (*close)(struct comm_struct *);
     int (*flush_buffer)(struct comm_struct *);
     int (*chars_in_buffer)(struct comm_struct *);
     int (*read)(struct comm_struct *, struct file *, unsigned char *, int);
     int (*write)(struct comm_struct *, struct file *, const unsigned char *, int);	
     int (*ioctl)(struct comm_struct *, struct file *, unsigned long, unsigned long);
     int (*poll)(struct comm_struct *, struct file *, int, struct poll_table_struct *);
     /*
      * The following routines are called from below.
      */
     void (*receive_buf)(struct comm_struct *, const unsigned char *cp, char *fp, int count);
     int  (*receive_room)(struct comm_struct *);
     void (*write_wakeup)(struct comm_struct *);
     void (*line_wakeup)(struct comm_struct *, int);
};

enum buffer_mode { COMM_ROUNDBUF = 0, COMM_FLIPBUF };

/*
 * This is the round buffer used for the comm driver.  The buffer is
 * located in the comm structure, and is used as a high speed interface
 * between the comm driver and the comm line discipline.
 */

struct comm_buffer {
    int   buftype;  /* Type of this buffer */
    union {
	struct {
	    int   bufsize;
	    int   front, tail;
	    char * pbuffer;
	} round_buffer;
    } u;
};

struct comm_struct {
	kdev_t device;
	unsigned long flags; 
	struct comm_driver driver;
	struct comm_ldisc  ldisc;
	struct tq_struct taskq;		/* Buttom half task queue */
	unsigned long callback_event;	/* Serial device callback event */
	int line_state;			/* Line state of this device */
        int modem_event;                /* If with modem event */
	int receive_delay;		/* Receive delay value */
	struct comm_buffer buffer;       /* Receive buffer */
	void *disc_data;
	void *driver_data;
	int (*callback )(struct comm_struct * pcomm, int event);
	struct wait_queue *write_wait;
	struct wait_queue *read_wait;
};

/*
 * Define the event bit.
 */

#define COMM_IN_EVENT			0x01
#define COMM_OUT_EVENT			0x02
#define COMM_MODEM_EVENT		0x04

int register_comm_driver ( const struct comm_driver * driver );
int unregister_comm_driver ( const char * driver_name );
int comm_register_ldisc(int disc, struct comm_ldisc *new_ldisc);

int comm_open (kdev_t dev, struct comm_struct ** preturn);
int comm_close (struct comm_struct * pcomm);
int comm_setldisc (struct comm_struct * pcomm, int new_ldisc);

#endif __KERNEL__

#endif	//_HOPEN_COMMDEV_H_

