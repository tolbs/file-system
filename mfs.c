/**************************************************************
* Class:  CSC-415-02 Summer 2020
* Name: Team Penta - Duy Nguyen, Taylor Artunian, Wameedh Mohammed Ali
* Student ID: 917446249 (Duy), 920351715 (Taylor), 920678405 (Wameedh)
* Project: Basic File System - PentaFS
*
* File: mfs.c
*
* Description: This file defines the methods to initialize, keep track of, and change our file system's inodes.
*
**************************************************************/

#include "mfs.h"

mfs_DIR* inodes;

size_t NumberOfElementsInInodesArray = sizeof(inodes)/sizeof(inodes[0]); // calculate the number of elemnts in inodes array

void mfs_init() {
  printf("----------------------------mfs_init----------------------------\n");

  uint64_t totalBytes = getVCB()->totalInodeBlocks * getVCB()->blockSize;
  printf("totalInodeBlocks %ld, blockSize %ld\n", getVCB()->totalInodeBlocks, getVCB()->blockSize);
  printf("Allocating %ld bytes for inodes.\n", totalBytes);

  inodes = calloc(getVCB()->totalInodeBlocks, getVCB()->blockSize);
  printf("Inodes allocated at %p.\n", inodes);

  uint64_t blocksRead = LBAread(inodes, getVCB()->totalInodeBlocks, getVCB()->inodeStartBlock);
  printf("Loaded %ld blocks of inodes into cache.\n", blocksRead);
  
  if(blocksRead != getVCB()->totalInodeBlocks) {
    printf("Error: Not all inodes loaded into cache.\n");
    mfs_close();
    exit(0);
  }


 mfs_setcwd("/root");

  printf("----------------------------------------------------------------\n");
}

void writeInodes() {
  printf("--------------------------writeInodes---------------------------\n");
  LBAwrite(inodes, getVCB()->totalInodeBlocks, getVCB()->inodeStartBlock);
  printf("----------------------------------------------------------------\n");
}

/* Do not pass invalid InodeTypes. */
char inodeTypeNames[3][64] = { "I_FILE", "I_DIR", "I_UNUSED" };

char* getInodeTypeName(char* buf, InodeType type) {
  strcpy(buf, inodeTypeNames[type]);
  return buf;
}

//holding our current directory's path
char currentDirectoryPath[MAX_FILEPATH_SIZE];
char currentDirectoryPathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE];
int currentDirectoryPathArraySize = 0;

//hold each level of a path
char requestedFilePath[MAX_FILEPATH_SIZE];
char requestedFilePathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE];
int requestedFilePathArraySize = 0;

void parseFilePath(const char *pathname) { 
  printf("------------------------Parsing File Path-----------------------\n");
  
  printf("Input: %s\n", pathname);

  /* Clear previous value and count. */
  requestedFilePath[0] = '\0';
  requestedFilePathArraySize = 0;

  /* Make mutable copy of pathname. */
  char _pathname[MAX_FILEPATH_SIZE];
  strcpy(_pathname, pathname);

  /* Setup tokenizer. */
  char* savePointer;
  char* token = strtok_r(_pathname, "/", &savePointer);

  /* Categorize the pathname. */
  int isAbsolute = pathname[0] == '/';
  int isSelfRelative = !strcmp(token, ".");
  int isParentRelative = !strcmp(token, "..");
  // Fourth case: relative to cwd


  if(token && !isAbsolute) {
    int maxLevel = isParentRelative ? currentDirectoryPathArraySize - 1 : currentDirectoryPathArraySize;
    for(int i=0; i<maxLevel; i++) {
      strcpy(requestedFilePathArray[i], currentDirectoryPathArray[i]);
      sprintf(requestedFilePath, "%s/%s", requestedFilePath, currentDirectoryPathArray[i]);
      requestedFilePathArraySize++;
    }
  }

  /* Discard '.' or '..'. */
  if(isSelfRelative || isParentRelative) {
    token = strtok_r(0, "/", &savePointer);
  }

  while(token && requestedFilePathArraySize < MAX_DIRECTORY_DEPTH) {

    strcpy(requestedFilePathArray[requestedFilePathArraySize], token);
    sprintf(requestedFilePath, "%s/%s", requestedFilePath, token);

    /* Printf for debug. */
    printf("\t%s\n", token);
    
    requestedFilePathArraySize++;
    token = strtok_r(0, "/", &savePointer);

  }

  printf("Output: %s\n", requestedFilePath);

printf("----------------------------------------------------------------\n");

}

