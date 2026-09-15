===============================================================================
  CSPSP Server                                                     Kevin Chen
  version 1.51b                                                          2011		
  
  official site: http://cspsp.appspot.com
  forums:        http://z4.invisionfree.com/CSPSP
===============================================================================

This the server application for CSPSP, a homebrew game for the Sony PSP.

-------------------------------------------------------------------------------
  INSTALLATION/SETUP
-------------------------------------------------------------------------------

This application requires the Microsoft .NET Framework. Please make sure you
have it installed before continuing (http://www.microsoft.com/net).

1. Extract the CSPSPServer folder to any location on your harddrive
2. Open config.txt to customize your server's settings (see the following
   section for more information).
3. If you are behind a router, portforward the port specified in config.txt
   (default is 42692). Check out this very helpful tutorial by xXxSpectre@:
   http://z4.invisionfree.com/CSPSP/index.php?showtopic=150.
4. Run CSPSPServer.exe! If everything went smoothly, you should now be able to
   connect to your server from your psp.
   

-------------------------------------------------------------------------------
  CONFIGURATION
-------------------------------------------------------------------------------

To configure your server, open up data/config.txt. Inside, you'll find a few 
settings that you can change. They should be pretty self-explanatory, but here
are a few short descriptions:

"name"           - the name of your server (max 32 characters)
"autobalance"    - enables/disables team auto-balance ["on"/"off"]
"friendlyfire"   - enables/disables friendly fire ["on"/"off"]
"alltalk"        - enables/disables chat between dead players/spectators and 
                    players still alive ["on"/"off"]
"maxplayers"     - the maximum number of players that can join the server 
                    (max 32)
"bots"           - enables/disables server-owned bots ["on"/"off"]
"roundtime"      - the duration of a round in seconds (minimum 10)
"buytime"        - the time in the beginning of a round allowed to buy weapons
"maptime"        - the duration of a map in minutes (maximum 120)
"port"           - the port that the server will use (between 1024 and 65535)
"respawntime"    - the time it takes someone to respawn after dying in seconds
                    (max 30)
"spawngun"       - the index of the gun you want players to spawn with in ctf
                    and ffa (for example, 11 is the MP5; -1 is just the 
                    default pistols)
"invincibletime" - the time a player is invincible for after respawning in 
                     seconds (max 10)

Server-authoritative mode and bot settings are stored in
data/gameplay_config.txt:

"infected_bot_count" - number of T/Zombie bots (default 1)
"ct_bot_count"       - number of CT survivor bots (default 3)
"round_freeze_time"  - freeze time before a round in seconds (0-10)
"infection_respawn_delay" - converted-player respawn delay (0-4)
"horde_regroup"      - keep survivors "inplace" or move them to "base"
"horde_wave_delay"   - regroup/buy time between Horde waves (1-30)
"horde_revive_survivors" - revive eliminated CTs after a cleared wave (on/off)

The two bot counts are limited together by "maxplayers". In Infection and
Horde they are reapplied as the starting T and CT bot teams each round. If
Infection has zero infected bots, one participating human starts as T.
T bots use T map spawns and CT bots use CT map spawns. Spawn markers are used
round-robin when a team has more bots than available markers. Maps missing a
required team spawn set are rejected instead of spawning players at (0,0).

Bot display names are read from data/bot_names.txt, one name per line. Blank
lines and lines beginning with "#" are ignored, and names are limited to 31
characters. The server cycles the list with numeric suffixes when there are
more configured bots than names. If the file is missing or empty, built-in
fallback names are used. The total bot count is derived from
"infected_bot_count" plus "ct_bot_count"; there is no separate botcount key.

To change the map cycle, open up data/mapcycle.txt. List the maps in the order 
that you want them to cycle in, in the format [map name] [type] on each line, 
where type is "tdm", "ctf", "ffa", "extermination", "infection", or
"horde". NETVERSION 11 sends each mode explicitly; older clients and servers
are not protocol-compatible with this project.

In Horde mode, server bots form the T wave and human players are CT survivors.
Clearing a wave preserves ground weapons, heals/rewards living CTs, respawns
the T bots, and synchronizes the wave state. Eliminated CTs remain dead unless
"horde_revive_survivors" is on. Survivors remain in place or regroup at CT
base according to "horde_regroup".
If every CT dies or the round timer expires, the run restarts at wave 1. The
server refills survivor ammunition authoritatively and synchronizes it through
the NETVERSION 11 ammo update packet.

In Infection mode every eliminated CT changes to T and respawns after the
configured infection delay. T players have 150 health; CT players have 100.

To manually ban players, open up data/banlist.txt and enter their name*, with 
one name per line.

To add admins, open up data/admins.txt and enter their name*, with one name 
per line. Admins basically have access to the same commands as the server 
owner (such as kick, ban, etc), except they can do so remotely while in-game.

The server loads up to 128 sequential weapons from data/guns.txt. To add a
normal primary, secondary, knife, or existing grenade type, append its
18-field row and increase the count on the first line; no server source table
needs editing. IDs must equal their row positions and remain in the range
0-127. The matching client guns.txt row and client sprites/sounds must also
exist. Fire mode, pellets, scope, buy category, and team availability are all
sent by the server and enforced from this file. Server bots automatically
select from eligible primary weapons. New grenade effect classes beyond the
existing flash, HE, and smoke effects still require gameplay code because the
current data format does not define effect behavior.
  
* name refers to the player's account name, without the clan tag. For example,
  the account name of someone named "[clan]name" would just be "name".


-------------------------------------------------------------------------------
  COMMANDS
-------------------------------------------------------------------------------

Commands can be entered in the input line (indicated by ">") at the bottom of 
the server, Here's a list of the available commands:

/help                 - lists available commands and their arguments
/timeleft             - shows remaining time left for current map
/kick [name]          - kicks player with specified name*
/ban [name]           - bans+kicks player with specified name*
/unban [name]         - unbans player with specified name*
/map [mapname] [type] - changes to a new map (tdm, ctf, ffa, extermination,
                        infection, or horde; default is tdm)
/resetround           - starts a new round and resets scores
normal text           - sends a server message to all players

* name refers to the player's account name, without the clan tag. For example,
  the account name of someone named "[clan]name" would just be "name".


-------------------------------------------------------------------------------
  COMMON ERRORS
-------------------------------------------------------------------------------

Here is a list of a few error messages and their explanations/solutions:

"Error: Map could not be loaded" 
   - this probably means that the map itself has problems.
   
"Error registering server: Server already registered"
   - the server was not unregistered correctly the last time it was closed. 
     This is more of a warning than an error; the server will still function.
   
"Error registering server: Version outdated"
   - a new version of the server application is available. It's recommended 
     that you update as soon as possible.
   
"Error contacting master server"
   - the master server that holds the list of servers is unavailable 
     (or your internet connection isn't working). The server will still run, 
     and players who have your server saved in favorites might be able to 
     still connect.
   
"Error registering server: Supplied IP does not match"
   - the IP sent to the master server differs from the IP that the master 
     server sees. This error is usually uncommon; try restarting the server 
     application.


-------------------------------------------------------------------------------
  UPDATES
-------------------------------------------------------------------------------

-added support for CSPSP version 1.91
-fixed freezing bug from v1.50
-tweaked networking code to be more robust (and caught a few bugs); also fixed 
  major exploits (speedhacking, infinite ammo, etc.)
-added a time slowdown effect at the end of a round
