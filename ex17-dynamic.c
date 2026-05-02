#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



struct Address{
int id;
int set;
char* name;
char* email;

};


struct Database {

    int max_data;
    int max_rows;
    struct Address* rows;

};

struct Connection {
    FILE * file;
    struct Database* db;

};

void die (const char* message){

if (errno){
    perror(message);
}

else {
    printf("ERROR: %s", message);
}

exit (1);
}

void Address_print (struct Address* addr){    

printf("ID: %d | Name: %s | Email: %s\n");

if ( !addr || !addr-set){
     printf("(empty record)\n");
        return;
     }

printf("%d %s %s\n",
           addr->id,
           addr->name ? addr->name : "(null)",
           addr->email ? addr->email : "(null)");

}


void Database_load(struct Connection* conn){                      


rewind(conn->file);
 
int rc= fread(&conn->db->max_rows,sizeof(struct int),1,conn->file);

if(rc!=1)
   die("could not load file");

int rc= fread(&conn->db->max_data,sizeof(struct int),1,conn->file);

if(rc!=1)
   die("could not load file");


conn->db->rows = calloc(conn->db->max_rows, sizeof( struct Address));


for(int i=0; i< conn->db->max_rows ; i++){

struct Address* addr = &conn->db->rows[i];


fread(addr->id , sizeof(int),1,conn->file);
fread(addr->set, sizeof(int),1,conn->file);

if(addr->set){

int len;

fread(&len,sizeof(int),1,conn->file);
addr->name = malloc(len+1);
fread(addr->name,sizeof(char),len,conn->file);
addr->name[len]='\0';

fread(&len,sizeof(int),1,conn->file);
addr->email = malloc(len+1);
fread(addr->email,sizeof(char),len,conn->file);
addr->email[len]='\0';


}





}



}


struct Connection* Database_open (const char* filename, char mode, int max_rows, int max_data){ //changed ok


struct Connection* conn = malloc (sizeof(struct Connection));

 if (!conn)
      die ("memory error");
    
     conn->db =malloc (sizeof(struct Database));

    if(!conn->db)
      die ("memory error");

//dynamic part

conn->db->max_rows= max_rows;
conn->db->max_data= max_data;


conn->db->rows = calloc(max_rows,sizeof(struct Address));
 if (!conn->db->rows){
       die("memory error");
     }



   if (mode == 'c')
       conn->file = fopen(filename,"w+");
   else 
      conn->file = fopen(filename,"r+");

   if (!conn->file)
        die("could not load database\n");

   if (conn->file)
      Database_load(conn);

   


return conn;
}


void Database_close(struct Connection* conn){   

if(!conn){
      return;
   }

if (conn->db){
    if(conn->db->rows){
       
        for (int i=0; i <db->max_rows ;i++){
         struct Address* addr= &conn->db->rows[i];
             
		if (addr->set){
		  free(addr->name);
		  free(addr->email);
			}
		

                  }
      free(conn->db->rows);

     }

      free(conn->db);
}
if (conn->file)
        fclose(conn->file);

    free(conn);


}

void Database_write(struct Connection* conn){

rewind(conn->file);

 fwrite(&conn->db->max_rows, sizeof(int),1,conn->file);
 fwrite(&conn->db->max_data, sizeof(int),1,conn->file);


for (int i=0; i < conn->db->max_rows ;i++){

  struct Address* addr = & conn->db->rows[i];

    fwrite(&addr->id, sizeof(int), 1, conn->file);
    fwrite(&addr->set, sizeof(int), 1, conn->file);

if(addr->set){
   int len;
   
   len =strlen(addr->name);
   fwrite(&len,sizeof(int),1,conn->file);
   fwrite(addr->name,sizeof(char),len,conn->file);


   len =strlen(addr->email);
   fwrite(&len,sizeof(int),1,conn->file);
   fwrite(addr->email,sizeof(char),len,conn->file);


     }

     }

}


void Database_create (struct Connection* conn){

int i=0;


for (i=0; i< conn->db->max_rows ;i++){

struct Address addr = { .id=i, .set=0};

conn->db->rows[i]= addr;

}
}

