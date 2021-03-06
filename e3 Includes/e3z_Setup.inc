|--------------------------------------------------------------------|
|- e3_Macro_Setup.inc ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains setup functions for e3.mac								-|
|--------------------------------------------------------------------|






#include e3 Includes\e3_Alerts.inc
#include e3 Includes\e3_Assists.inc
|- #include e3 Includes\e3_Background.inc
|- #include e3 Includes\e3_Basics.inc
#include e3 Includes\e3_BegForBuffs.inc
#include e3 Includes\e3_BuffCheck.inc
#include e3 Includes\e3_Buy.inc
|- #include e3 Includes\e3_Casting.inc
#include e3 Includes\e3_Combine.inc
|- #include e3 Includes\e3_CorpseSummoning.inc
#include e3 Includes\e3_HealChain.inc
#include e3 Includes\e3_Heals.inc
|- #include e3 Includes\e3_Loot.inc
#include e3 Includes\e3_Pets.inc
#include e3 Includes\e3_Sell.inc
#include e3 Includes\e3_SmoothMoves.inc
|- #include e3 Includes\e3_Utilities.inc
#include e3 Includes\e3_Wait4Rez.inc

#include e3 Includes\e3_Classes_Bard.inc
#include e3 Includes\e3_Classes_Beastlord.inc
#include e3 Includes\e3_Classes_Berserker.inc
#include e3 Includes\e3_Classes_Cleric.inc
#include e3 Includes\e3_Classes_Druid.inc
#include e3 Includes\e3_Classes_Enchanter.inc
#include e3 Includes\e3_Classes_Magician.inc
#include e3 Includes\e3_Classes_Monk.inc
#include e3 Includes\e3_Classes_Necromancer.inc
#include e3 Includes\e3_Classes_Paladin.inc
#include e3 Includes\e3_Classes_Ranger.inc
#include e3 Includes\e3_Classes_Rogue.inc
#include e3 Includes\e3_Classes_ShadowKnight.inc
#include e3 Includes\e3_Classes_Shaman.inc
#include e3 Includes\e3_Classes_Warrior.inc
#include e3 Includes\e3_Classes_Wizard.inc

|- #include e3 Includes\VetAAs.inc


|-------------------------------- Septaron .inc filename changes ------------------------------------------
#include e3 Includes\e3z_Background.inc
#include e3 Includes\e3z_Basics.inc
#include e3 Includes\e3z_CorpseSummoning.inc
#include e3 Includes\e3z_Loot.inc
#include e3 Includes\e3z_Utilities.inc
#include e3 Includes\e3z_Casting.inc
#include e3 Includes\e3z_VetAAs.inc

|-------------------------------- End Septaron Mods ------------------------------------------

SUB e3_Setup(modeSelect)

/if (${Debug}) /echo |- e3_Setup ==>

	/declare Debug bool outer FALSE
	/if (${modeSelect.Equal[Debug]}) /varset Debug TRUE

	| The file path for e3 Data.ini will still need to be updated in corresponding includes because you must use /noparse to write variables to inis.
	/declare MacroData_Ini string outer e3 Macro Inis\e3 Data.ini
	/declare i int local

	/call check_Plugins
	
	/echo Loading e3 v${macroVersion}...

	| create a macro data
	/if (!${Ini[${MacroData_Ini}].Length}) {
		/echo Welcome to e3! preforming first time setup...
		/call make_macroDataIni
	}

| Import file paths ======================================================================================================= 

	| General Settings
	/if (!${Ini[${MacroData_Ini},File Paths,General Settings].Length}) {
		/echo ERROR: Could not find designated file path for [General Settings].  Please review review settings in [${MacroData_Ini} > File Paths].
		/endmacro
	} else {
		/declare genSettings_Ini string outer ${Ini[${MacroData_Ini},File Paths,General Settings]}
	}

	| Advanced Settings
	/if (!${Ini[${MacroData_Ini},File Paths,Advanced Settings].Length}) {
		/echo ERROR: Could not find designated file path for [Advanced Settings].  Please review review settings in [${MacroData_Ini} > File Paths].
		/endmacro
	} else {
		/declare advSettings_Ini string outer ${Ini[${MacroData_Ini},File Paths,Advanced Settings]}
	}
	
	| Bot Settings
	/if (!${Ini[${MacroData_Ini},File Paths,Bot Settings].Length}) {
		/echo ERROR: Could not find designated file path for [Bot Settings].  Please review review settings in [${MacroData_Ini} > File Paths].
		/endmacro
	} else {
		/declare Character_Ini string outer ${Ini[${MacroData_Ini},File Paths,Bot Settings]}
	}
	
