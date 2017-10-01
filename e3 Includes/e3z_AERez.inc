|----------------------------------------------------------------------------------------------------| 
| File: aerez.mac
| 
| Auto Area Resurrect Macro v1.2
| by Skomag 
|
| This macro can be used to automatically ressurect all corpses in range.
|
| To start an AE Rez, use the following in a macro or on the command line:
|   /echo aerez
|
| To stop an AE Rez, either /end the macro or use the following in a macro or on the command line:
|   /echo aerezstop
|
| - The corpse will first be /considered to see if it can be rezed.
| - If the corpse cannot be rezed, it will be added to an ignore list and not targeted again.
| - Each corpse in range will be rezed one time, then a 20s pause, then try all corpses again.
| - To disable the loop and only rez each corpse once, set the CustomRezDelay to 0 (see below)
| - Before casting spell, the macro will /say that it is rezing and send a 'rezinc' tell.
| - If the character is not online, the spell will abort (turn on QuietRez to disable)
|
|
| The following options can be set by /declaring a variable in Sub Main before the loop.
|
| - Use a custom spell to rez.  Default: epic first, then Reviviscence.
|   /declare UseRezSpell string outer "Spell Name"
| - Auto rez a corpse on a manual /consider.  Default: will not cast
|   /declare AlwaysRezOnCon bool outer TRUE
| - Do not /say or send a /tell (disables offline interruption)  Default: will /say and /tell
|   /declare QuietRez bool outer TRUE
| - Delay between rezing corpses again - set to 0 to only try once.  Default: 20 seconds
|   /declare CustomRezDelay string outer 0
|
| This macro can also be used as an include file
| 1) Rename aerez.mac to aerez.inc
| 2) Add any /declare options to Sub Main before the loop, if any
| 3) Remove or comment out the Sub Main part of this file
| 4) Remove spell_routines.inc if your macro already includes it
| 
|----------------------------------------------------------------------------------------------------|

#include spell_routines.inc

Sub Main 

	| Options, see above
	|/declare QuietRez bool outer TRUE
	|/delcare UseRezSpell string outer "Spell Name"
	|/declare CustomRezDelay 0
	/declare AlwaysRezOnCon bool outer TRUE

	| This macro is stand alone, so we start auto rezing
	/call Event_StartAERez

  :loop    
    /doevents 
  /goto :loop 

/return 

|----------------------------------------------------------------------------------------------------| 

#event StartAERez "[MQ2] aerez"
#event StopAERez  "[MQ2] aerezstop"
#event CanRez     "This corpse can be resurrected."
#event CannotRez  "This corpse cannot be resurrected."
#event NotOnline  "You told #1#, '#2# is not online at this time'"
#event NotOnline  "Your tell to #1# has been queued"

Sub Event_StartAERez
	/if (${DoingAERez}) /return
	/echo  AE Rez Started - to stop: /echo aerezstop
	/popup AE Rez Started - to stop: /echo aerezstop

	/declare RezRange int local 100
	/declare Delay string local 10s
	/if (${CustomRezDelay}) /varset Delay ${CustomRezDelay}
	/if (!${Defined[RezTimer]}) /declare RezTimer timer outer
	/if (!${Defined[RezIgnoreCount]}) /declare RezIgnoreCount int outer 0
	/if (!${Defined[DoingAERez]}) /declare DoingAERez bool outer
	/varset DoingAERez TRUE
	/declare x int local
	/declare y int local
	/declare CorpseCount int local
	/declare SkipCorpse bool local
	/declare CurID int local
	
	:loop
		:waiting
			/if (!${DoingAERez}) /return
			/doevents
		/if (${RezTimer}) /goto :waiting

		/echo Starting Rez Loop

		| find all corpses in range	
		/varset CorpseCount ${SpawnCount[radius ${RezRange}]}
		/if (!${CorpseCount}) {
			/if (${Delay.Equal[0]}) /varset DoingAERez FALSE
			/varset RezTimer ${Delay}
			/goto :loop			
		}
		/if (${Defined[CorpseList]}) /deletevar CorpseList
		/declare CorpseList[${CorpseCount}] int local
		/for x 1 to ${CorpseCount}
			/varset CorpseList[${x}] ${NearestSpawn[${x}, radius ${RezRange}].ID}
		/next x
		| target and consider consider each corpse if not in ignore list
		/for x 1 to ${CorpseCount}
			/if (!${DoingAERez}) /return
			/varset CurID ${CorpseList[${x}]}
			| skip if not a corpse
			/varset SkipCorpse FALSE
			/if (${Spawn[id ${CurID}].Type.NotEqual[Corpse]} && ${Spawn[id ${CurID}].Type.NotEqual[Item]} && ${Spawn[id ${CurID}].Animation} != 16) /varset SkipCorpse TRUE
			/if (${Spawn[id ${CurID}].CleanName.Count[ ]} < 1 || ${Spawn[id ${CurID}].CleanName.Arg[2].NotEqual[corpse]}) /varset SkipCorpse TRUE
			| check ignore list for this corpse
			/if (${RezIgnoreCount}) {
				/for y 1 to ${RezIgnoreCount}
					/if (${CorpseList[${x}]} == ${RezIgnoreList[${y}]}) /varset SkipCorpse TRUE
				/next y
			}
			/if (!${SkipCorpse}) {
|/echo ${CurID} ${Spawn[id ${CurID}]} ${Spawn[id {$CurID}].Type} ${Spawn[id ${CurID}].Animation}
				/squelch /target id ${Spawn[id ${CurID}]}
				/delay 2s ${Target.ID}==${CurID}
				/consider
				/delay 2s
				/doevents
				/delay 1s
			}
		/next x
		/call CleanIgnoreList
		| if CustomRezDelay option is set to 0, end the ae rez - otherwise, wait for delay and rerez
		/if (${Delay.Equal[0]}) /varset DoingAERez FALSE
		/varset RezTimer ${Delay}
	/goto :loop
