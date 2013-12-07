#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <stdarg.h>
#include <fcntl.h>

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* This is the master Users and Groups config for the platform.
** DO NOT EVER RENUMBER.
*/

#define AID_ROOT             0  /* traditional unix root user */

#define AID_SYSTEM        1000  /* system server */

#define AID_RADIO         1001  /* telephony subsystem, RIL */
#define AID_BLUETOOTH     1002  /* bluetooth subsystem */
#define AID_GRAPHICS      1003  /* graphics devices */
#define AID_INPUT         1004  /* input devices */
#define AID_AUDIO         1005  /* audio devices */
#define AID_CAMERA        1006  /* camera devices */
#define AID_LOG           1007  /* log devices */
#define AID_COMPASS       1008  /* compass device */
#define AID_MOUNT         1009  /* mountd socket */
#define AID_WIFI          1010  /* wifi subsystem */
#define AID_ADB           1011  /* android debug bridge (adbd) */
#define AID_INSTALL       1012  /* group for installing packages */
#define AID_MEDIA         1013  /* mediaserver process */
#define AID_DHCP          1014  /* dhcp client */
#define AID_SDCARD_RW     1015  /* external storage write access */
#define AID_VPN           1016  /* vpn system */
#define AID_KEYSTORE      1017  /* keystore subsystem */
#define AID_USB           1018  /* USB devices */
#define AID_DRM           1019  /* DRM server */
#define AID_DRMIO         1020  /* DRM IO server */
#define AID_GPS           1021  /* GPS daemon */
#define AID_UNUSED1       1022  /* deprecated, DO NOT USE */
#define AID_RFU1          1023  /* RFU */
#define AID_RFU2          1024  /* RFU */
#define AID_NFC           1025  /* nfc subsystem */

#define AID_SHELL         2000  /* adb and debug shell user */
#define AID_CACHE         2001  /* cache access */
#define AID_DIAG          2002  /* access to diagnostic resources */

/* The 3000 series are intended for use as supplemental group id's only.
 * They indicate special Android capabilities that the kernel is aware of. */
#define AID_NET_BT_ADMIN  3001  /* bluetooth: create any socket */
#define AID_NET_BT        3002  /* bluetooth: create sco, rfcomm or l2cap sockets */
#define AID_INET          3003  /* can create AF_INET and AF_INET6 sockets */
#define AID_NET_RAW       3004  /* can create raw INET sockets */
#define AID_NET_ADMIN     3005  /* can configure interfaces and routing tables. */

#define AID_MISC          9998  /* access to misc storage */
#define AID_NOBODY        9999

#define AID_APP          10000 /* first app user */

#if !defined(EXCLUDE_FS_CONFIG_STRUCTURES)
struct android_id_info {
    const char *name;
    unsigned aid;
};

static const struct android_id_info android_ids[] = {
    { "root",      AID_ROOT, },
    { "system",    AID_SYSTEM, },
    { "radio",     AID_RADIO, },
    { "bluetooth", AID_BLUETOOTH, },
    { "graphics",  AID_GRAPHICS, },
    { "input",     AID_INPUT, },
    { "audio",     AID_AUDIO, },
    { "camera",    AID_CAMERA, },
    { "log",       AID_LOG, },
    { "compass",   AID_COMPASS, },
    { "mount",     AID_MOUNT, },
    { "wifi",      AID_WIFI, },
    { "dhcp",      AID_DHCP, },
    { "adb",       AID_ADB, },
    { "install",   AID_INSTALL, },
    { "media",     AID_MEDIA, },
    { "drm",       AID_DRM, },
    { "drmio",     AID_DRMIO, },
    { "nfc",       AID_NFC, },
    { "shell",     AID_SHELL, },
    { "cache",     AID_CACHE, },
    { "diag",      AID_DIAG, },
    { "net_bt_admin", AID_NET_BT_ADMIN, },
    { "net_bt",    AID_NET_BT, },
    { "sdcard_rw", AID_SDCARD_RW, },
    { "vpn",       AID_VPN, },
    { "keystore",  AID_KEYSTORE, },
    { "usb",       AID_USB, },
    { "gps",       AID_GPS, },
    { "inet",      AID_INET, },
    { "net_raw",   AID_NET_RAW, },
    { "net_admin", AID_NET_ADMIN, },
    { "misc",      AID_MISC, },
    { "nobody",    AID_NOBODY, },
};

