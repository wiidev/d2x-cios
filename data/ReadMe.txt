/*
 * cIOS d2x v${MAJOR_VER} ${MINOR_VER} by blackb0x
 *
 * Previous releases were by davebaol
 * 
 */  


[ DISCLAIMER ]

  THIS APPLICATION COMES WITH NO WARRANTY AT ALL, NEITHER EXPRESSED NOR
  IMPLIED. NO ONE BUT YOURSELF IS RESPONSIBLE FOR ANY DAMAGE TO YOUR WII
  CONSOLE BECAUSE OF A IMPROPER USAGE OF THIS SOFTWARE. 


[ DESCRIPTION ]

  This is a Custom IOS, an IOS modified to add some new features
  not available in the official IOS.

  This IOS has been made to be used ONLY with homebrew software.

  The d2x cIOS is an enhanced version of the cIOSX rev21 by Waninkoko. 
  To see what's new read the file Changelog.txt or visit the d2x GitHub
  page: https://github.com/wiidev/d2x-cios


[ HOW TO INSTALL IT ]

  You can install d2x cios through ModMii or d2x-cios-installer.
  
  ModMii:

  - If you're going to install a d2x vWii edition don't use ModMii. In this
    case use the d2x-cios-installer (see below).
  - Download and install ModMii v6.2.3 or higher on your PC. Download link:
    http://gbatemp.net/topic/207126-modmii-for-windows
  - From ModMii's Main Menu, enter "4", then "beta"
  - Select the d2x beta you want to build
  - Mark some or all d2x cIOSs for download (i.e. "d2x")
  - Enter "D" then "Y" to build d2x beta cIOSs\WADs
  - Install cIOSs using a WAD Manager - i.e. WiiMod\MMM\YAWMM (available on 
    ModMii's Download Page 2)

  d2x-cios-installer:

  - Download the latest d2x-cios-installer from its google code page:
    http://code.google.com/p/d2x-cios-installer/downloads/list
  - Extract it into the apps folder of your sd card or usb device
  - Extract d2x-v${MAJOR_VER}-${MINOR_VER}.zip on your sd card or usb device
    into the folder /apps/d2x-cios-installer.
    NOTE: This will overwrite the file /apps/d2x-cios-installer/ciosmaps.xml
    possibly present in that folder. You might want to rename it before
    extracting the d2x package. 
  - Launch the Homebrew Channel, start the installer and follow the 
    instructions on the screen


[ KUDOS ]

- rodries, for the help with EHCI improvements.
- Crediar, for all I learned studying Sneek source code.
- Oggzee, for his brilliant fraglist.
- WiiPower, for the great help with ios reload block from usb.
- dragbe and NutNut, for their d2x cios installer.
- XFlak, for his wonderful ModMii which supported d2x wads since its birth.
  Without ModMii d2x cios would probably never have existed. Also, XFlak had 
  the original idea to replace the buggy EHCI module of cIOSX rev21 with the 
  working one from rev19. 
- HackWii.it and GBAtemp.net communities, for their ideas and support.
- Totoro, for the official d2x logo
- ChaN, for his FatFs.
- Waninkoko, for his cIOSX rev21.
- Team Twiizers and devkitPRO devs for their great work in libogc.
- WiiGator, for his work in the DIP plugin.
- kwiirk, for his EHCI module.
- Hermes, for his EHCI improvements.
- neimod, for the Custom IOS module.
- All the betatesters.