//Added to test parseFileName
void printFilePath() {

  for(int i=0; i<requestedFilePathArraySize; i++) {

    if(i<requestedFilePathArraySize-1) {
      printf("Directory %d: %s\n", i, requestedFilePathArray[i]);
    } else {
      printf("Filename: %s\n", requestedFilePathArray[i]);
    }

  }
}

mfs_DIR* getInode(const char *pathname){
  printf("----------------------------getInode----------------------------\n");
  // Loop over inodes, find requested node, return that node, if does not exist return NULL

  printf("Searching for path: '%s'\n", pathname);
  for (size_t i = 0; i < getVCB()->totalInodes; i++) {
    printf("\tInode path: '%s'\n", inodes[i].path);
    if (strcmp(inodes[i].path, pathname) == 0) {
      printf("----------------------------------------------------------------\n");
      return &inodes[i];
    }
  }

  printf("Inode with path '%s' does not exist.\n", pathname);

  printf("----------------------------------------------------------------\n");

  return NULL;

}

mfs_DIR* getFreeInode(){
  printf("--------------------------getFreeInode--------------------------\n");
  // Search through inodes, return the first avalable inode,
  // Update inUse of the returned node to 1 (Which means it's in use right now)
  // if there is no free inode return NULL
  mfs_DIR* returnediNode;

  for (size_t i = 0; i < getVCB()->totalInodes; i++) {
    if (inodes[i].inUse == 0) { // if the inode inUse equals 0, that means it is free so we return it
      inodes[i].inUse = 1; // update the node to be in use before returning it
      returnediNode = &inodes[i];
      
      printf("----------------------------------------------------------------\n");
      
      return returnediNode;
      
    }
  }

  printf("----------------------------------------------------------------\n");

  return NULL;
}

mfs_DIR* createInode(InodeType type, const char* path){
  printf("--------------------------createInode---------------------------\n");

  // returns an inode if it succeeds and NULL if it could not create the inode
  mfs_DIR* inode;
  char parentPath[MAX_FILEPATH_SIZE];
  mfs_DIR* parentNode;

  /* Obtain current time. */
  time_t currentTime;
  currentTime = time(NULL);

  // call checkValidityOfPath() if fales return NULL
  if (checkValidityOfPath() == 0){
    printf("----------------------------------------------------------------\n");
    return NULL;
  }

  // call getFreeInode() to recieve the next available inode
  inode = getFreeInode();

  //find and assign the parent to the new inode
  getParentPath(parentPath, path);
  parentNode = getInode(parentPath);
  
  /* Set properties on inode. */
  inode->type = type;
  strcpy(inode->name , requestedFilePathArray[requestedFilePathArraySize - 1]);
  sprintf(inode->path, "%s/%s", parentPath, inode->name);
  inode->lastAccessTime = currentTime;
  inode->lastModificationTime = currentTime;

  /* Try to set the parent. If it fails, revert. */
  if (!setParent(parentNode, inode)) {
    freeInode(inode);
    printf("Error setting parent. Reverting changes.\n");
    printf("----------------------------------------------------------------\n");
    return NULL;
  }
    
  printf("Sucessfully created inode for path '%s'.\n", path);       
  printf("----------------------------------------------------------------\n");
  return inode;

}

int parentHasChild(mfs_DIR* parent, mfs_DIR* child) {

  /* Loop through children. Return 1 on the first name match. */
  for( int i = 0; i < parent->numChildren; i++ ) {
    if(!strcmp(parent->children[i], child->name)) {
      return 1;
    }
  }

  return 0;
}

