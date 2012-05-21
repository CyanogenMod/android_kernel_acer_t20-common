#ifndef __LINUX_A1026_H
#define __LINUX_A1026_H

#include <linux/ioctl.h>

struct a1026img {
	unsigned char *buf;
	unsigned img_size;
};

struct a1026_platform_data {
	uint32_t gpio_a1026_clk;
	uint32_t gpio_a1026_reset;
	uint32_t gpio_a1026_wakeup;
};

#define A1026_MAX_FW_SIZE    (128*1024)
#define A1026_CMD_FIFO_DEPTH	64

/* IOCTLs for Audience A1026 */
#define A1026_IOCTL_MAGIC 'u'

#define A1026_BOOTUP_INIT           _IOW(A1026_IOCTL_MAGIC, 0x01, struct a1026img *)
#define A1026_WAKEUP                _IOW(A1026_IOCTL_MAGIC, 0x02, void *)
#define A1026_UART_SYNC_CMD         _IOW(A1026_IOCTL_MAGIC, 0x03, void *)
#define A1026_DOWNLOAD_MODE         _IOW(A1026_IOCTL_MAGIC, 0x04, void *)
#define A1026_SET_CONFIG            _IOW(A1026_IOCTL_MAGIC, 0x05, enum A1026_TableID)

#define ENABLE_DIAG_IOCTLS 1
/* For Diag */
#define A1026_SYNC_CMD              _IOW(A1026_IOCTL_MAGIC, 0x50, void *)
#define A1026_READ_DATA             _IOR(A1026_IOCTL_MAGIC, 0x51, unsigned)
#define A1026_WRITE_MSG             _IOW(A1026_IOCTL_MAGIC, 0x52, unsigned)

/* A1026 Command codes */
#define A1026_msg_Sync              0x80000000
#define A1026_msg_Sync_Ack          0x80000000
#define A1026_msg_Sleep             0x80100001
#define A1026_msg_Reset             0x8002

#define RESET_IMMEDIATE             0x0000

/* Bypass */
#define A1026_msg_Bypass	        0x801C /* 0ff = 0x0000; on = 0x0001 (Default) */
#define A1026_msg_VP_ON             0x801C0001
#define A1026_msg_VP_OFF            0x801C0000

/* Diagnostic API Commands */
#define A1026_msg_BOOT              0x0001
#define A1026_msg_BOOT_ACK          0x01

/* general definitions */
#define RETRY_CNT                   5
#define POLLING_RETRY_CNT           3

/* PassThrough */
#define PassThrotuh_Disable         0x80520000
#define PassThrotuh_A_to_C          0x80520048

enum A1026_TableID {
	A1026_TABLE_VOIP_INTMIC,
	A1026_TABLE_VOIP_EXTMIC,
	A1026_TABLE_30CM_CAMCORDER_INTMIC,
	A1026_TABLE_30CM_CAMCORDER_INTMIC_REAR,
	A1026_TABLE_CAMCORDER_EXTMIC,
	A1026_TABLE_30CM_ASR_INTMIC,
	A1026_TABLE_ASR_EXTMIC,
	A1026_TABLE_STEREO_CAMCORDER,
	A1026_TABLE_3M_CAMCORDER_INTMIC,
	A1026_TABLE_3M_CAMCORDER_INTMIC_REAR,
	A1026_TABLE_3M_ASR_INTMIC,
	A1026_TABLE_SOUNDHOUND_INTMIC,
	A1026_TABLE_SOUNDHOUND_EXTMIC,
	A1026_TABLE_ES305_PLAYBACK,
	A1026_TABLE_CTS,
	A1026_TABLE_HDMI_VOIP,
	A1026_TABLE_SUSPEND = 99
};

/* audio table */
#define VOIP_INTMIC                         0x80310000
#define VOIP_EXTMIC                         0x80310001
#define T30CM_CAMCORDER_INTMIC              0x80310002
#define T30CM_CAMCORDER_INTMIC_REAR         0x80310003
#define T30CM_CAMCORDER_EXTMIC              0x80310004
#define T30CM_ASR_INTMIC                    0x80310005
#define ASR_EXTMIC                          0x80310006
#define STEREO_CAMCORDER                    0x80310007
#define T3M_CAMCORDER_INTMIC                0x80310008
#define T3M_CAMCORDER_INTMIC_REAR           0x80310009
#define T3M_ASR_INTMIC                      0x8031000a
#define SOUNDHOUND_INTMIC                   0x8031000b
#define SOUNDHOUND_EXTMIC                   0x8031000c
#define ES305_PLAYBACK                      0x8031000d
#define CTS                                 0x8031000e
#define HDMI_VOIP                           0x8031000f

#endif
