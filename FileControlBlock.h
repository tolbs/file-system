#define MAX_FCB 20

int linuxFd;	//holds the systems file descriptor
char * buf;		//holds the open file buffer
int index;		//holds the current position in the buffer
int blockIndex;		//holds the index of the block in the FCB
int buflen;		//holds how many valid bytes are in the buffer
int mode;   //fileMode mode; // sets when file if open for write 
int inote;  //mfs_DIR* inode; //holdsa pointer to the inode associated with the file 
int eof;
