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
      cout<< endl;

      myfile.getline( str, n);  
      
      char *ptr2 = strtok(str, " "); 
      while (ptr2 != NULL){
  	R_b[block_index] = atoi(ptr2);
      	block_index++;
        ptr2 = strtok (NULL, " ");  
      }
      cout<< endl;
    
      myfile.close();
    }
    

  }

  

};


class sparse_array{
public:
  int array_size;
  vector<int> b;
  vector<string> strings; 

  void create(uint64_t size){
    array_size = size;
    srand(time(NULL) );

     for (int i = 0; i < size/2; i++){
      b.push_back(1);
    }
    for (int i = size/2; i < size; i++){
      b.push_back(0);
    }
  }

  void append(string elem, uint64_t pos){
    strings.push_back(elem);
    b[pos] =1;
  }

  bool get_at_rank(uint64_t r, std::string& elem){
    elem = strings[r];
    rank_support rank(&b);
    return (rank.rank1(array_size)>= r);
  }

  bool get_at_index(uint64_t r, std::string& elem){
    if(b[r] == 1){
    rank_support rank(&b);
    int string_index = rank.rank1(r);
    elem = strings[string_index];
    return true;
    }
    return false;
  }

  uint64_t num_elem_at(uint64_t r){
    rank_support rank(&b);
    return rank.rank1(r);
  }

  uint64_t size(){return array_size;}

  uint64_t num_elem(){
    rank_support r(&b);
    return r.rank1(array_size);
  }
  
  void save(string& fname){
    ofstream file (fname);
    if (file.is_open()){

      //save bitvector
      for (auto i = b.begin(); i != b.end(); ++i){
  	file << *i;
      }
      file << "\n";
    
      file.close();
    }
    
  }

  void load(string& fname) {
    string line;
    ifstream myfile (fname);
    if (myfile.is_open()){
      getline (myfile,line);
      
      for (int i = 0; i < line.size(); i++){
  	b.push_back(line[i] - '0');   
      }
    
      myfile.close();
    }

  }

};




int main (int argc, char *argv[]) {

  string line;
  cin >> line;

  if(line.compare("Create") == 0){
    sparse_array s;
    cout << "Enter number of bits: " <<endl;
    int size;
    cin >> size;

    s.create(size);

    cout <<"Type in functions that you want to do, followed by \"end\": " <<endl;
    cin >> line;


    while(line.compare("end") != 0){
   
      if(line.compare("append") == 0){
	string elem;
	int pos;
	cout <<"Enter element: " <<endl;
	cin >> elem;
	cout <<"Enter position: " <<endl;
	cin >> pos;
	struct timeval begin, end;
	gettimeofday(&begin, 0);
    
	s.append(elem, pos);
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

      } if(line.compare("get_at_rank") == 0){
	string elem;
	int r;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	struct timeval begin, end;
	gettimeofday(&begin, 0);
    
	if(s.get_at_rank(r, elem)){
	  cout<<"True"<<endl;
	}else{
	  cout<<"False"<<endl;
	}
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

      } if(line.compare("get_at_index") == 0){
	string elem;
	int r;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	struct timeval begin, end;
	gettimeofday(&begin, 0);
	if(s.get_at_index(r, elem)){
	  cout<<"True" <<endl;
	}else{
	  cout<<"False"<<endl;
	}
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

      } if(line.compare("num_elem_at") == 0){
	int r;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	struct timeval begin, end;
	gettimeofday(&begin, 0);
    
	cout <<  s.num_elem_at(r) << endl;
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

      } if(line.compare("size") == 0){
	struct timeval begin, end;
	gettimeofday(&begin, 0);
    
	cout << s.size() <<endl;
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;

      } if(line.compare("num_elem") == 0){
	struct timeval begin, end;
	gettimeofday(&begin, 0);
    
	cout<<s.num_elem()<<endl;
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;

	cout << "Time elapsed: " <<elapsed << " seconds" <<endl;
      }   

      cin >>line;      

    }

      cout << "Do you want to save? (y/n)" << endl;
      cin >> line;
      
      if(line.compare("y") == 0 || line.compare( "Yes") ==0){
	cout << "Enter filename: " << endl;
	cin >> line;
	s.save(line);
      }
    
  }else if(line.compare("Load") == 0){
    sparse_array s;
    string filename;
    cout << "Enter filename: " << endl;
    cin >> filename;

    s.load(filename);
    
    while(line.compare("end") != 0){
      if(line.compare("append") == 0){
	string elem;
	int pos;
	cout <<"Enter element: " <<endl;
	cin >> elem;
	cout <<"Enter position: " <<endl;
	cin >> pos;
	s.append(elem, pos);
	cout<< "Done"<<endl;

      } if(line.compare("get_at_rank") == 0){
	string elem;
	int r;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	cout <<"Enter element: " <<endl;
	cin >> elem;

	if(s.get_at_rank(r, elem)){
	  cout<<"True" <<endl;
	}else{
	  cout<<"False"<<endl;
	}

      } if(line.compare("get_at_index") == 0){
	string elem;
	int r;
	cout <<"Enter element: " <<endl;
	cin >> elem;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	if(s.get_at_index(r, elem)){
	    cout<<"True" <<endl;
	  }else{
	    cout<<"False"<<endl;
	  }

      } if(line.compare("num_elem_at") == 0){
	int r;
	cout <<"Enter rank: " <<endl;
	cin >> r;
	cout <<  s.num_elem_at(r) << endl;

      } if(line.compare("size") == 0){
	cout << s.size() <<endl;

      } if(line.compare("num_elem") == 0){
	cout<<s.num_elem()<<endl;
      }

      cin >>line;      

    }

  }
    
  return 0;
}
