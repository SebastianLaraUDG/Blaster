# MultiplayerSessions
Multiplayer sessions plugin from Unreal Multiplayer shooter course


In order to use this multiplayer plugin you need to:
 * Enable the OnlineSubsystem Steam plugin in the editor.
 * Enable the Steam sockets plugin in the editor.
 * Run Steam (it's ok if you're running it in the background).
 *
 * Please make sure you set up correctly the DefaultEngine.ini file (devID, etc.).
 * https://dev.epicgames.com/documentation/en-us/unreal-engine/online-subsystem-steam-interface-in-unreal-engine
 *
 *
 * In version 5.6 in the DefaultEngine.ini replace this line:
 * +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")
 * with the following:
 * !NetDriverDefinitions=ClearArray
 * +NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Scripts/SteamSockets.SteamSocketsNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")
