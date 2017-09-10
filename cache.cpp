#include<iostream>
#include<fstream>
#include<string>
#include <stdio.h>   
#include <stdlib.h>
#include<vector>
#include<sstream>
#include<math.h> 
#include<sys/time.h>
#include <chrono>
#include<ctime>

using namespace std;

//create a struct with addr and behavior
struct cache{
	int type;
	unsigned long long addr;	

};

//-----------------------------DIRECT MAPPED-----------------------------------------------
void directMapped(vector<cache> inst, int numInst,ofstream &outfile){

	//ofstream outfile(output);
	int hit;
	int lineSize = 32;
	int cacheSizes[] = {1024, 4096, 16384, 32768};
	int numBlocks; //number of blocks in the cache
	long lineIndex;
	long cacheIndex;
	long blockAddr;
	
	//loop through different cache sizes
	for(int i =0; i<4; i++){

		hit = 0;
		numBlocks = 0;		 
		lineIndex = 0;
		cacheIndex = 0;
		blockAddr = 0;
		numBlocks  = cacheSizes[i]/lineSize; //calculate the number of blocks in the cache
		std::vector<std::vector<long> > myCache(numBlocks, std::vector <long> (lineSize, 0));

		//loop through vector holding file information for each cache size
		for(int i = 0; i < inst.size(); i++){

			blockAddr = inst[i].addr / lineSize; //divide address by 32 (line size) to find the block address
			cacheIndex = blockAddr % numBlocks; //mod the block address by the number of blocks to find the cache index
			lineIndex = inst[i].addr % lineSize;//line index used to index into the vector

			//there is a cache hit if the current address is already in the cache
			if(myCache[cacheIndex][lineIndex] == inst[i].addr){
		
				hit ++;
				
			}
			//cache miss so fill in cache line
			else{

				for( int i = 0; i < lineSize; i++){

					myCache[cacheIndex][i] = blockAddr * lineSize+i;				

				}


			}
			

		}
		
		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits		

	}
	outfile<<"\n";

}

//-----------------------------SET ASSOCIATIVE-----------------------------------------------
void setAssociative(vector<cache> inst, int numInst,ofstream &outfile){
		
	//ofstream outfile(output);
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();
	struct timeval t;

	struct timeval tp;
	gettimeofday(&tp, NULL);

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numSets[] = {2, 4, 8, 16}; //create an associativity array
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	for( int i = 0 ; i < 4; i++){

		numBlocks  = cacheSize/lineSize/numSets[i]; //calculate the number of blocks in the cache
		struct lru c;
		c.time = -1;
		c.line.resize(lineSize, -1);

		std::vector <std::vector< struct lru>> myCache(numSets[i], std::vector < struct lru> (numBlocks,  c)); //create a vector of vectors holding lru structs

		hit = 0;
		lineIndex ;
		cacheIndex ; 

		//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
 
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if(check == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
 
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number

						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}

				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 

				//myCache[removeIndex][cacheIndex].time =(long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits


	}
	
	outfile<<"\n";

}

//-----------------------------FULLY ASSOCIATIVE LRU-----------------------------------------------
void fullyAssociativeLRU(vector<cache> inst, int numInst,ofstream &outfile){
	//exact same as lru above except associativity is now (cacheSize/lineSize) rather than array
	//ofstream outfile(output);
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();

	struct timeval t;

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	numBlocks  = cacheSize/lineSize/(cacheSize/lineSize); //calculate the number of blocks in the cache
	struct lru c;
	c.time = -1;
	c.line.resize(lineSize, -1);

	std::vector <std::vector< struct lru>> myCache((cacheSize/lineSize), std::vector < struct lru> (numBlocks,  c)); //create a vector of vectors holding lru structs

	hit = 0;
	lineIndex ;
	cacheIndex ; 

	//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if(check == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}
				
				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits



	outfile<<"\n";

}


