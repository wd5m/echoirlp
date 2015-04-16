.mute  `[`. | 

&lt;callsign&gt;

 ... | -r | -c | -u | -s | -t | -a | -e]

.unmute `[` 

&lt;callsign&gt;

 ... | -r | -c | -u | -s | -t | -a | -e]

These variations of the .mute and .unmute commands affect a group of stations
at a time and additionally put the conference into a mode were new connections
from stations of that type are automatically muted as well.  These commands
are primarily of use for controlling large nets such as the N2LEN 9/11 net.

Where the various suffixes mean:
  * -a: All users (except yourself)
  * -c: tbd conferences
  * -e: Echolink conferences
  * -r: RF users (-R and -L stations)
  * -s: Sysops and Admins
  * -t: Station talking
  * -u: PC users

You can use more than one option in a command so if you want to mute
both RF users and PC users, but not conferences and sysops as well as a
specific conference you could enter
.mute -ru `*`echotest`*`

The station that is currently talking are **NOT** muted by these commands unless
the "t" flag is included.

These are additive, if you mute conferences in one command and then mute rf
users in the next command both remain muted.









