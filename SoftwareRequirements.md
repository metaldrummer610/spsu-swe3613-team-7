# Libraries Required #
To build the project, the following applications are required:
  * g++
  * make
  * subversion (svn)
  * postgresql
  * libpqxx
  * boost (in libs folder)
  * enet (in libs folder)
  * fltk (in libs folder)
  * apache log4cxx (in libs folder)
  * libx11-dev
  * libapr1-dev
  * libaprutil1-dev

For those that are lazy :P
```
sudo apt-get install g++ make subversion postgresql libpqxx-dev libx11-dev libapr1-dev libaprutil1-dev
```

# Build Instructions #
Steps to build:
  1. Build the Boost libraries
    1. Run tar -xvf on boost\_1\_47\_0.tar.bz2
    1. Run a ./bootstrap.sh
    1. Run a sudo ./b2 install
  1. Build the apache log4cxx library
    1. Run tar -xvf on apache-log4cxx-0.10.0.tar.gz
    1. Run ./configure
    1. Run a make
    1. Run a sudo make install
  1. Build the fltk libraries
    1. Run a tar -xvf on fltk-1.3.0-source.tar.gz
    1. Run a ./configure
    1. Run a make
    1. Run a sudo make install
  1. Build the enet library
    1. Run tar -xvf on enet-1.3.3.tar.gz
    1. Run a ./configure
    1. Run a make
    1. Run a sudo make install
  1. Build the common library
    1. Move into the common directory
    1. Run make
  1. Build the server
    1. Move into the server directory
    1. Run make
  1. Build the client
    1. Move into the client/fltk directory
    1. Run make

To run the server, run a "make run" from the server directory, or execute the "server" binary in the server/bin/ folder.

To run the client, run a "make run" from the client/fltk directory, or execute the "client" binary in the client/fltk/bin directory.

Running "make clean" in either the common, server, or client/fltk directories will clean up the binaries from the respective folder.

# Building Apache log4cxx #
The logging library may require some TLC to get it to build... While building the code, I had to modify a few files and add references to "string.h" and "stdlib.h". I'm not sure why they would release their code with those blatant errors, but whatever...

# Adding a user for database access #
Reference the following link: [Adding A User To PostgreSQL](http://www.cyberciti.biz/faq/howto-add-postgresql-user-account/)

Run the following commands to set up the user for the database:
```
sudo adduser swe3613
```
The password for the user is "swe3613". Hit enter for all the information fields, we don't really care. Must be done as root.

```
sudo su - postgres
```
Log into the postgres user account

```
psql template1
```
Access the template1 database.

```
CREATE USER swe3613 WITH PASSWORD 'swe3613';
```
This creates the user in the database.

```
CREATE DATABASE swe3613;
```
This creates the database for the application.

```
GRANT ALL PRIVILEGES ON DATABASE swe3613 to swe3613;
```
Gives the user access to the database.

Now, before you can log in easily, there is a config setting you have to change. If you open /etc/postgresql/9.1/main/pg\_hba.conf in a root text editor and navigate down to the line that says:
```
# Database administrative login by Unix domain socket
```

The line below that should say something to the effect of:
```
local   all    postgres   peer
```

That line should be changed to say:
```
local   all    all  trust
```

Once you have edited the file, you have to restart the service:
```
sudo service postgresql restart
```

This will allow you to easily log in without complaining about "Peer authentication" errors.