//-----------------------------FULLY ASSOCIATIVE HOT COLD-----------------------------------------------
void fullyAssociativeHotCold(vector<cache> inst, int numInst,ofstream &outfile){
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	numBlocks  = cacheSize/lineSize; //calculate the number of blocks in the cache
	struct lru c;
	c.time = 0; //just initializing not using 
	c.line.resize(lineSize, -1);
	std::vector< int> tree(numBlocks-1,0); //create a vector for the tree
	std::vector< struct lru> myCache(numBlocks,  c); //create a vector of vectors holding lru structs

	hit = 0;
	lineIndex ;
	cacheIndex ; 
	

	//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = (inst[n].addr / lineSize) *lineSize;
			//cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;
			int hitIndex = -1;
			int treeIndex = -1;

			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k].line[lineIndex] == inst[n].addr){
					hitIndex = k;
					//hit++;
					
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache hit
			if(check == true){
				hit++; // change this put in loop above
				treeIndex = numBlocks + hitIndex -1;
				//go back up the tree to update the hot cold values
				while(treeIndex != 0){

					//update left child 
					if(treeIndex % 2 != 0){

						treeIndex = (treeIndex -1)/2; //update the new treeIndex
						tree[treeIndex] = 1; //set left child to hot since we found a hit
					}
					//update the right child (even numbers)
					else{
						treeIndex = (treeIndex -2)/2;		
						tree[treeIndex] = 0; //set right child to cold 
					}
					

				}	
				

			}

			//cache miss
			else{
				
				//update the tree
				int remove = -1;
				treeIndex = 0;

				for(int y = 0; y < log2(numBlocks); y++){

					//if the tree index is hot, then make it cold and update the tree index
					if(tree[treeIndex] == 1){

						tree[treeIndex] =0;
						treeIndex = treeIndex*2 +2;
					}
					else{
					//if the tree index is cold, then make it hot and update the tree index
						tree[treeIndex] =1;
						treeIndex = treeIndex*2 +1;

					}
	
				}

				remove = treeIndex -(numBlocks -1);
				for(int j = 0; j <myCache[remove].line.size(); j++){

					myCache[remove].line[j] = blockAddr + j;
				}
					

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits



	outfile<<"\n";



}


//----------------------SET ASSOCIATIVE NO WRITE MISS ALLOCATION-----------------------------------------------

//exact same as the set associative except ignore misses for store
void setAssociativeNoWriteMissAlloc(vector<cache> inst, int numInst,ofstream &outfile){
		
	//ofstream outfile(output);
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();

	struct timeval t;

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numSets[] = {2, 4, 8, 16}; //create an associativity array
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	for( int i = 0 ; i < 4; i++){

		numBlocks  = cacheSize/lineSize/numSets[i]; //calculate the number of blocks in the cache
		struct lru c;
		c.time = -1;
		c.line.resize(lineSize, -1);

		std::vector <std::vector< struct lru>> myCache(numSets[i], std::vector < struct lru> (numBlocks,  c)); //create a vector of vectors holding lru structs

		hit = 0;
		lineIndex ;
		cacheIndex ; 

		//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if((check == false) && (inst[n].type != 1)){ //only count misses if its not a store
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}
				
				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits


	}
	
	outfile<<"\n";

}


//-----------------------------SET ASSOCIATIVE NEXT LINE PREFETCHING-------------------------------------------
//do next line prefetching after checking current line

void setAssociativeNextLinePref(vector<cache> inst, int numInst,ofstream &outfile){
		
	//ofstream outfile(output);
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();

	struct timeval t;

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numSets[] = {2, 4, 8, 16}; //create an associativity array
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	for( int i = 0 ; i < 4; i++){

		numBlocks  = cacheSize/lineSize/numSets[i]; //calculate the number of blocks in the cache
		struct lru c;
		c.time = -1;
		c.line.resize(lineSize, -1);

		std::vector <std::vector< struct lru>> myCache(numSets[i], std::vector < struct lru> (numBlocks,  c)); //create a vector of vectors holding lru structs

		hit = 0;
		lineIndex ;
		cacheIndex ; 

		//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if(check == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}
				
				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}

			//now do prefetch
			bool prefetchHit = false; // use a bool to check if there was a hit for the prefetch
			cacheIndex = (cacheIndex + 1) % numBlocks;
			blockAddr++; //increment the block address

			for(int k = 0; k< myCache.size(); k++){
				//find if the prefetch causes a cache hit
				if(myCache[k][cacheIndex].line[0] == blockAddr * lineSize){
					
					prefetchHit = true;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count(); // update time stamp used
					break; //break out of the loop since we found a hit
		
				}

			}
			
			//if there was a cache miss for the line prefetched (basically repeat everything above for prefetch hit)
			if(prefetchHit == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}
				//update time stamp
				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits


	}
	
	outfile<<"\n";

}


//-----------------------------SET ASSOCIATIVE PREFETCH ON A MISS-------------------------------------------
//just move prefetch into cache miss section 

void setAssociativePrefOnMiss(vector<cache> inst, int numInst,ofstream &outfile){
		
	//ofstream outfile(output);
	struct lru{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();

	struct timeval t;

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numSets[] = {2, 4, 8, 16}; //create an associativity array
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	for( int i = 0 ; i < 4; i++){

		numBlocks  = cacheSize/lineSize/numSets[i]; //calculate the number of blocks in the cache
		struct lru c;
		c.time = -1;
		c.line.resize(lineSize, -1);

		std::vector <std::vector< struct lru>> myCache(numSets[i], std::vector < struct lru> (numBlocks,  c)); //create a vector of vectors holding lru structs

		hit = 0;
		lineIndex ;
		cacheIndex ; 

		//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;
					myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if(check == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}
				
				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}	




				//now do prefetch
				bool prefetchHit = false; // use a bool to check if there was a hit for the prefetch
				cacheIndex = (cacheIndex + 1) % numBlocks;
				blockAddr++; //increment the block address

				for(int k = 0; k< myCache.size(); k++){
					//find if the prefetch causes a cache hit
					if(myCache[k][cacheIndex].line[0] == blockAddr * lineSize){
					
						prefetchHit = true;
						myCache[k][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count(); // update time stamp used
						break; //break out of the loop since we found a hit
		
					}

				}
			
				//if there was a cache miss for the line prefetched (basically repeat everything above for prefetch hit)
				if(prefetchHit == false){
					double lowestTime = 1000000000000; //default to a very large number
					int removeIndex = -1;
	
					for(int p = 0; p< myCache.size(); p++){

						//find least recently used to kick out
						if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number
							lowestTime = myCache[p][cacheIndex].time; //update lowest time
							removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
						}

					}
					//update time stamp
					myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 
					for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

						myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
					}			
			

				}		
			

			}

			


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits


	}
 
}