void Database_set (struct Connection* conn, int id, const char* name, const char* email ){


struct Database* db = conn->db;

if (id< 0 || id>= db->max_rows){
      die("Id out of bounds");
    }	

struct Address* addr = &db->rows[id];

if (addr->set){ //free former data if overwritting
     free(addr->name);
     free(addr->email);

   // die ("already set, delete it first");
}


int name_len = strlen(name);
int email_len = strlen(email);

if( name_len>= db->max_data || email_len >= db->max_data){

    die ("input too long");
    }

addr->name = malloc(name_len+1);
addr->email = malloc(email_len+1);

if ( !addr->name || ! addr->email) {

     die ("memory allocation failed");
    }



strcpy(addr->name,name);
strcpy(addr->email,email);

addr->id =id;
addr->set=1;



/*addr->id=id;
addr->set=1;
char* res = strncpy(addr->name,name, conn->db->max_data-1);
addr->name[conn->db->max_data-1]='\0';

if (!res){
    die ("error writing name");
}

res= strncpy(addr->email,email,conn->db->max_data-1);
addr->email[conn->db->max_data-1]='\0';
if (!res){
    die ("error writing email");
}  */

}


void Database_get(struct Connection* conn, int id){

 struct Database* db = conn->db;

if( id<0 || id >= db->max_rows ){
     die("id out of bounds");

}

    struct Address* addr = &db->rows[id];

    if(addr->set){
      Address_print(addr);
    }

    else {
        die("ID is not set");
    }

}





void Database_delete(struct Connection* conn, int id){

struct Database* db = conn->db;

 if (id < 0 || id >= db->max_rows)
        die("ID out of bounds");

struct Address* addr= &db->rows[id];

if (!addr->set){

    return;
  }

free(addr->name);
free(addr->email);

addr->name=NULL;
addr->email=NULL;


addr->set=0;
addr->id=id;


/*struct Address addr= { .id=id, .set=0 };

conn->db->rows[id]= addr; */

}






void Database_list(struct Connection* conn){


struct Database* db = conn->db;


for(int i=0;i< conn->db->max_rows ;i++){
   struct Address* addr = &db->rows[i];
   if(addr->set)
       Address_print(addr);

}
}





void Database_find(struct Connection* conn, const char* query){

struct Database* db = conn->db;

int found= 0;

for (int i=0; i < db->max_rows ; i++){

       struct Address* addr= &db->rows[i];

            if (!addr->set){

	          continue;
                  }

          if (( addr->name && strtstr(addr->name,query)) || (addr->email && strstr(addr->email,query))){

                   Address_print(addr);
                    found =1;
              }

   }

         if(!found){
             printf("No results found\n);
                }

}





}







int main(int argc, char* argv [] ){

if (argc>3){
    printf("Usage: ./ex17 [<bdfile> <action>  [action parameters]");
}

char* filename= argv[1];
char action = argv[2][0];


int id=0;

if (argc>3){
    id = atoi(argv[3]);
}
 if (id>=conn->db->max_rows){
     die ("there are not that many records");
 }


 switch (action){

 case 'c':   if (argc<5){
                die("USAGE: [] dbfile action max_rows max_data ");
              }
             int max_rows = atoi(argv[3]);
             int max_data = atoi(argv[4]);
             if (max_rows <=0 || max_data <=0){
                die("invalid size values");
              }
             struct Connection* conn = Database_open(filename, action,max_rows,max_data);
             Database_create(conn);
             Database_write(conn);
             break;

 case 'g':   if (argc!=4);
             die("Need an ID to get");
             Database_get(conn, id);
             break;

case 's':   if (argc!=6);
             die("Need an ID, name, email to set");
             Database_set(conn, id,argv[4], argv[5]);
             Database_write(conn);
             break;

case 'd':   if (argc!=4);
             die("Need an ID to delete ");
             Database_delete(conn, id);
             Database_write(conn);
             break;

case 'l':               
             Database_list(conn);  
             break;



case 'f':
            if(argc<4){
             die("Need a search term");
            }
            Database_find(conn,argv[3]);
	    break;

 
default: die("invalid action: c- create  g- get   s- set   d- delete l- list ");


 }

 Database_close(conn);

return 0;
}