| Create Ini files ======================================================================================================== 
	/if (${Bool[${Ini[${genSettings_Ini}]}]} && !${Ini[${MacroData_Ini},e3 Build,Version].Equal[${macroVersion}]}) /echo New version of e3 detected. Hold on to your nuggets!
	
	| Macro Setting Inis
	/if (!${Ini[${MacroData_Ini},e3 Build,Version].Equal[${macroVersion}]} || !${Bool[${Ini[${genSettings_Ini}]}]}) /call EVENT_make_MacroSettingsIni
	
	| Generate Character_Ini.
	/if (!${Ini[${MacroData_Ini},${MacroQuest.Server}-${Me.CleanName},Bot_Ini version].Equal[${macroVersion}]} || !${Bool[${Ini[${Character_Ini}]}]}) /call EVENT_make_CharacterSettingsIni
	
	| Macro Aliases
	/if (!${Ini[${MacroData_Ini},e3 Build,Version].Equal[${macroVersion}]}) /call EVENT_updateAliases

	| Update macro version in e3 Data.ini
	/if (!${Ini[${MacroData_Ini},e3 Build,Version].Equal[${macroVersion}]}) /call WriteToIni "${MacroData_Ini},e3 Build,Version" "${macroVersion}" 1
	
| Import ini settings ===================================================================================================== 
	
	/echo Importing user settings...
	
	| Debug
	/if (!${modeSelect.Equal[Debug]}) /call IniToVar "${advSettings_Ini},Debug,Debug Mode (On/Off)" Debug bool outer
	
	| Chat Channel
	/declare ChatToggle string outer /bc
	/if (${Ini[${Macro_Ini},General,Macro Output Channel].Length}) /call IniToVar "${Macro_Ini},General,Macro Output Channel" ChatToggle string outer

	/call IniToArray "${advSettings_Ini},Included Setups,Include#" macroSetups
	/call IniToArray "${advSettings_Ini},${Me.Class.ShortName} Functions,${Me.Class.ShortName} Function#" mainLoop_Array
	
| Setup Includes ========================================================================================================== 

	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_Setup
	/next i

| Complete a pending exchange ============================================================================================= 
	/if (${Ini[${MacroData_Ini},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].Length} && ${Ini[${MacroData_Ini},${MacroQuest.Server}-${Me.CleanName},Pending Exchange].NotEqual[NONE]} && !${itemExchange_Timer}) /call completePendingExchange
	
| ========================================================================================================================= 
	
	/echo e3 loaded.
	/echo
	
	/echo ** For updates, issues, suggestions, or donations, visit 'http://code.google.com/p/e3-macro-builds/'.
	/echo ** For help using e3, use /e3help to learn about different basic functions and /comannds.
	/if (${NetBots.Client.Find[Tackleberry]}) /echo Though you may find this message a bit 'extreme' so is bitching about a few lines of text which can easily be ignored.
	
	/echo
	
	/if (${Debug}) {
		/echo
		/echo
		/echo *** Debug_Mode ON ***
		/echo The macro has paused so you can review setup information, type /mqpause to continue.
		/echo
		/echo
		/mqpause
	}

/if (${Debug}) /echo <== e3_Setup -|	
/RETURN



|-------------------------
|- Creates a file settings file to change file paths, and store various pieces of data
|-------------------------
SUB make_macroDataIni
/if (${Debug}) /echo |- make_macroDataIni ==>

	/noparse /ini "e3 Macro Inis\e3 Data.ini" "e3 Build" "Version"
	/noparse /ini "e3 Macro Inis\e3 Data.ini" "File Paths" "Bot Settings" "e3 Bot Inis\${MacroQuest.Server}-${Me.CleanName}.ini"
	/noparse /ini "e3 Macro Inis\e3 Data.ini" "File Paths" "General Settings" "e3 Macro Inis\General Settings.ini"
	/noparse /ini "e3 Macro Inis\e3 Data.ini" "File Paths" "Advanced Settings" "e3 Macro Inis\Advanced Settings.ini"

/if (${Debug}) /echo <== make_macroDataIni -|
/RETURN