#define android_id_count \
    (sizeof(android_ids) / sizeof(android_ids[0]))
    
struct fs_path_config {
    unsigned mode;
    unsigned uid;
    unsigned gid;
    const char *prefix;
};

/* Rules for directories.
** These rules are applied based on "first match", so they
** should start with the most specific path and work their
** way up to the root.
*/

static struct fs_path_config android_dirs[] = {
    { 00770, AID_SYSTEM, AID_CACHE,  "cache" },
    { 00771, AID_SYSTEM, AID_SYSTEM, "data/app" },
    { 00771, AID_SYSTEM, AID_SYSTEM, "data/app-private" },
    { 00771, AID_SYSTEM, AID_SYSTEM, "data/dalvik-cache" },
    { 00771, AID_SYSTEM, AID_SYSTEM, "data/data" },
    { 00771, AID_SHELL,  AID_SHELL,  "data/local/tmp" },
    { 00771, AID_SHELL,  AID_SHELL,  "data/local" },
    { 01771, AID_SYSTEM, AID_MISC,   "data/misc" },
    { 00770, AID_DHCP,   AID_DHCP,   "data/misc/dhcp" },
    { 00771, AID_SYSTEM, AID_SYSTEM, "data" },
    { 00750, AID_ROOT,   AID_SHELL,  "sbin" },
    { 00755, AID_ROOT,   AID_SHELL,  "system/bin" },
    { 00755, AID_ROOT,   AID_SHELL,  "system/vendor" },
    { 00755, AID_ROOT,   AID_SHELL,  "system/xbin" },
    { 00755, AID_ROOT,   AID_ROOT,   "system/etc/ppp" },
    { 00777, AID_ROOT,   AID_ROOT,   "sdcard" },
    { 00755, AID_ROOT,   AID_ROOT,   0 },
};

/* Rules for files.
** These rules are applied based on "first match", so they
** should start with the most specific path and work their
** way up to the root. Prefixes ending in * denotes wildcard
** and will allow partial matches.
*/
static struct fs_path_config android_files[] = {
    { 00440, AID_ROOT,      AID_SHELL,     "system/etc/init.goldfish.rc" },
    { 00550, AID_ROOT,      AID_SHELL,     "system/etc/init.goldfish.sh" },
    { 00440, AID_ROOT,      AID_SHELL,     "system/etc/init.trout.rc" },
    { 00550, AID_ROOT,      AID_SHELL,     "system/etc/init.ril" },
    { 00550, AID_ROOT,      AID_SHELL,     "system/etc/init.testmenu" },
    { 00550, AID_DHCP,      AID_SHELL,     "system/etc/dhcpcd/dhcpcd-run-hooks" },
    { 00440, AID_BLUETOOTH, AID_BLUETOOTH, "system/etc/dbus.conf" },
    { 00440, AID_BLUETOOTH, AID_BLUETOOTH, "system/etc/bluetooth/main.conf" },
    { 00440, AID_BLUETOOTH, AID_BLUETOOTH, "system/etc/bluetooth/input.conf" },
    { 00440, AID_BLUETOOTH, AID_BLUETOOTH, "system/etc/bluetooth/audio.conf" },
    { 00444, AID_NET_BT,    AID_NET_BT,    "system/etc/bluetooth/blacklist.conf" },
    { 00640, AID_SYSTEM,    AID_SYSTEM,    "system/etc/bluetooth/auto_pairing.conf" },
    { 00444, AID_RADIO,     AID_AUDIO,     "system/etc/AudioPara4.csv" },
    { 00555, AID_ROOT,      AID_ROOT,      "system/etc/ppp/*" },
    { 00555, AID_ROOT,      AID_ROOT,      "system/etc/rc.*" },
    { 00644, AID_SYSTEM,    AID_SYSTEM,    "data/app/*" },
    { 00644, AID_SYSTEM,    AID_SYSTEM,    "data/app-private/*" },
    { 00644, AID_APP,       AID_APP,       "data/data/*" },
        /* the following two files are INTENTIONALLY set-gid and not set-uid.
         * Do not change. */
    { 02755, AID_ROOT,      AID_NET_RAW,   "system/bin/ping" },
    { 02750, AID_ROOT,      AID_INET,      "system/bin/netcfg" },
    	/* the following five files are INTENTIONALLY set-uid, but they
	 * are NOT included on user builds. */
    { 06755, AID_ROOT,      AID_ROOT,      "system/xbin/su" },
    { 06755, AID_ROOT,      AID_ROOT,      "system/xbin/librank" },
    { 06755, AID_ROOT,      AID_ROOT,      "system/xbin/procrank" },
    { 06755, AID_ROOT,      AID_ROOT,      "system/xbin/procmem" },
    { 06755, AID_ROOT,      AID_ROOT,      "system/xbin/tcpdump" },
    { 04770, AID_ROOT,      AID_RADIO,     "system/bin/pppd-ril" },
		/* the following file is INTENTIONALLY set-uid, and IS included
		 * in user builds. */
    { 06750, AID_ROOT,      AID_SHELL,     "system/bin/run-as" },
    { 00755, AID_ROOT,      AID_SHELL,     "system/bin/*" },
    { 00755, AID_ROOT,      AID_SHELL,     "system/xbin/*" },
    { 00755, AID_ROOT,      AID_SHELL,     "system/vendor/bin/*" },
    { 00750, AID_ROOT,      AID_SHELL,     "sbin/*" },
    { 00755, AID_ROOT,      AID_ROOT,      "bin/*" },
    { 00750, AID_ROOT,      AID_SHELL,     "init*" },
    { 00644, AID_ROOT,      AID_ROOT,       0 },
};