/return

Sub Event_StopAERez
	/echo  AE Rez Stopped - to start again: /echo aerez
	/popup AE Rez Stopped - to start again: /echo aerez
	/if (${Defined[DoingAERez]}) /varset DoingAERez FALSE
/return

|----------------------------------------------------------------------------------------------------| 


Sub Event_CanRez
	/declare TellMessage string local rezinc
	/declare SpellGem string local gem7
	
	/if (${Target.ID} && (${DoingAERez} || ${AlwaysRezOnCon})) {
		/if (!${Defined[DoingARez]}) /declare DoingARez bool outer
		/varset DoingARez TRUE
		| announce the rez if quiet option not set
		/if (!${QuietRez}) {
			/tell ${Target.CleanName.Arg[1,']} ${TellMessage}
		}

		| use custom spell if defined, otherwise epic, or fall back to Reviviscence
		/if (${Defined[UseRezSpell]} && ${Spell[${UseRezSpell}].ID}) {
			/call Cast "${UseRezSpell}" ${SpellGem} 20 CheckOnline
			/if (${Macro.Return.Equal[CAST_SUCCESS]}) {
				/call AddIgnoreList ${Target.ID}
				/bc Rezzed ${Target.Name}
			}
		} else /if (${FindItem[Water Sprinkler of Nem Ankh].InvSlot}) {
			/call Cast "Water Sprinkler of Nem Ankh" item 20 CheckOnline
			/if (${Macro.Return.Equal[CAST_SUCCESS]}) {
				/call AddIgnoreList ${Target.ID}
				/bc Rezzed ${Target.Name}
			}
		} else {
			/call Cast "Reviviscence" ${SpellGem} 20 CheckOnline
			/if (${Macro.Return.Equal[CAST_SUCCESS]}) {
				/call AddIgnoreList ${Target.ID}
				/bc Rezzed ${Target.Name}
			}
		}
		/varset DoingARez FALSE
	}
/return

Sub CheckOnline
	| callback for Cast
|	/if (!${QuietRez} && !${DoingARez}) /call Interrupt
	/doevents NotOnline
/return

Sub Event_NotOnline(Line, Name)
	| sent a tell on rez but got an offline message - interrupt cast
	/if (!${QuietRez} && ${DoingARez} && ${Target.CleanName.Arg[1,'].Equal[${Name}]}) /call Interrupt
/return

Sub Event_CannotRez
	/if (${Target.ID} && (${DoingAERez} || ${AlwaysRezOnCon})) {
		/echo Could not rez ${Target.CleanName}, adding to ignore list.
		/call AddIgnoreList ${Target.ID}
	}
/return

|----------------------------------------------------------------------------------------------------| 

Sub AddIgnoreList(int NewID)
	/if (!${Defined[RezIgnoreCount]}) /declare RezIgnoreCount int outer 0
	| ignore list is empty - create it with new id
	/if (!${RezIgnoreCount}) {
		/varset RezIgnoreCount 1
		/if (${Defined[RezIgnoreList]}) /deletevar RezIgnoreList
		/declare RezIgnoreList[1] int outer
		/varset RezIgnoreList[1] ${NewID}
		/return
	}
	| copy existing list temp list, one entry larger
	/declare Temp[${Math.Calc[${RezIgnoreCount}+1]}] int local
	/declare x int local
	/for x 1 to ${RezIgnoreCount}
		| return if new idis already in the ignore list
		/if (${NewID} == ${RezIgnoreList[${x}]}) /return
		/varset Temp[${x}] ${RezIgnoreList[${x}]}
	/next x
	/varset RezIgnoreCount ${Math.Calc[${RezIgnoreCount}+1]}
	/varset Temp[${RezIgnoreCount}] ${NewID}
	| delete old ignore list and copy temp list to new ignore list
	/deletevar RezIgnoreList
	/declare RezIgnoreList[${RezIgnoreCount}] int outer
	/for x 1 to ${RezIgnoreCount}
		/varset RezIgnoreList[${x}] ${Temp[${x}]}
	/next x
/return

Sub CleanIgnoreList
	/if (!${RezIgnoreCount}) /return
	| remove ids from ignore list that are no longer in zone
	/declare x int local
	/declare Found int local 0
	/declare Temp[${RezIgnoreCount}] int local
	/for x 1 to ${RezIgnoreCount}
		| skip corpse id if it's not in the zone spawn list any more
		/if (!${Spawn[id ${RezIgnoreList[${x}]}].ID}) /next x
		/varset Found ${Math.Calc[${Found}+1]}
		/varset Temp[${Found}] ${RezIgnoreList[${x}]}
	/next x
	
	| shrink ignore list
	/if (${Found} != ${RezIgnoreCount}) {
		/deletevar RezIgnoreList
		/varset RezIgnoreCount ${Found}
		/declare RezIgnoreList[${RezIgnoreCount}] int outer
		/for x 1 to ${RezIgnoreCount}
			/varset RezIgnoreList[${x}] ${Temp[${x}]}
		/next x
	}
/return

|----------------------------------------------------------------------------------------------------|