//set the given parent and child inodes to each other if they exist
//returns 0 if it can't, and 1 on success
int setParent(mfs_DIR* parent, mfs_DIR* child){
  printf("----------------------------setParent---------------------------\n");
  
  //checking if the parent can take anymore children in our file system
  if(parent->numChildren == MAX_NUMBER_OF_CHILDREN) {
    printf("Folder '%s' has maximum children.\n", parent->path);
    printf("----------------------------------------------------------------\n");
    return 0;
  }

  //checking if a child like the one passed, already exists
  if(parentHasChild(parent, child)) {
    printf("Folder '%s' already exists.\n", child->path);
    return 0;
  }

  //set the rest of the properties of the parent and child to correspond with each other
  strcpy(parent->children[parent->numChildren], child->name);
  parent->numChildren++;
  parent->lastAccessTime = time(0);
  parent->lastModificationTime = time(0);
  parent->sizeInBlocks += child->sizeInBlocks;
  parent->sizeInBytes += child->sizeInBytes;

  strcpy(child->parent, parent->path);
  sprintf(child->path, "%s/%s", parent->path, child->name);

  printf("Set parent of '%s' to '%s'.\n", child->path, child->parent);
  
  printf("----------------------------------------------------------------\n");
  return 1;
}

int removeFromParent(mfs_DIR* parent, mfs_DIR* child) {
  printf("-----------------------removeFromParent-------------------------\n");
  
  /* Loop through parent's list of children until name match. */
  for(int i=0; i<parent->numChildren; i++) {
    
    if(!strcmp(parent->children[i], child->name)) {

      /* Clear entry in parent's list of children. Decrement child count. */
      strcpy(parent->children[i], "");
      parent->numChildren--;
      parent->sizeInBlocks -= child->sizeInBlocks;
      parent->sizeInBytes -= child->sizeInBytes;
      return 1;
    }
  }

  printf("Could not find child '%s' in parent '%s'.\n", child->name, parent->path);
  printf("----------------------------------------------------------------\n");
  return 0;

}

//method will return NULL if the parent's path cannot be found, otherwise sets the passed buffer to the path and returns it
char* getParentPath(char* buf ,const char* path){
  printf("-------------------------getParentPath--------------------------\n");

  //Parses the requestedFilePathArray into a string to find the parent string "path"
  //Copy parent path to buf, return buf
  
  /* Parse the path. */
  parseFilePath(path);

  char parentPath[MAX_FILEPATH_SIZE] = "";

  /* Loop until the second to last element. */
  for(int i=0; i<requestedFilePathArraySize - 1; i++) {

    /* Append a separator and the next level of the path. */
    strcat(parentPath, "/");
    strcat(parentPath, requestedFilePathArray[i]);
  }

  strcpy(buf, parentPath);

  printf("Input: %s, Parent Path: %s\n", path, buf);
  
  printf("----------------------------------------------------------------\n");
  
  return buf;
}

//checks if a the path is valid
int checkValidityOfPath(){
  printf("----------------------checkValidityOfPath-----------------------\n");
  // Returns 0 if the path is invalid, and 1 if it is valid
  // It would loop over requestedFilePathArray and assemble each partial path by adding next level
  // Search for partial path with getInode and fails when it cannot found
  printf("----------------------------------------------------------------\n");
}

mfs_DIR* getInodeByID(int id) {
  if(0 <= id < getVCB()->totalInodes) {
    return &inodes[id];
  } else {
    return NULL;
  }
}

int writeBufferToInode(mfs_DIR* inode, char* buffer, size_t bufSizeBytes, uint64_t blockNumber) {
  printf("-----------------------writeBufferToInode-----------------------\n");

  /* Check if dataBlockPointers is full. */
  int freeIndex = -1;
  for(int i=0; i<MAX_DATABLOCK_POINTERS; i++) {
    if(inode->directBlockPointers[i] == INVALID_DATABLOCK_POINTER) {
      
      /* Record free dataBlockPointer index. */
      freeIndex = i;
      break;
    }
  }

  /* If there is no place to put the new dataBlock pointer. Return 0 blocks/bytes written. */
  if(freeIndex == -1) {
    return 0;
  }

  /* Write buffered data to disk, update inode, write inodes to disk. */
  LBAwrite(buffer, 1, blockNumber);

  /* Record the block number in the inode, reserve the block in the freeMap and write the VCB. */
  inode->directBlockPointers[freeIndex] = blockNumber;
  setBit(getVCB()->freeMap, blockNumber);
  writeVCB();

  inode->numDirectBlockPointers++;
  inode->sizeInBlocks++;
  inode->sizeInBytes += bufSizeBytes;
  inode->lastAccessTime = time(0);
  inode->lastModificationTime = time(0);

  writeInodes();

  printf("----------------------------------------------------------------\n");

  return 1;

}

