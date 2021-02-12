How to startup server.
1. Go to resource/cfg-remote.txt
2. Enter ip address that matches the address of the remote debugger (this also includes the port of the remite debugger)
3. Go to GPRO-NET-VSLAUNCH-REMOTE
4. Make sure remote debugger is on with the virtual machine
5. Hit remote windows debug

Tips with the server
- Whenever done with server, Just hit escape on the console in the virtual remote debugger machine
- Output file will go to C\users\(name)\Remote\gpro-net-sdk-remote\bin\x64\v142\Debug

How to startup client
1. Go to GPRO-NET-VSLAUNCH-REMOTE
2. Make sure server is on
3. Hit local windows debugger
4. Put in IP address for the remote debugger machine, which includes port
5. Put in User name and then it should connect you in

Commands with client
- 0 = quit out of server. Press 2 to confirm change
- 1 = send message. This can be send to a specific user by typing their exact name or you can type public to broadcast it over the server
- 2 = recieves messages that have been sent to us since the last time there was a recieve message command
- 3 = print list of clients currently in server. To get it, press 2 to recieve it. 