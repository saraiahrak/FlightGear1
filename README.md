# FlightGear1

Introduction:
FlightGear is an open-source flight simulator.
Our project goal is to create a sophisticated and open flight simulator framework for use
a fun, realistic, and challenging desktop flight simulator.

Our project communicates with the FlightGear Simulator in two channels- as client (where the simulator is a server), and as a server (where the simulator is a client), allowing back and forth communication to receive information from FlightGear while controlling it with commands.

The information received from the simulator is based on the generic_small.xml file, while the commands sent to FlightGear are based on a text file containing those commands.

download the project from GitHub (https://github.com/saraiahrak/FlightGear1) and put the given 'fly.txt' in the same project directory. 
download FlightGear: ‫‪http://home.flightgear.or‬‬g


The sampled values will be define in a given file called 'generic_small.xml'.
Add this file to the location where you installed FlightGear in the /data/protocol folder.

Open FlightGear Simulator and add in setting the following:
--telnet=socket,in,10,127.0.0.1,5402,tcp=8080
--generic=socket,out,10,127.0.0.1,5400,tcp,generic_small

Run the program with the following:
1 -- g++ -std=c++14 *.cpp -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -o a.out -pthread
2 -- ./a.out fly.txt

After it's running open the flightGear and click FLY!
