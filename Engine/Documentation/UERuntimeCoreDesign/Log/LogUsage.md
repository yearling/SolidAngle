##Overview##

''Original Author:'' [[User:Rama|Rama]] ([[User talk:Rama|talk]])

Dear Community,

Logs are essential for giving yourself feedback as to whether

*Your new functions are even being called
*What data your algorithm is using during runtime
*Reporting errors to yourself and the end user / debugging team
*Imposing a fatal error to stop runtime execution in special circumstances

This page describes how to use the '''Unreal output log'''. 

Other options are also discussed at the bottom of the page.

##Accessing Logs##

###In-Game###

To see logs you must run your game with -Log (you must create a shortcut to the Editor executable and add -Log to the end).

or use console command "showlog" in your game.

###Within Editor (Play-In-Editor)###

Log messages are sent to the 'Output' log which is accessible via Window -> Developer Tools -> Output Log.

If you are using the Editor and PIE, logging should be enabled by default due to the presence of "GameCommandLine#-log" in your Engine INI file. If no logging is visible, add the "-Log" command line option as per the instructions for In-Game logging above.

##Quick Usage##
## 可用值 
Fatal、Error、Warning、Display、Log、Verbose 或 VeryVerbose。
<syntaxhighlight lang#"cpp">
UE_LOG(LogTemp, Warning, TEXT("Your message"));
</syntaxhighlight>
This way you can log without the need of creating a custom category. Doing so will keep everything clean and sorted though.

##Setting Up Your Own Log Category##

These macros go in YourGame.h and YourGame.cpp

###YourGame.H###
You can have different log categories for different aspects of your game!

This gives you additional info, because UE_LOG prints out which log category is displaying a message.

See below for why this would be useful.
<syntaxhighlight lang#"cpp">
//General Log
DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(YourInit, Log, All);

//Logging for your AI system
DECLARE_LOG_CATEGORY_EXTERN(YourAI, Log, All);

//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(YourCriticalErrors, Log, All);
</syntaxhighlight>

###YourGame.CPP###
<syntaxhighlight lang#"cpp">
//General Log
DEFINE_LOG_CATEGORY(YourLog);

//Logging during game startup
DEFINE_LOG_CATEGORY(YourInit);

//Logging for your AI system
DEFINE_LOG_CATEGORY(YourAI);

//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(YourCriticalErrors);
</syntaxhighlight>

##Log formatting##
###Log Message###
<syntaxhighlight lang#"cpp">
//"This is a message to yourself during runtime!"
UE_LOG(YourLog,Warning,TEXT("This is a message to yourself during runtime!"));
</syntaxhighlight>

###Log an FString###

  '''%s strings are wanted as TCHAR* by Log, so use *FString()'''

<syntaxhighlight lang#"cpp">
//"MyCharacter's Name is %s"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's Name is %s"), *MyCharacter->GetName() );
</syntaxhighlight>

###Log an Bool###
<syntaxhighlight lang#"cpp">
//"MyCharacter's Bool is %s"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's Bool is %s"), (MyCharacter->MyBool ? TEXT("True") : TEXT("False")));
</syntaxhighlight>

###Log an Int###
<syntaxhighlight lang#"cpp">
//"MyCharacter's Health is %d"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's Health is %d"), MyCharacter->Health );
</syntaxhighlight>

###Log a Float###
<syntaxhighlight lang#"cpp">
//"MyCharacter's Health is %f"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's Health is %f"), MyCharacter->Health );
</syntaxhighlight>

###Log an FVector###
<syntaxhighlight lang#"cpp">
//"MyCharacter's Location is %s"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's Location is %s"), 
    *MyCharacter->GetActorLocation().ToString());
</syntaxhighlight>

###Log an FName###
<syntaxhighlight lang#"cpp">
//"MyCharacter's FName is %s"
UE_LOG(YourLog,Warning,TEXT("MyCharacter's FName is %s"), 
    *MyCharacter->GetFName().ToString());
</syntaxhighlight>

###Log an FString,Int,Float###
<syntaxhighlight lang#"cpp">
//"%s has health %d, which is %f percent of total health"
UE_LOG(YourLog,Warning,TEXT("%s has health %d, which is %f percent of total health"),
    *MyCharacter->GetName(), MyCharacter->Health, MyCharacter->HealthPercent);
</syntaxhighlight>

##Log Coloring##
###Log: Grey###
<syntaxhighlight lang#"cpp">
//"this is Grey Text"
UE_LOG(YourLog,Log,TEXT("This is grey text!"));
</syntaxhighlight>

