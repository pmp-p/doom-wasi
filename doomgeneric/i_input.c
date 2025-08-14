//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//


#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>

#if !defined(O_NONBLOCK)
#define F_GETFL  3
#define F_SETFL  4
#define O_NONBLOCK    04000
int fcntl(int fd, int cmd, ...) {
    return -1;
}
#endif

#include <stdio.h>

#if !defined(XCC)
#   include <linux/kd.h> // KDGKBTYPE, KB_84, KB_101, KDSKBMODE, K_MEDIUMRAW
#   include <sys/ioctl.h> // ioctl()
#   include <termios.h>  // raw tty init
#else

// ====================================================================================
// <linux/kd.h>
#define 	KB_84		0x01
#define 	KB_101		0x02
#define KDSKBMODE	0x4B45	/* sets current keyboard mode */
#define KDGKBTYPE	0x4B33
#define KDGKBMODE	0x4B44
#define	K_MEDIUMRAW	0x02



// ====================================================================================
#define _IOC(a,b,c,d) ( ((a)<<30) | ((b)<<8) | (c) | ((d)<<16) )
#define _IOC_NONE  0U
#define _IOC_WRITE 1U
#define _IOC_READ  2U

#define _IO(a,b) _IOC(_IOC_NONE,(a),(b),0)
#define _IOW(a,b,c) _IOC(_IOC_WRITE,(a),(b),sizeof(c))
#define _IOR(a,b,c) _IOC(_IOC_READ,(a),(b),sizeof(c))
#define _IOWR(a,b,c) _IOC(_IOC_READ|_IOC_WRITE,(a),(b),sizeof(c))

#define TCGETS		0x5401
#define TCSETS		0x5402
#define TCSETSW		0x5403
#define TCSETSF		0x5404
#define TCGETA		0x5405
#define TCSETA		0x5406
#define TCSETAW		0x5407
#define TCSETAF		0x5408
#define TCSBRK		0x5409
#define TCXONC		0x540A
#define TCFLSH		0x540B
#define TIOCEXCL	0x540C
#define TIOCNXCL	0x540D
#define TIOCSCTTY	0x540E
#define TIOCGPGRP	0x540F
#define TIOCSPGRP	0x5410
#define TIOCOUTQ	0x5411
#define TIOCSTI		0x5412
#define TIOCGWINSZ	0x5413
#define TIOCSWINSZ	0x5414
#define TIOCMGET	0x5415
#define TIOCMBIS	0x5416
#define TIOCMBIC	0x5417
#define TIOCMSET	0x5418
#define TIOCGSOFTCAR	0x5419
#define TIOCSSOFTCAR	0x541A
#define FIONREAD	0x541B
#define TIOCINQ		FIONREAD
#define TIOCLINUX	0x541C
#define TIOCCONS	0x541D
#define TIOCGSERIAL	0x541E
#define TIOCSSERIAL	0x541F
#define TIOCPKT		0x5420
#define FIONBIO		0x5421
#define TIOCNOTTY	0x5422
#define TIOCSETD	0x5423
#define TIOCGETD	0x5424
#define TCSBRKP		0x5425
#define TIOCSBRK	0x5427
#define TIOCCBRK	0x5428
#define TIOCGSID	0x5429
#define TIOCGRS485	0x542E
#define TIOCSRS485	0x542F
#define TIOCGPTN	0x80045430
#define TIOCSPTLCK	0x40045431
#define TIOCGDEV	0x80045432
#define TCGETX		0x5432
#define TCSETX		0x5433
#define TCSETXF		0x5434
#define TCSETXW		0x5435
#define TIOCSIG		0x40045436
#define TIOCVHANGUP	0x5437
#define TIOCGPKT	0x80045438
#define TIOCGPTLCK	0x80045439
#define TIOCGEXCL	0x80045440
#define TIOCGPTPEER	0x5441
#define TIOCGISO7816	0x80285442
#define TIOCSISO7816	0xc0285443

#define FIONCLEX	0x5450
#define FIOCLEX		0x5451
#define FIOASYNC	0x5452
#define TIOCSERCONFIG	0x5453
#define TIOCSERGWILD	0x5454
#define TIOCSERSWILD	0x5455
#define TIOCGLCKTRMIOS	0x5456
#define TIOCSLCKTRMIOS	0x5457
#define TIOCSERGSTRUCT	0x5458
#define TIOCSERGETLSR   0x5459
#define TIOCSERGETMULTI 0x545A
#define TIOCSERSETMULTI 0x545B

#define TIOCMIWAIT	0x545C
#define TIOCGICOUNT	0x545D
#define FIOQSIZE	0x5460