static inline void fs_config(const char *path, int dir,
                             unsigned *uid, unsigned *gid, unsigned *mode)
{
    struct fs_path_config *pc;
    int plen;
    
    pc = dir ? android_dirs : android_files;
    plen = strlen(path);
    for(; pc->prefix; pc++){
        int len = strlen(pc->prefix);
        if (dir) {
            if(plen < len) continue;
            if(!strncmp(pc->prefix, path, len)) break;
            continue;
        }
        /* If name ends in * then allow partial matches. */
        if (pc->prefix[len -1] == '*') {
            if(!strncmp(pc->prefix, path, len - 1)) break;
        } else if (plen == len){
            if(!strncmp(pc->prefix, path, len)) break;
        }
    }
    *uid = pc->uid;
    *gid = pc->gid;
    *mode = (*mode & (~07777)) | pc->mode;
    
#if 0
    fprintf(stderr,"< '%s' '%s' %d %d %o >\n", 
            path, pc->prefix ? pc->prefix : "", *uid, *gid, *mode);
#endif
}
#endif

/* NOTES
**
** - see buffer-format.txt from the linux kernel docs for
**   an explanation of this file format
** - dotfiles are ignored
** - directories named 'root' are ignored
** - device notes, pipes, etc are not supported (error)
*/

void die(const char *why, ...)
{
    va_list ap;

    va_start(ap, why);
    fprintf(stderr,"error: ");
    vfprintf(stderr, why, ap);
    fprintf(stderr,"\n");
    va_end(ap);
    exit(1);
}

static int verbose = 0;
static int total_size = 0;

static void fix_stat(const char *path, struct stat *s)
{
    fs_config(path, S_ISDIR(s->st_mode), &s->st_uid, &s->st_gid, &s->st_mode);
}

static void _eject(struct stat *s, char *out, int olen, char *data, unsigned datasize)
{
    // Nothing is special about this value, just picked something in the
    // approximate range that was being used already, and avoiding small
    // values which may be special.
    static unsigned next_inode = 300000;

    while(total_size & 3) {
        total_size++;
        putchar(0);
    }

    fix_stat(out, s);
//    fprintf(stderr, "_eject %s: mode=0%o\n", out, s->st_mode);

    printf("%06x%08x%08x%08x%08x%08x%08x"
           "%08x%08x%08x%08x%08x%08x%08x%s%c",
           0x070701,
           next_inode++,  //  s.st_ino,
           s->st_mode,
           0, // s.st_uid,
           0, // s.st_gid,
           1, // s.st_nlink,
           0, // s.st_mtime,
           datasize,
           0, // volmajor
           0, // volminor
           0, // devmajor
           0, // devminor,
           olen + 1,
           0,
           out,
           0
           );

    total_size += 6 + 8*13 + olen + 1;

    if(strlen(out) != olen) die("ACK!");

    while(total_size & 3) {
        total_size++;
        putchar(0);
    }

    if(datasize) {
        fwrite(data, datasize, 1, stdout);
        total_size += datasize;
    }
}