|--------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.	-|
|--------------------------------------------------------------------------------|
#EVENT make_MacroSettingsIni "[MQ2] Make Macro ini"
SUB EVENT_make_MacroSettingsIni
/if (${Debug}) /echo |- make_MacroSettingsIni ==>

	/echo Creating macro settings files...

	/call WriteToIni "${genSettings_Ini},General,Macro Output Channel" /bc
	
	/call WriteToIni "${advSettings_Ini},Debug,Debug Mode (On/Off)" Off
	
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#1" background 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#2" basics 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#3" casting 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#4" begForBuffs 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#5" buffCheck 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#6" buy 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#7" pet 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#8" heal 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#9" healChain 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#10" assist 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#11" class 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#12" corpseSummon 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#13" loot 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#14" sell 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#15" smoothMoves 1
	/call WriteToIni "${advSettings_Ini},Included Setups,Include#16" Wait4Rez 1
	
	/delay 15
	
	| Import the array of Included Setups
	/call IniToArray "${advSettings_Ini},Included Setups,Include#" macroSetups
	
	| Call each setup
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_MacroSettings
	/next i
	
	| Call class specific setups
	/call BRD_MacroSettings
	/call BST_MacroSettings
	/call BER_MacroSettings
	/call CLR_MacroSettings
	/call DRU_MacroSettings
	/call ENC_MacroSettings
	/call MAG_MacroSettings
	/call MNK_MacroSettings
	/call NEC_MacroSettings
	/call PAL_MacroSettings
	/call RNG_MacroSettings
	/call ROG_MacroSettings
	/call SHD_MacroSettings
	/call SHM_MacroSettings
	/call WAR_MacroSettings
	/call WIZ_MacroSettings

/if (${Debug}) /echo <== make_MacroSettingsIni -|
/RETURN

	

|--------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.	-|
|--------------------------------------------------------------------------------|
#EVENT make_CharacterSettingsIni "[MQ2] Make bot ini"
SUB EVENT_make_CharacterSettingsIni
/if (${Debug}) /echo |- make_CharacterSettingsIni ==>

	/echo Creating ${Me.CleanName}'${If[${Me.CleanName.Right[1].Equal[s]},,s]} settings file...
	
	/call IniToArray "${advSettings_Ini},Included Setups,Include#" macroSetups
	
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_CharacterSettings
	/next i
	
	/call WriteToIni "${MacroData_Ini},${MacroQuest.Server}-${Me.CleanName},Bot_Ini version" "${macroVersion}" 1

/if (${Debug}) /echo <== make_CharacterSettingsIni -|
/RETURN	



|------------------------------------------------------------|
|- Deletes and Recreates aliases associated with e3.mac.	-|
|------------------------------------------------------------|
#EVENT updateAliases "[MQ2] Update macro aliases"
SUB EVENT_updateAliases
/if (${Debug}) /echo |- EVENT_updateAliases ==>

	/echo Updating macro aliases....
	/echo (WARNING: Your clients will lock up for a few moments.  Stand fast, friend...)
	/echo Aliases may not work properly until you restart MQ2 on ALL clients.
	
	| Core aliases
	/squelch /alias /updateAliases /echo Update macro aliases
	/squelch /alias /makeCharIni /echo Make Character Ini
	/squelch /alias /makeSettingsIni /echo Make General Settings Ini
	/squelch /alias /makeAdvSettingsIni /echo Make Advanced Settings Ini
	/squelch /alias /e3help /echo Help
	/squelch /alias /debugon /varset Debug 1
	/squelch /alias /debugoff /varset Debug 0

	/call IniToArray "${advSettings_Ini},Included Setups,Include#" macroSetups
	
	/declare i int local
	
	/for i 1 to ${macroSetups.Size}
		/if (${macroSetups[${i}].Equal[class]}) /varset macroSetups[${i}] ${Me.Class.ShortName}
		/if (${Bool[${macroSetups[${i}]}]}) /call ${macroSetups[${i}]}_Aliases
	/next i
	
	/call BRD_Aliases
	/call BST_Aliases
	/call BER_Aliases
	/call CLR_Aliases
	/call DRU_Aliases
	/call ENC_Aliases
	/call MAG_Aliases
	/call MNK_Aliases
	/call NEC_Aliases
	/call PAL_Aliases
	/call RNG_Aliases
	/call ROG_Aliases
	/call SHD_Aliases
	/call SHM_Aliases
	/call WAR_Aliases
	/call WIZ_Aliases

	/call VetAAs_Aliases

/if (${Debug}) /echo <== EVENT_updateAliases -|
/RETURN



|----------------------------------------------------|
|- Checks for and enables required macro plugins.	-|
|----------------------------------------------------|
SUB check_Plugins
/echo Checking plugins...