#define TIOCM_LE        0x001
#define TIOCM_DTR       0x002
#define TIOCM_RTS       0x004
#define TIOCM_ST        0x008
#define TIOCM_SR        0x010
#define TIOCM_CTS       0x020
#define TIOCM_CAR       0x040
#define TIOCM_RNG       0x080
#define TIOCM_DSR       0x100
#define TIOCM_CD        TIOCM_CAR
#define TIOCM_RI        TIOCM_RNG
#define TIOCM_OUT1      0x2000
#define TIOCM_OUT2      0x4000
#define TIOCM_LOOP      0x8000

#define FIOSETOWN       0x8901
#define SIOCSPGRP       0x8902
#define FIOGETOWN       0x8903
#define SIOCGPGRP       0x8904
#define SIOCATMARK      0x8905
#if __LONG_MAX == 0x7fffffff
#   define SIOCGSTAMP      _IOR(0x89, 6, char[16])
#   define SIOCGSTAMPNS    _IOR(0x89, 7, char[16])
#else
#   define SIOCGSTAMP      0x8906
#   define SIOCGSTAMPNS    0x8907
#endif

int ioctl(int fd, unsigned long request, ...) {
    return -1;
}
// ====================================================================================



#ifdef __cplusplus
extern "C" {
#endif

/* #include <features.h>

#define __NEED_pid_t
#define __NEED_struct_winsize

#include <bits/alltypes.h>
*/

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

#define NCCS 32

// #include <bits/termios.h>

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_line;
	cc_t c_cc[NCCS];
	speed_t __c_ispeed;
	speed_t __c_ospeed;
};

#define VINTR     0
#define VQUIT     1
#define VERASE    2
#define VKILL     3
#define VEOF      4
#define VTIME     5
#define VMIN      6
#define VSWTC     7
#define VSTART    8
#define VSTOP     9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

#define IGNBRK  0000001
#define BRKINT  0000002
#define IGNPAR  0000004
#define PARMRK  0000010
#define INPCK   0000020
#define ISTRIP  0000040
#define INLCR   0000100
#define IGNCR   0000200
#define ICRNL   0000400
#define IUCLC   0001000
#define IXON    0002000
#define IXANY   0004000
#define IXOFF   0010000
#define IMAXBEL 0020000
#define IUTF8   0040000

#define OPOST  0000001
#define OLCUC  0000002
#define ONLCR  0000004
#define OCRNL  0000010
#define ONOCR  0000020
#define ONLRET 0000040
#define OFILL  0000100
#define OFDEL  0000200
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_XOPEN_SOURCE)
#define NLDLY  0000400
#define NL0    0000000
#define NL1    0000400
#define CRDLY  0003000
#define CR0    0000000
#define CR1    0001000
#define CR2    0002000
#define CR3    0003000
#define TABDLY 0014000
#define TAB0   0000000
#define TAB1   0004000
#define TAB2   0010000
#define TAB3   0014000
#define BSDLY  0020000
#define BS0    0000000
#define BS1    0020000
#define FFDLY  0100000
#define FF0    0000000
#define FF1    0100000
#endif

#define VTDLY  0040000
#define VT0    0000000
#define VT1    0040000

#define B0       0000000
#define B50      0000001
#define B75      0000002
#define B110     0000003
#define B134     0000004
#define B150     0000005
#define B200     0000006
#define B300     0000007
#define B600     0000010
#define B1200    0000011
#define B1800    0000012
#define B2400    0000013
#define B4800    0000014
#define B9600    0000015
#define B19200   0000016
#define B38400   0000017

#define B57600   0010001
#define B115200  0010002
#define B230400  0010003
#define B460800  0010004
#define B500000  0010005
#define B576000  0010006
#define B921600  0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017

#define CSIZE  0000060
#define CS5    0000000
#define CS6    0000020
#define CS7    0000040
#define CS8    0000060
#define CSTOPB 0000100
#define CREAD  0000200
#define PARENB 0000400
#define PARODD 0001000
#define HUPCL  0002000
#define CLOCAL 0004000

#define ISIG   0000001
#define ICANON 0000002
#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400
#define IEXTEN 0100000

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

#define TCSANOW   0
#define TCSADRAIN 1
#define TCSAFLUSH 2


#define EXTA    0000016
#define EXTB    0000017
#define CBAUD   0010017
#define CBAUDEX 0010000
#define CIBAUD  002003600000
#define CMSPAR  010000000000
#define CRTSCTS 020000000000

#define XCASE   0000004
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE  0004000
#define FLUSHO  0010000
#define PENDIN  0040000
#define EXTPROC 0200000

