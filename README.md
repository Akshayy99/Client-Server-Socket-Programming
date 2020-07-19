# Basic Socket Programming

A single client or multiple clients can connect to a server using sockets, and can request to see files available in the server directory, and download them onto their respective client directories.

## Commands implemented:

```bash
listall
```
Used by client to get the list of files available on the server.\
Server sends the list of files in it's directory to the client.

```bash
send <filename>
```
`send` command requires one argument which is name of the file to be downloaded.\
Server starts sending the file to the client in chunks.\
Error message given to the client if requested file does not exist or could not be accessed.

## Multiple clients
Multiple clients have been handled using forking. A child process is made for serving a client.
Each time a new client initiates a connection to the server, the server spawns a child process to handle it.