| REQUIRED PLUGINS ==>


	| REQUIRED PLUGINS ==>
	
| -----------------------------------------------------------------------------------------------------------------------------------Check MQ2EQBC.
	/if (!${Plugin[MQ2EQBC].Name.Length}) {
		/echo Plugin MQ2EQBC is not loaded, attempting to resolve...
		/plugin MQ2EQBC
		/delay 10s !${Select[${EQBC},NULL]}
		/if (${Select[${EQBC},NULL]}) {
			/echo ***WARNING*** Could not load MQ2EQBC, macro functionality may be limited.
			/echo Starting in Solo Mode...
		}
	}

| -------------------------------------------------------------------------------------------Auto-Connect to EQBCS.
	/if (${Bool[${EQBC}]}) {
		/if (!${EQBC.Connected}) {
			/bccmd connect
			/delay 100 ${EQBC.Connected}
			/delay 5
			/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS! Please open EQBCS and try again.  Macro functionality may be limited...
		}


	| --------------------------------------------------------------------------------------------------------------------------------Check MQ2NetBots.
		/if (!${Plugin[MQ2NetBots].Name.Length}) {
			/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
			/plugin MQ2NetBots
			/delay 3s ${NetBots}
			/if (!${NetBots}) {
				/echo ***WARNING*** Could not load MQ2NetBots! Macro functionality may be limited.
			}
		}
		
	| -------------------------------------------------------------------------------------------Auto-Connect to NetBots.
		/if (${NetBots}) {
			/squelch /netbots on grab=on send=on
		}
		
	| -----------------------------------------------------------------------------------------------------------------------------Check MQ2EmuNetAdvPath.
		/if (!${Plugin[MQ2EmuNetAdvPath].Name.Length}) {
			/echo Plugin MQ2EmuNetAdvPath is not loaded, attempting to resolve...
			/plugin MQ2EmuNetAdvPath
			/delay 3s ${NetAdvPath}
			/if (!${NetAdvPath}) {
				/echo ***WARNING*** Could not load MQ2EmuNetAdvPath. Please ensure you're using a copy of MQ2 which was compiled with the MQ2EmuNetAdvPath plugin and try again.
			}
		}
	}
	
| ------------------------------------------------------------------------------------------------------------------------------Check MQ2MoveUtils.
	/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
		/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
		/plugin MQ2MoveUtils
		/delay 3s !${Select[${Stick.Status},NULL]}
		/if (${Select[${Stick.Status},NULL]}) {
			/echo ***WARNING*** Could not load MQ2MoveUtils! Macro functionality may be limited.
			/if (!${NetAdvPath}) {
				/echo Follow and Assist stick DISABLED.
			} else {
				/echo Assist and NPC stick DISABLED.
				/echo Follow restricted to NetBots.
			}
		}
	}	

| -------------------------------------------------------------------------------------------------------------------------------Check MQ2Exchange.
	/if (!${Plugin[MQ2Exchange].Name.Length}) {
		/echo Plugin MQ2Exchange is not loaded, attempting to resolve...
		/plugin MQ2Exchange
		/delay 3s ${Plugin[MQ2Exchange].Name.Length}
		/if (!${Plugin[MQ2Exchange].Name.Length}) {
			/echo ***WARNING*** Could not load MQ2Exchange! Macro functionality may be limited.
			/echo Item swapping is DISABLED.
		}
	}

| ----------------------------------------------------------------------------------------------------------------------------------Check MQ2Twist.
	/if (${Select[${Me.Class.ShortName},BRD]}) {
		/if (!${Plugin[MQ2Twist].Name.Length}) {
			/echo Plugin Unable to detect MQ2Twist, attempting to resolve...
			/plugin MQ2Twist
			/delay 3s !${Select[${Twist},NULL]}
			/if (${Select[${Twist},NULL]}) {
				/echo ***WARNING*** Could not load MQ2Twist! Macro functionality may be limited.
				/echo Bard melodies DISABLED.
			}
		}	
	}

| <== REQUIRED PLUGINS

| ----------------------------------------------------------------------------------------------------------------------------Check MQ2ItemDisplay.
	/if (!${Plugin[mq2itemdisplay].Name.Length}) /plugin MQ2ItemDisplay

| ------------------------------------------------------------------------------------------------------------------------------------Check MQ2HUD.
	/if (!${Plugin[mq2hud].Name.Length}) /plugin MQ2HUD	
	
| <== REQUIRED PLUGINS
/RETURN