static void _eject_trailer()
{
    struct stat s;
    memset(&s, 0, sizeof(s));
    _eject(&s, "TRAILER!!!", 10, 0, 0);

    while(total_size & 0xff) {
        total_size++;
        putchar(0);
    }
}

static void _archive(char *in, char *out, int ilen, int olen);

static int compare(const void* a, const void* b) {
  return strcmp(*(const char**)a, *(const char**)b);
}

static void _archive_dir(char *in, char *out, int ilen, int olen)
{
    int i, t;
    DIR *d;
    struct dirent *de;

    if(verbose) {
        fprintf(stderr,"_archive_dir('%s','%s',%d,%d)\n",
                in, out, ilen, olen);
    }

    d = opendir(in);
    if(d == 0) die("cannot open directory '%s'", in);

    int size = 32;
    int entries = 0;
    char** names = malloc(size * sizeof(char*));
    if (names == NULL) {
      fprintf(stderr, "failed to allocate dir names array (size %d)\n", size);
      exit(1);
    }

    while((de = readdir(d)) != 0){
            /* xxx: feature? maybe some dotfiles are okay */
        if(de->d_name[0] == '.') continue;

            /* xxx: hack. use a real exclude list */
        if(!strcmp(de->d_name, "root")) continue;

        if (entries >= size) {
          size *= 2;
          names = realloc(names, size * sizeof(char*));
          if (names == NULL) {
            fprintf(stderr, "failed to reallocate dir names array (size %d)\n",
                    size);
            exit(1);
          }
        }
        names[entries] = strdup(de->d_name);
        if (names[entries] == NULL) {
          fprintf(stderr, "failed to strdup name \"%s\"\n",
                  de->d_name);
          exit(1);
        }
        ++entries;
    }

    qsort(names, entries, sizeof(char*), compare);

    for (i = 0; i < entries; ++i) {
        t = strlen(names[i]);
        in[ilen] = '/';
        memcpy(in + ilen + 1, names[i], t + 1);

        if(olen > 0) {
            out[olen] = '/';
            memcpy(out + olen + 1, names[i], t + 1);
            _archive(in, out, ilen + t + 1, olen + t + 1);
        } else {
            memcpy(out, names[i], t + 1);
            _archive(in, out, ilen + t + 1, t);
        }

        in[ilen] = 0;
        out[olen] = 0;

        free(names[i]);
    }
    free(names);
}

static void _archive(char *in, char *out, int ilen, int olen)
{
    struct stat s;

    if(verbose) {
        fprintf(stderr,"_archive('%s','%s',%d,%d)\n",
                in, out, ilen, olen);
    }

    if(lstat(in, &s)) die("could not stat '%s'\n", in);

    if(S_ISREG(s.st_mode)){
        char *tmp;
        int fd;

        fd = open(in, O_RDONLY);
        if(fd < 0) die("cannot open '%s' for read", in);

        tmp = (char*) malloc(s.st_size);
        if(tmp == 0) die("cannot allocate %d bytes", s.st_size);

        if(read(fd, tmp, s.st_size) != s.st_size) {
            die("cannot read %d bytes", s.st_size);
        }

        _eject(&s, out, olen, tmp, s.st_size);

        free(tmp);
        close(fd);
    } else if(S_ISDIR(s.st_mode)) {
        _eject(&s, out, olen, 0, 0);
        _archive_dir(in, out, ilen, olen);
    } else if(S_ISLNK(s.st_mode)) {
        char buf[1024];
        int size;
        size = readlink(in, buf, 1024);
        if(size < 0) die("cannot read symlink '%s'", in);
        _eject(&s, out, olen, buf, size);
    } else {
        die("Unknown '%s' (mode %d)?\n", in, s.st_mode);
    }
}

void archive(const char *start, const char *prefix)
{
    char in[8192];
    char out[8192];

    strcpy(in, start);
    strcpy(out, prefix);

    _archive_dir(in, out, strlen(in), strlen(out));
}

int main(int argc, char *argv[])
{
    argc--;
    argv++;

    if(argc == 0) die("no directories to process?!");

    while(argc-- > 0){
        char *x = strchr(*argv, '=');
        if(x != 0) {
            *x++ = 0;
        } else {
            x = "";
        }

        archive(*argv, x);

        argv++;
    }

    _eject_trailer();

    return 0;
}


