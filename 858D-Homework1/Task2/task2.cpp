#include <vector>
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <cstring>
#include <sys/time.h>
using namespace std;


class rank_support{
  
public:
  vector<int> bit_vector;
  int * R_s =NULL;
  int * R_b = NULL;
  int R_p = 0, s=0, b=0,n=0;
  int superblock_index =0;
  int block_index = 0;
  
  rank_support(vector<int>* vector){ 
    //copy vector as an attribute to access in other functions
    for (auto i = vector->begin(); i != vector->end(); ++i){
      bit_vector.push_back(*i);
      
    }

    n = bit_vector.size();
    s = (int)(log2(n)*log2(n))/2;
    b = (int)(log2(n))/2;
    if(s==0){s=1;}
    if(b==0){b=1;}

    //If R_s and R_b don't exist, then create them 
    if (R_s ==NULL && R_b ==NULL){

      //create tables
      R_s =new int[n/s];
      R_b = new int[n/b];

      int ones_counter =0;
      for (int i = 0; i < n; i++){

	if(i%s ==0){
	  R_s[superblock_index] = ones_counter;
	  superblock_index++;
	}

	if(i%b == 0){
	  R_b[block_index] = ones_counter - R_s[superblock_index-1];
	  block_index++;
	}
  
	if(bit_vector[i] == 1){
	  ones_counter++;
	}
     
      }

    }
  }

  rank_support* clone() const { return new rank_support(*this); }


  uint64_t rank1(uint64_t i){
      
    int rank;
    int x = (i-1)%b;
    
    for(int j=i-1-x;j<i; j++){
      R_p+=bit_vector[j];
    }

    int R_s_value =  R_s[(int)((i-1)/s)];
    int R_b_value=  R_b[(int)((i-1)/b)];
    
    rank = R_s_value + R_b_value + R_p;
    return rank;
  }
  

  uint64_t overhead(){   
    int superblock_bits = (2*n)/log2(n);
    int block_bits = (n/b)*(log2(log2(n)));
    return superblock_bits + block_bits; 

  }


  void save(string fname){
    ofstream file (fname);
    if (file.is_open()){

      //save bitvector
      for (auto i = bit_vector.begin(); i != bit_vector.end(); ++i){
  	file << *i;
      }
      file << "\n";

      //Save superblocks
      for(int i=0; i<superblock_index;i ++){
  	file << R_s[i]<< " ";
      }
      file << "\n";

      //Save blocks
      for(int i=0; i<block_index;i ++){
  	file << R_b[i] << " ";
      }
      file << "\n";
    
      file.close();
    }
  }

  void load(string fname){

    string line;
    s = (int)(log2(n)*log2(n))/2;
    b = (int)(log2(n))/2;
  
    //create tables
    R_s =new int[n/s];
    R_b = new int[n/b];
      
    ifstream myfile (fname);
    if (myfile.is_open()){

      char str[n];
      getline(myfile,line);
      myfile.getline( str, n);  
      
      char *ptr = strtok(str, " "); 
      while (ptr != NULL){
  	R_s[superblock_index] = atoi(ptr);
      	superblock_index++;
        ptr = strtok (NULL, " ");  
      }

      myfile.getline( str, n);  
      
      char *ptr2 = strtok(str, " "); 
      while (ptr2 != NULL){
  	R_b[block_index] = atoi(ptr2);
      	block_index++;
        ptr2 = strtok (NULL, " ");  
      }
    
      myfile.close();
    }
    

  }

  

};


class select_support{
public:
  rank_support* r2; 
  
  select_support(rank_support* r){
    r2 = r->clone();
    
  }

  uint64_t select1(uint64_t i){
    return select_helper(i, r2->n, r2->bit_vector, (r2->n)/2);
  }


  int select_helper(uint64_t ones, int n, vector<int> v, int actual_index){

    int index = n/2;
    rank_support r(&v);
    int rank = r.rank1(index);

    if(rank == ones && v[index-1] !=0){
      return actual_index;
    }else if(rank >= ones){
      
      auto first = v.begin();
      auto last = v.begin() + n/2;
      actual_index-= index/2;
      vector<int> new_vector(first, last);
      return select_helper(ones, n/2, new_vector, actual_index);
    }else{
      auto first = v.begin() + n/2;
      auto last = v.end();
      vector<int> new_vector(first, last);
      actual_index+= index/2;
      return select_helper(ones-rank, new_vector.size(), new_vector, actual_index);

    }

  }


  uint64_t overhead(){
    return r2->overhead();
  }

  void save(string fname){  return r2->save(fname);}
  
  void load(string fname){  return r2->load(fname);}

};




int main (int argc, char *argv[]) {

  string line;
  cin >> line;

  if(line.compare("Create") == 0){
    vector<int> b;
    cout << "Enter number of bits: " <<endl;
    int size;
    cin >> size;

    srand(time(NULL) );

    if(size <= 100){

    cout << "Bitvector is : ";
    }

    for (int i = 0; i < size; i++){
      int bit=rand()%2;
      if(size <= 100){
      cout << bit << " ";
      }
      b.push_back(bit);
    }

    if(size <= 100){
    cout<<endl;
    }

    rank_support r(&b);
    select_support s(&r);

    cout<<"Number of ones is " << r.rank1(size) <<endl;

    cout << "Enter select: " <<endl;
    int select;
    cin >> select;
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    cout << "Select " <<  select << " is index " << s.select1(select) << endl;

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

    cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

    rank_support r2(&b);
    select_support s2(&r);
    cout<< "Overhead of rank data structure: " << s2.overhead() << " bits"<<endl;
    cout << "Do you want to save? (y/n)" << endl;
    cin >> line;
      
    if(line.compare("y") == 0 || line.compare( "Yes") ==0){
      cout << "Enter filename: " << endl;
      cin >> line;
      s2.save(line);
    }  
    
    
  }else if(line.compare("Load") == 0){
    // //Load bitvector and rank structure from file
    string filename;
    vector<int> b;
    cout << "Enter filename: " << endl;
    cin >> filename;
    
    ifstream myfile (filename);
    if (myfile.is_open()){
      getline (myfile,line);
      
      for (int i = 0; i < line.size(); i++){
  	b.push_back(line[i] - '0');   
      }
    
      myfile.close();
    }

  
    rank_support r(&b);
    select_support s(&r);

    cout << "Enter select: " <<endl;
    int select;
    cin >> select;
      struct timeval begin, end;
    gettimeofday(&begin, 0);
    
    cout << "Select " <<  select << " is " << s.select1(select) << endl;

     gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

    cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

    rank_support r2(&b);
    select_support s2(&r);
    cout<< "Overhead of rank data structure: " << s2.overhead() << " bits"<<endl;
  }
    
  return 0;
}