#define XTABS  0014000

// #include <bits/termios.h>

speed_t cfgetospeed (const struct termios *);
speed_t cfgetispeed (const struct termios *);
int cfsetospeed (struct termios *, speed_t);
int cfsetispeed (struct termios *, speed_t);

int tcgetattr (int, struct termios *) {
    return -1;
}

int tcsetattr (int, int, const struct termios *) {
    return -1;
}

int tcgetwinsize (int, struct winsize *);
int tcsetwinsize (int, const struct winsize *);

int tcsendbreak (int, int);
int tcdrain (int);
int tcflush (int, int);
int tcflow (int, int);

pid_t tcgetsid (int);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
void cfmakeraw(struct termios *);
int cfsetspeed(struct termios *, speed_t);
#endif

#ifdef __cplusplus
}
#endif

// ====================================================================================



#endif

#include <unistd.h> // close

#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_scale.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "doomgeneric.h"

int vanilla_keyboard_mapping = 1;

// Is the shift key currently down?

static int shiftdown = 0;

// Lookup table for mapping AT keycodes to their doom keycode
static const char at_to_doom[] =
{
    /* 0x00 */ 0x00,
    /* 0x01 */ KEY_ESCAPE,
    /* 0x02 */ '1',
    /* 0x03 */ '2',
    /* 0x04 */ '3',
    /* 0x05 */ '4',
    /* 0x06 */ '5',
    /* 0x07 */ '6',
    /* 0x08 */ '7',
    /* 0x09 */ '8',
    /* 0x0a */ '9',
    /* 0x0b */ '0',
    /* 0x0c */ '-',
    /* 0x0d */ '=',
    /* 0x0e */ KEY_BACKSPACE,
    /* 0x0f */ KEY_TAB,
    /* 0x10 */ 'q',
    /* 0x11 */ 'w',
    /* 0x12 */ 'e',
    /* 0x13 */ 'r',
    /* 0x14 */ 't',
    /* 0x15 */ 'y',
    /* 0x16 */ 'u',
    /* 0x17 */ 'i',
    /* 0x18 */ 'o',
    /* 0x19 */ 'p',
    /* 0x1a */ '[',
    /* 0x1b */ ']',
    /* 0x1c */ KEY_ENTER,
    /* 0x1d */ KEY_FIRE, /* KEY_RCTRL, */
    /* 0x1e */ 'a',
    /* 0x1f */ 's',
    /* 0x20 */ 'd',
    /* 0x21 */ 'f',
    /* 0x22 */ 'g',
    /* 0x23 */ 'h',
    /* 0x24 */ 'j',
    /* 0x25 */ 'k',
    /* 0x26 */ 'l',
    /* 0x27 */ ';',
    /* 0x28 */ '\'',
    /* 0x29 */ '`',
    /* 0x2a */ KEY_RSHIFT,
    /* 0x2b */ '\\',
    /* 0x2c */ 'z',
    /* 0x2d */ 'x',
    /* 0x2e */ 'c',
    /* 0x2f */ 'v',
    /* 0x30 */ 'b',
    /* 0x31 */ 'n',
    /* 0x32 */ 'm',
    /* 0x33 */ ',',
    /* 0x34 */ '.',
    /* 0x35 */ '/',
    /* 0x36 */ KEY_RSHIFT,
    /* 0x37 */ KEYP_MULTIPLY,
    /* 0x38 */ KEY_LALT,
    /* 0x39 */ KEY_USE,
    /* 0x3a */ KEY_CAPSLOCK,
    /* 0x3b */ KEY_F1,
    /* 0x3c */ KEY_F2,
    /* 0x3d */ KEY_F3,
    /* 0x3e */ KEY_F4,
    /* 0x3f */ KEY_F5,
    /* 0x40 */ KEY_F6,
    /* 0x41 */ KEY_F7,
    /* 0x42 */ KEY_F8,
    /* 0x43 */ KEY_F9,
    /* 0x44 */ KEY_F10,
    /* 0x45 */ KEY_NUMLOCK,
    /* 0x46 */ 0x0,
    /* 0x47 */ 0x0, /* 47 (Keypad-7/Home) */
    /* 0x48 */ 0x0, /* 48 (Keypad-8/Up) */
    /* 0x49 */ 0x0, /* 49 (Keypad-9/PgUp) */
    /* 0x4a */ 0x0, /* 4a (Keypad--) */
    /* 0x4b */ 0x0, /* 4b (Keypad-4/Left) */
    /* 0x4c */ 0x0, /* 4c (Keypad-5) */
    /* 0x4d */ 0x0, /* 4d (Keypad-6/Right) */
    /* 0x4e */ 0x0, /* 4e (Keypad-+) */
    /* 0x4f */ 0x0, /* 4f (Keypad-1/End) */
    /* 0x50 */ 0x0, /* 50 (Keypad-2/Down) */
    /* 0x51 */ 0x0, /* 51 (Keypad-3/PgDn) */
    /* 0x52 */ 0x0, /* 52 (Keypad-0/Ins) */
    /* 0x53 */ 0x0, /* 53 (Keypad-./Del) */
    /* 0x54 */ 0x0, /* 54 (Alt-SysRq) on a 84+ key keyboard */
    /* 0x55 */ 0x0,
    /* 0x56 */ 0x0,
    /* 0x57 */ 0x0,
    /* 0x58 */ 0x0,
    /* 0x59 */ 0x0,
    /* 0x5a */ 0x0,
    /* 0x5b */ 0x0,
    /* 0x5c */ 0x0,
    /* 0x5d */ 0x0,
    /* 0x5e */ 0x0,
    /* 0x5f */ 0x0,
    /* 0x60 */ 0x0,
    /* 0x61 */ 0x0,
    /* 0x62 */ 0x0,
    /* 0x63 */ 0x0,
    /* 0x64 */ 0x0,
    /* 0x65 */ 0x0,
    /* 0x66 */ 0x0,
    /* 0x67 */ KEY_UPARROW,
    /* 0x68 */ 0x0,
    /* 0x69 */ KEY_LEFTARROW,
    /* 0x6a */ KEY_RIGHTARROW,
    /* 0x6b */ 0x0,
    /* 0x6c */ KEY_DOWNARROW,
    /* 0x6d */ 0x0,
    /* 0x6e */ 0x0,
    /* 0x6f */ 0x0,
    /* 0x70 */ 0x0,
    /* 0x71 */ 0x0,
    /* 0x72 */ 0x0,
    /* 0x73 */ 0x0,
    /* 0x74 */ 0x0,
    /* 0x75 */ 0x0,
    /* 0x76 */ 0x0,
    /* 0x77 */ 0x0,
    /* 0x78 */ 0x0,
    /* 0x79 */ 0x0,
    /* 0x7a */ 0x0,
    /* 0x7b */ 0x0,
    /* 0x7c */ 0x0,
    /* 0x7d */ 0x0,
    /* 0x7e */ 0x0,
    /* 0x7f */ KEY_FIRE, //KEY_RCTRL,
};