###Warning: Yellow###
<syntaxhighlight lang#"cpp">
//"this is Yellow Text"
UE_LOG(YourLog,Warning,TEXT("This is yellow text!"));
</syntaxhighlight>

###Error: Red###
<syntaxhighlight lang#"cpp">
//"This is Red Text"
UE_LOG(YourLog,Error,TEXT("This is red text!"));
</syntaxhighlight>

###Fatal: Crash for Advanced Runtime Protection###

You can throw a fatal error yourself if you want to make sure that certain code never runs.

I have used this myself to help protect against algorithm cases that I wanted to make sure never occurred again.

It's actually really useful!

But it does look like a crash, and so if you use this, dont be worried, just look at the crash call stack :)

  Again this is an advanced case that crashes the program, use only for extremely important circumstances.
  
<syntaxhighlight lang#"cpp">
//some complicated algorithm
if(some fringe case that you want to tell yourself if the runtime execution ever reaches this point)
{
	//"This fringe case was reached! Debug this!"
	UE_LOG(YourLog,Fatal,TEXT("This fringe case was reached! Debug this!"));
}
</syntaxhighlight>

##Related Tutorial##
[[Log_Macro_with_Netmode_and_Colour|Custom Log Coloring & NetMode]]


##Quick tip print##
This a trick for easy print debug, you can use this MACRO at the begin of your cpp
<syntaxhighlight lang#"cpp">
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)
</syntaxhighlight>

then you can use a regular lovely print(); inside to all.

To prevent your screen from being flooded, you can change the first parameter, key, to a positive number.  Any message printed with that key will remove any other messages on screen with the same key.  This is great for things you want to log frequently.

##Other Options for Debugging##

###Logging message to the screen###

For the times when you want to just display the message on the screen, you can also do:

<syntaxhighlight lang#"cpp">
 #include <EngineGlobals.h>
 #include <Runtime/Engine/Classes/Engine/Engine.h>
 // ...
 GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
 GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %f, y: %f"), x, y));
</syntaxhighlight>

To prevent your screen from being flooded, you can change the first parameter, key, to a positive number.  Any message printed with that key will remove any other messages on screen with the same key.  This is great for things you want to log frequently.

###Logging message to the ~ Client Console ###

Pressing the ~ key in Unreal brings up the client console. 

If you use the PlayerController class you can print a message to this console, which has the advantage of being a completely different logging space which does not require tabbing out of the game to view easily

  PC->ClientMessage("Your Message");

Answerhub post on using ClientMessage:

https://answers.unrealengine.com/questions/81662/vshow-function.html

Forum Post

It is also possible to [[https://forums.unrealengine.com/showthread.php?33367-Log-to-Console|send messages to the client console]].

## Log conventions (in the console, ini files, or environment variables) ##
* [cat]   # a category for the command to operate on, or 'global' for all categories.
* [level] # verbosity level, one of: none, error, warning, display, log, verbose, all, default
At boot time, compiled in default is overridden by ini files setting, which is overridden by command line

## Log console command usage ##
* Log list            - list all log categories
* Log list [string]   - list all log categories containing a substring
* Log reset           - reset all log categories to their boot-time default
* Log [cat]           - toggle the display of the category [cat]
* Log [cat] off       - disable display of the category [cat]
* Log [cat] on        - resume display of the category [cat]
* Log [cat] [level]   - set the verbosity level of the category [cat]
* Log [cat] break     - toggle the debug break on display of the category [cat]

## Log command line ##

 <nowiki>-LogCmds#\"[arguments],[arguments]...\"           - applies a list of console commands at boot time
-LogCmds#\"foo verbose, bar off\"         - turns on the foo category and turns off the bar category</nowiki>

## Environment variables ##
Any command line option can be set via the environment variable '''UE-CmdLineArgs'''

 <nowiki>set UE-CmdLineArgs#\"-LogCmds#foo verbose breakon, bar off\"</nowiki>

## Config file ##
In DefaultEngine.ini or Engine.ini:
 <nowiki>
[Core.Log]
global#[default verbosity for things not listed later]
[cat]#[level]
foo#verbose break</nowiki>

##Printing the Class Name, Line Number, and Function Name Automatically##

To automatically print the Class Name, Function Name, and Line number, see my other wiki! 

'''[[Logs,_Printing_the_Class_Name,_Function_Name,_Line_Number_of_your_Calling_Code!]]'''
[[Category:Code]]

This lets you easily debug code during runtime because you are told what class and what line number is associated with the message you are printing to yourself!
