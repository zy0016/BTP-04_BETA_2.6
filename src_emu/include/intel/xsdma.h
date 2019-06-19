#ifndef __XSDMA_H
#define __XSDMA_H

#include <hal/cache.h>
#include <hal/hal.h>

#if 0         //defined in /hal/bulverde.h
#define DCSR_BUSERRINTR     (0x1U << 0)  // Bus error status bit
#define DCSR_STARTINTR      (0x1U << 1)  // Descriptor fetch status 
#define DCSR_ENDINTR        (0x1U << 2)  // finish status
#define DCSR_STOPINTR       (0x1U << 3)  // stopped status

#define DCSR_REQPEND        (0x1U << 8)  // Request Pending (read-only)
#define DCSR_EORINT         (0x1U << 9)  // The End-of-Receive interrupt bit
#define DCSR_CMPST          (0x1U << 10) // The Descriptor Compare Status bit
#define DCSR_CLRCMPST       (0x1U << 24) // Clear Descriptor Compare Status bit
#define DCSR_SETCMPST       (0x1U << 25) // Set Descriptor Compare Status bit
#define DCSR_EORSTOPEN      (0x1U << 26) // Enable the Stop on the EOR
#define DCSR_EORJMPEN       (0x1U << 27) // Enable the Jump to the Next descriptor
#define DCSR_EORIRQEN       (0x1U << 28) // Enable the End-of-Receive interrupt
#define DCSR_STOPIRQEN      (0x1U << 29) // Enable the stopped interrupt
#define DCSR_NOFETCH        (0x1U << 30) // Descriptor fetch mode, 0 = fetch
#define DCSR_RUN            (0x1U << 31) // Run, 1=start
#endif

#define DCSR_WRITABLES_MSK  (DCSR_BUSERRINTR |\
                             DCSR_STARTINTR  |\
                             DCSR_ENDINTR    |\
                             DCSR_STOPIRQEN  |\
                             DCSR_NOFETCH    |\
                             DCSR_RUN         )


        // Mask to clear all interrupt reasons in DCSR
#define DCSR_CLEAR_INTS_MSK (DCSR_BUSERRINTR |\
                             DCSR_STARTINTR  |\
                             DCSR_ENDINTR    )


typedef struct {
	volatile unsigned long ddadr;	/* Points to the next descriptor + flags */
	volatile unsigned long dsadr;	/* DSADR value for the current transfer */
	volatile unsigned long dtadr;	/* DTADR value for the current transfer */
	volatile unsigned long dcmd;	/* DCMD value for the current transfer */
} bulv_dma_desc;

typedef enum {
	DMA_PRIO_HIGHEST = 0,
	DMA_PRIO_HIGH = 1,
	DMA_PRIO_MEDIUM = 2,
	DMA_PRIO_LOW = 3
} bulv_dma_prio;

int bulv_request_dma (bulv_dma_prio prio,
			 void (*irq_handler)(int, void *, struct pt_regs *),
			 void *data);

void bulv_free_dma (int dma_ch);

#define dma_flush_cache(p, s) cpu_cache_clean_invalidate_range((unsigned long)p, (unsigned long)p + s, 1)
#define dma_invalidate_cache(p, s) cpu_dcache_invalidate_range((unsigned long)p, (unsigned long)p + s)

#endif //__XSDMA_H