void freeInode(mfs_DIR* node){
  printf("----------------------------freeInode---------------------------\n");
  
  printf("Freeing inode: '%s'\n", node->path);
  
  node->inUse = 0;
  node->type = I_UNUSED;
  node->name[0] = NULL;
  node->path[0] = NULL;
  node->parent[0] = NULL;
  node->sizeInBlocks = 0;
  node->sizeInBytes = 0;
  node->lastAccessTime = 0;
  node->lastModificationTime = 0;

  /* free the data blockes asociated with the file, if we are deleting a file */
  if(node->type == I_FILE){
    for (size_t i = 0; i < node->numDirectBlockPointers; i++) {
      int blockPointer = node->directBlockPointers[i];
      clearBit(getVCB()->freeMap, blockPointer);
    }
  }
  printf("----------------------------------------------------------------\n");

  /* Commit changes to disk. */
  writeInodes();

}

void mfs_close() {
  printf("----------------------------mfs_close---------------------------\n");
  free(inodes);
  printf("----------------------------------------------------------------\n");
}
 

int mfs_mkdir(const char *pathname, mode_t mode) { // return 0 for sucsess and -1 if not
  printf("----------------------------mfs_mkdir---------------------------\n");
  // Parses file name, adds info for inode mfs_DIR
  // Adds info to parent if necessary and checks if the folder already exists
  char parentPath[256] = "";
  parseFilePath(pathname);

  for (size_t i = 0; i < requestedFilePathArraySize - 1; i++) {
     strcat(parentPath, "/");
     strcat(parentPath, requestedFilePathArray[i]);
  }
  
  mfs_DIR* parent = getInode(parentPath);
  if (parent) {
    for (size_t i = 0; i < parent->numChildren; i++){
      if(strcmp(parent->children[i], requestedFilePathArray[requestedFilePathArraySize - 1])){
          printf("Folder already exists!\n");
          printf("----------------------------------------------------------------\n");
          return -1;
      }
    }
  } else {
    printf("Parent '%s' does not exist!\n", parentPath);
    printf("----------------------------------------------------------------\n");
    return -1;
  }
  

  if( createInode(I_DIR, pathname)){
    writeInodes();
    printf("----------------------------------------------------------------\n");
    return 0;
  }
  printf("Failed to make directory '%s'.\n", pathname);
  printf("----------------------------------------------------------------\n");
  return -1;
}

//Upon success, returns a 0 otherwise returns a -1
int mfs_rmdir(const char *pathname) {
  printf("----------------------------mfs_rmdir---------------------------\n");
  mfs_DIR* node = getInode(pathname);
  if(!node) {
    printf("%s does not exist.\n", pathname);
    return -1;
  }
  mfs_DIR* parent = getInode(node->parent);
  if (node->type == I_DIR && node->numChildren == 0){
    removeFromParent(parent,node);
    freeInode(node);
    printf("----------------------------------------------------------------\n");
    return 0;
  }
  printf("----------------------------------------------------------------\n");
  return -1;
}

mfs_DIR* mfs_opendir(const char *fileName) {
  printf("---------------------------mfs_opendir--------------------------\n");
  int ret = b_open(fileName, 0);
  if(ret < 0) {
    printf("----------------------------------------------------------------\n");
    return NULL;
  }
  printf("----------------------------------------------------------------\n");
  return getInode(fileName);
}

int readdirCounter = 0;
struct mfs_dirent directoryEntry;

struct mfs_dirent* mfs_readdir(mfs_DIR *dirp) {
  printf("--------------------------mfs_readdir---------------------------\n");
  
  if(readdirCounter == dirp->numChildren) {
    readdirCounter = 0;
    return NULL;
  }
  
  /* Get child inode. */
  char childPath[MAX_FILEPATH_SIZE];
  sprintf(childPath, "%s/%s", dirp->path, dirp->children[readdirCounter]);
  mfs_DIR* child = getInode(childPath);