// Lookup table for mapping ASCII characters to their equivalent when
// shift is pressed on an American layout keyboard:
static const char shiftxform[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127
};

/* Checks whether or not the given file descriptor is associated
   with a local keyboard.
   Returns 1 if it is, 0 if not (or if something prevented us from
   checking). */

int tty_is_kbd(int fd)
{
    int data = 0;

    if (ioctl(fd, KDGKBTYPE, &data) != 0)
        return 0;

    if (data == KB_84) {
        printf("84-key keyboard found.\n");
        return 1;
    } else if (data == KB_101) {
        printf("101-key keyboard found.\n");
        return 1;
    } else {
        printf("KDGKBTYPE = 0x%x.\n", data);
        return 0;
    }
}

static int old_mode = -1;
static struct termios old_term;
static int kb = -1; /* keyboard file descriptor */

void kbd_shutdown(void)
{
    /* Shut down nicely. */

    printf("Cleaning up.\n");
    fflush(stdout);

    printf("Exiting normally.\n");
    if (old_mode != -1) {
        ioctl(kb, KDSKBMODE, old_mode);
        tcsetattr(kb, 0, &old_term);
    }

    if (kb > 3)
        close(kb);

    exit(0);
}

static int kbd_init(void)
{
    struct termios new_term;
    char *files_to_try[] = {"/dev/tty", "/dev/tty0", "/dev/console", NULL};
    int i;
    int flags;
    int found = 0;

    /* First we need to find a file descriptor that represents the
       system's keyboard. This should be /dev/tty, /dev/console,
       stdin, stdout, or stderr. We'll try them in that order.
       If none are acceptable, we're probably not being run
       from a VT. */
    for (i = 0; files_to_try[i] != NULL; i++) {
        /* Try to open the file. */
        kb = open(files_to_try[i], O_RDONLY);
        if (kb < 0) continue;
        /* See if this is valid for our purposes. */
        if (tty_is_kbd(kb)) {
            printf("Using keyboard on %s.\n", files_to_try[i]);
            found = 1;
            break;
        }
        close(kb);
    }

    /* If those didn't work, not all is lost. We can try the
       3 standard file descriptors, in hopes that one of them
       might point to a console. This is not especially likely. */
    if (files_to_try[i] == NULL) {
        for (kb = 0; kb < 3; kb++) {
            if (tty_is_kbd(i)) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        printf("Unable to find a file descriptor associated with "\
                "the keyboard.\n" \
                "Perhaps you're not using a virtual terminal?\n");
        return 1;
    }

    /* Find the keyboard's mode so we can restore it later. */
    if (ioctl(kb, KDGKBMODE, &old_mode) != 0) {
        printf("Unable to query keyboard mode.\n");
        kbd_shutdown();
    }

    /* Adjust the terminal's settings. In particular, disable
       echoing, signal generation, and line buffering. Any of
       these could cause trouble. Save the old settings first. */
    if (tcgetattr(kb, &old_term) != 0) {
        printf("Unable to query terminal settings.\n");
        kbd_shutdown();
    }

    new_term = old_term;
    new_term.c_iflag = 0;
    new_term.c_lflag &= ~(ECHO | ICANON | ISIG);

    /* TCSAFLUSH discards unread input before making the change.
       A good idea. */
    if (tcsetattr(kb, TCSAFLUSH, &new_term) != 0) {
        printf("Unable to change terminal settings.\n");
    }

    /* Put the keyboard in mediumraw mode. */
    if (ioctl(kb, KDSKBMODE, K_MEDIUMRAW) != 0) {
        printf("Unable to set mediumraw mode.\n");
        kbd_shutdown();
    }

    /* Put in non-blocking mode */
    flags = fcntl(kb, F_GETFL, 0);
    fcntl(kb, F_SETFL, flags | O_NONBLOCK);

    printf("Ready to read keycodes. Press Backspace to exit.\n");

    return 0;
}

int kbd_read(int *pressed, unsigned char *key)
{
    unsigned char data;

    if (read(kb, &data, 1) < 1) {
        return 0;
    }

    *pressed = (data & 0x80) == 0x80;
    *key = data & 0x7F;

    /* Print the keycode. The top bit is the pressed/released
       flag, and the lower seven are the keycode. */
    //printf("%s: 0x%2X (%i)\n", *pressed ? "Released" : " Pressed", (unsigned int)*key, (unsigned int)*key);

    return 1;
}

static unsigned char TranslateKey(unsigned char key)
{
	return key;

	/*
    if (key < sizeof(at_to_doom))
        return at_to_doom[key];
    else
        return 0x0;
	*/

    //default:
    //  return tolower(key);
}

// Get the equivalent ASCII (Unicode?) character for a keypress.

static unsigned char GetTypedChar(unsigned char key)
{
    key = TranslateKey(key);

    // Is shift held down?  If so, perform a translation.

    if (shiftdown > 0)
    {
        if (key >= 0 && key < arrlen(shiftxform))
        {
            key = shiftxform[key];
        }
        else
        {
            key = 0;
        }
    }

    return key;
}

static void UpdateShiftStatus(int pressed, unsigned char key)
{
    int change;

    if (pressed) {
        change = 1;
    } else {
        change = -1;
    }

    if (key == KEY_RSHIFT) {
        shiftdown += change;
    }
}


void I_GetEvent(void)
{
    event_t event;
    int pressed;
    unsigned char key;


	while (DG_GetKey(&pressed, &key))
    {
        UpdateShiftStatus(pressed, key);

        // process event

        if (pressed)
        {
            // data1 has the key pressed, data2 has the character
            // (shift-translated, etc)
            event.type = ev_keydown;
            event.data1 = TranslateKey(key);
            event.data2 = GetTypedChar(key);

            if (event.data1 != 0)
            {
                D_PostEvent(&event);
            }
        }
        else
        {
            event.type = ev_keyup;
            event.data1 = TranslateKey(key);

            // data2 is just initialized to zero for ev_keyup.
            // For ev_keydown it's the shifted Unicode character
            // that was typed, but if something wants to detect
            // key releases it should do so based on data1
            // (key ID), not the printable char.

            event.data2 = 0;

            if (event.data1 != 0)
            {
                D_PostEvent(&event);
            }
            break;
        }
    }


                /*
            case SDL_MOUSEMOTION:
                event.type = ev_mouse;
                event.data1 = mouse_button_state;
                event.data2 = AccelerateMouse(sdlevent.motion.xrel);
                event.data3 = -AccelerateMouse(sdlevent.motion.yrel);
                D_PostEvent(&event);
                break;
                */
}

void I_InitInput(void)
{
}

