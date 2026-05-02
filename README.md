This project shows understanding of:

pointers and structs
dynamic memory allocation
file I/O (fread, fwrite)
serialization/deserialization
CLI argument parsing
basic data management patterns




This program is a file-backed database system written in C, using:

in-memory data structures (for fast access)
binary file persistence (to save/load data across executions)
a command-line interface (CLI) to interact with the database

It supports basic database operations like storing, retrieving, deleting, listing, and searching records.



Core Data Structures:

struct Address:

Represents a single record:

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

id → unique identifier (index)
set → whether the record is active (1 = used, 0 = empty)
name, email → dynamically allocated strings





struct Database:


struct Database {
    int max_data;
    int max_rows;
    struct Address *rows;
};
Stores metadata (max_rows, max_data)
Holds a dynamic array of records



struct Connection:

struct Connection {
    FILE *file;
    struct Database *db;
};
Wraps:
file pointer (persistent storage)
in-memory database


File Persistence (Key Feature)
 Concept

The program implements manual serialization:

- Converts in-memory data → binary format → writes to file
- Reads binary file → reconstructs data in memory

 File Layout:

[max_rows][max_data]
For each row:
    [id][set]
    if set:
        [name_len][name_bytes]
        [email_len][email_bytes]

The program uses manual dynamic memory management:

Allocation:
-malloc, calloc for:
-database
-rows array
-strings

Deallocation:
-free in:
-Database_delete
-Database_close

Safety practices:
-NULL checks
-freeing before overwriting
-avoiding leaks and double frees



CLI Interface

The program is controlled via command-line arguments:

./db file.dat <action> [arguments...]




Supported Commands
| Create database|

Commmand:    ./db file.dat c 100 256
-initializes database
- sets max rows and max string size



Set (insert/update)
Commmand:   ./db file.dat s 1 "John" "john@mail.com"

-stores data at given ID
-allocates memory dynamically

  


Get (retrieve)
Command:  ./db file.dat g 1
-prints a single record



Delete
Command: ./db file.dat d 1
-frees memory
-marks record as unused


List
Command: ./db file.dat l
-prints all active records



Find (search)
Command:  ./db file.dat f "john"
-linear search using strstr
-matches substring in name or email
