::d2x-beta.bat should be saved here in order for ModMii v4.6.0+ to use it: modules\More-cIOSs\{ANY FOLDER NAME}\d2x-beta.bat

::--------update what comes after "d2x-beta-rev=" and "ciosversion=" below----------

set ciosversion=${IOS_REV}
::must be <=65535
::if ciosversion above is different from version in ciosmaps.xml, then ModMii v6.5.2 or higher required to build it successfully

set d2x-beta-rev=${MAJOR_VER}-${MINOR_VER}
::This info is used to sign *00.app file for cIOS detection, everything after "=" but before the first "-" is considered the version # and it must be a number! Good examples are "5-beta1-test1" or "5" or "999". Bad examples are "5beta1" or "five-beta1" or "-5-beta1"
::everything after the first "-" is considered the version string (limited by 16 chars) and it only recognizes letters, numbers and + - _ ( ) [ ]

set cIOSFamilyName=d2x
::Only change this if building a cIOS that isn't a "d2x" branded cIOS. This string is limited by 16 chars and it only recognizes letters, numbers and + - _ ( ) [ ]


if "%wadname%"=="" goto:endofd2xbat

::------------------------Section to update (below)--------------------
::here you should enter the new beta cIOS hashes (under the OLD non-beta "wadname")
if /i "%wadname:~0,17%" EQU "cIOS249[37]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[37]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[38]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[38]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[53]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[53]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[55]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[55]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[56]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[56]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[57]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[57]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[58]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[58]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[60]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[60]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[70]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[70]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS249[80]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS250[80]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
if /i "%wadname:~0,17%" EQU "cIOS251[38]-d2x-v" set md5=ac0ffee1eddad70defec81005e570015
::------------------------Section to update (above)--------------------

::----------Additional Instructions---------------
::save any of the following beta files to the following directory
::if any of the following is not found, the standard d2x module used in the previous ModMii release will continue to be used
::modules\More-cIOSs\{ANY FOLDER NAME}\MLOAD.app
::modules\More-cIOSs\{ANY FOLDER NAME}\FAT.app
::modules\More-cIOSs\{ANY FOLDER NAME}\EHCI.app
::modules\More-cIOSs\{ANY FOLDER NAME}\USBS.app
::modules\More-cIOSs\{ANY FOLDER NAME}\DIPP.app
::modules\More-cIOSs\{ANY FOLDER NAME}\ES.app
::modules\More-cIOSs\{ANY FOLDER NAME}\FFSP.app
::------------------------------------------------------

set md5alt=%md5%

::change some variables
set wadname=%wadname:~0,17%%d2x-beta-rev%
if /i "%name:~0,17%" NEQ "Advanced Download" set name=%wadname%

:endofd2xbat 

