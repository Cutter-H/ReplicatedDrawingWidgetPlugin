Widget supports being on multiple screens as well across multiple players. Usage is simple Drag and Drop into the widget blueprint.

DOES NOT CURRENTLY SUPPORT BRUSHES. The Brush in the pen settings is for future use. At the moment it is only lines of different colors.


How does it replicate?
The plugin comes with a subsystem that spawns a replicated manager actor. It also listense for actor spawns and adds a lightweight component to player states when they are spawned. The player state component allows the widget to send information to the server. The manager actor holds all data and only 1 is created. Widgets are bound to a delegate on the replicated manager that sends out broadcasts on lines added. The widgets can also erase the data as well as refresh to match the manager to ensure no errors are present. The widgets are identified by the BoardName in the widget as well as the player drawing. There are overridable functions in the widget that allow for conditional line draws (such as if you wish to disable a player or players) and an override to get the drawer/player's name (the default is a GetPlayerName on the player state). 

Please see the video below for an example of the usage. 

This is a plugin that was done in an afternoon and is not finished, but it is in a usable state.
Documentation and fixes/polish is planned for tomorrow.



https://github.com/user-attachments/assets/5021e16a-fe09-44fd-8017-67a086c04e54

