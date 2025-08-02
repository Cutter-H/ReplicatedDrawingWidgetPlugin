Widget supports being on multiple screens as well across multiple players. Usage is simple Drag and Drop into the widget blueprint.

For documentation please see the [Wiki](https://github.com/Cutter-H/ReplicatedDrawingWidgetPlugin/wiki)

If you wish for further information on the plugin's C++ related content, please see the header files, as they are thoroughly commented.

Release for 5.5: [Here](https://github.com/Cutter-H/ReplicatedDrawingWidgetPlugin/releases/tag/5.5)

If you want support for other versions let me know.

You can find a settings data asset in the plugin's content folder. This has 2 separate variables you can use for optimization purposes. Moving or renaming the DataAsset will cause issues and it will no longer work.

Any changes to pen data will immediately affect new lines. There are overridable functions in the widget that allow for conditional line draws (such as if you wish to disable a player or players) and an override to get the drawer/player's name (the default is a GetPlayerName on the player state). 


Please see the video below for an example of the usage. 


https://github.com/user-attachments/assets/4534f22f-e2bb-452d-80e5-e7576ed51dd2


## How does it replicate?

Since widgets do not replicate, the plugin comes with a subsystem that spawns a replicated manager actor. This is so no additional steps are required on your part, making the wodget drag and drop. It also listens for actor spawns and adds a lightweight component to player states when they are spawned. The player state component allows the widget to send information to the server. The manager actor holds all data and only 1 is created. Widgets are bound to a delegate on the replicated manager that sends out broadcasts on lines added. The widgets can also erase the data as well as refresh to match the manager to ensure no errors are present. The widgets are identified by the BoardName in the widget as well as the player drawing. 