//-----------------------------FIFO BONUS-----------------------------------------------
//same as the lru except dont keep track of the time hit was found (just use time stamp when inserting new value into cache)
void fifo(vector<cache> inst, int numInst,ofstream &outfile){
		
	//ofstream outfile(output);
	struct fifo{
		
		double time; //keeps track of when its last used
		std::vector<long> line;

	};

	std::chrono::high_resolution_clock::time_point start;
	start = std::chrono::high_resolution_clock::now();
	struct timeval t;

	struct timeval tp;
	gettimeofday(&tp, NULL);

	int lineSize = 32; //line size = 32
	int cacheSize = 16384; //set cache size to 16384
	int numSets[] = {2, 4, 8, 16}; //create an associativity array
	int numBlocks; 
	int hit;
	long blockAddr;
	long lineIndex;
	long cacheIndex;

	for( int i = 0 ; i < 4; i++){

		numBlocks  = cacheSize/lineSize/numSets[i]; //calculate the number of blocks in the cache
		struct fifo c;
		c.time = -1;
		c.line.resize(lineSize, -1);

		std::vector <std::vector< struct fifo>> myCache(numSets[i], std::vector < struct fifo> (numBlocks,  c)); //create a vector of vectors holding fifo structs

		hit = 0;
		lineIndex ;
		cacheIndex ; 

		//loop through vector holding file information for each cache size
		for(int n = 0; n < inst.size(); n++){
			bool check = false; //check if theres a hit
			blockAddr = inst[n].addr / lineSize;
			cacheIndex = blockAddr % numBlocks;
			lineIndex = inst[n].addr % lineSize;


			for(int k = 0; k< myCache.size(); k++){
				//find cache hits
				if(myCache[k][cacheIndex].line[lineIndex] == inst[n].addr){
					
					hit++;					
					check = true; //there was a hit so set check to T
					break; //break out of the loop since we found a hit
		
				}

			}

			//if there was a cache miss
			if(check == false){
				double lowestTime = 1000000000000; //default to a very large number
				int removeIndex = -1;
	
				for(int p = 0; p< myCache.size(); p++){

					//find least recently used to kick out
 
					if(myCache[p][cacheIndex].time < lowestTime){ //time has to be less than random number

						lowestTime = myCache[p][cacheIndex].time; //update lowest time
						removeIndex = p; //p becomes the index to kick out since it has the lowest time			

					
					}

				}

				myCache[removeIndex][cacheIndex].time = std::chrono::duration<double, std::milli> (std::chrono::high_resolution_clock::now() - start).count();//put new timestamp 

				
				for(int m = 0; m < myCache[removeIndex][cacheIndex].line.size();m++){

					myCache[removeIndex][cacheIndex].line[m] = blockAddr * lineSize + m;
				}			
			

			}


		}

		outfile<<hit<<","<<numInst<<"; "; //populate output file with hits


	}
	
	outfile<<"\n";

}

int main(int argc, char *argv[]) {

	int numInst = 0;
	string type1;
	unsigned long long addr1 = 0;
	cache myCache;

	//push structs into vector inst
	std::vector<cache>inst;

	//input file
	ifstream infile(argv[1]);

	//placing values from input file into struct
	while(infile >> type1>>std::hex>>addr1){

		numInst++; //get total count of file		

		if(type1=="L")
			myCache.type = 0; //use 0 for load
		else
			myCache.type = 1; //use 1 for store

		myCache.addr = addr1;

		inst.push_back(myCache); // insert into vector

	}

	infile.close();

	ofstream outfile(argv[2]);
	ofstream bonusFile("bonus.txt");

	directMapped(inst,numInst, outfile);
 	setAssociative(inst, numInst,outfile);
	fullyAssociativeLRU(inst, numInst,outfile);
	fullyAssociativeHotCold(inst, numInst,outfile);		
	setAssociativeNoWriteMissAlloc(inst, numInst,outfile);
	setAssociativeNextLinePref(inst, numInst,outfile);
	setAssociativePrefOnMiss(inst, numInst,outfile);
	bonusFile<<"Fifo policy: "<<endl;
	fifo(inst, numInst,bonusFile);

	outfile.close();
	bonusFile.close();


}