  /* Set properties on dirent. */
  directoryEntry.d_ino = child->id;
  strcpy(directoryEntry.d_name, child->name);

  /* Increment the counter to the next child. */
  readdirCounter++;

  printf("----------------------------------------------------------------\n");
  return &directoryEntry;
}

int mfs_closedir(mfs_DIR *dirp) {
  printf("-------------------------mfs_closedir---------------------------\n");
  printf("----------------------------------------------------------------\n");
  // 
  return 0;
}

char * mfs_getcwd(char *buf, size_t size) {
  printf("---------------------------mfs_getcwd---------------------------\n");
  if(strlen(currentDirectoryPath) > size) {
    errno = ERANGE;
    printf("----------------------------------------------------------------\n");
    return NULL;
  }
  strcpy(buf, currentDirectoryPath);
  printf("----------------------------------------------------------------\n");
  return buf;
}

//Note: This does not currently check validity of 
//      the path. This may need to first check whether the provided path is within limit of MAX_FILENAME_SIZE
//      and set errno similar to mfs_getcwd.

int mfs_setcwd(char *buf) {
  printf("---------------------------mfs_setcwd---------------------------\n");
  
  parseFilePath(buf);

  /* Check if inode exists. */
  mfs_DIR* inode = getInode(requestedFilePath);
  if(!inode) {
    printf("Directory '%s' does not exist.\n", requestedFilePath);
    printf("----------------------------------------------------------------\n");
    return 1;
  }

  /* Clear previous cwd. */
  currentDirectoryPath[0] = '\0';
  currentDirectoryPathArraySize = 0;

  /* Copy parsed pathname to currentDirectoryPathArray. */
  for(int i=0; i<requestedFilePathArraySize; i++) {
    strcpy(currentDirectoryPathArray[i], requestedFilePathArray[i]);
    sprintf(currentDirectoryPath, "%s/%s", currentDirectoryPath, requestedFilePathArray[i]);
    currentDirectoryPathArraySize++;
  }

  printf("Set cwd to '%s'.\n", currentDirectoryPath);

  printf("----------------------------------------------------------------\n");
  return 0;

}

//Added to test mfs_setcwd
void printCurrentDirectoryPath() {

  for(int i=0; i<currentDirectoryPathArraySize; i++) {

    if(i<currentDirectoryPathArraySize-1) {
      printf("Directory %d: %s\n", i, currentDirectoryPathArray[i]);
    } else {
      printf("Filename: %s\n", currentDirectoryPathArray[i]);
    }

  }
}

//Initial implementation of mfs_isFile and mfs_isDir
int mfs_isFile(char * path) {
  printf("------------------------------isFile----------------------------\n");
  mfs_DIR* inode = getInode(path);
  printf("----------------------------------------------------------------\n");
  return inode ? inode->type == I_FILE : 0;
}

int mfs_isDir(char * path) {
  printf("------------------------------isDir-----------------------------\n");
  mfs_DIR* inode = getInode(path);
  printf("----------------------------------------------------------------\n");
  return inode ? inode->type == I_DIR : 0;
}

int mfs_delete(char* filePath) {
  printf("---------------------------mfs_delete---------------------------\n");
  //Get inode
  mfs_DIR* fileNode = getInode(filePath); 
  //Get parent
  mfs_DIR* parentNode = getInode(fileNode->parent);
  //Remove child from parent
  removeFromParent(parentNode, fileNode);
  //Clear properties on child inode so it is not found in search & Set inuse to false.
  freeInode(fileNode);
  
  printf("----------------------------------------------------------------\n");
  return 0;
}

// mfs_DIR needs rework to contain these fields.
int mfs_stat(const char *path, struct mfs_stat *buf) {
  printf("----------------------------mfs_stat----------------------------\n");
  mfs_DIR* inode = getInode(path);
  if(inode) {
    buf->st_size = 999;
    buf->st_blksize = getVCB()->blockSize;
    buf->st_blocks = 2;
    buf->st_accesstime = 1;
    buf->st_modtime = 1;
    buf->st_createtime = 1;
    printf("----------------------------------------------------------------\n");
    return 1;
  }
  printf("----------------------------------------------------------------\n");
  return 0;
}
