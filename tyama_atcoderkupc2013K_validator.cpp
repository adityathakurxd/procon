#include <iostream>
#include <ext/stdio_filebuf.h>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

#define N 16

pair<char,char> list[N]={
	make_pair('A','B'),
	make_pair('B','A'),make_pair('B','C'),make_pair('B','E'),
	make_pair('C','B'),make_pair('C','D'),
	make_pair('D','C'),
	make_pair('E','B'),make_pair('E','F'),
	make_pair('F','E'),make_pair('F','G'),
	make_pair('G','F'),make_pair('G','H'),
	make_pair('H','G'),make_pair('H','I'),
	make_pair('I','H'),
};
string TAG="[Validator] ";
//Reactive
int main(int argc, char **argv){
	if(argc<2){
		cerr<<"validator [program]"<<endl;
		cerr<<"program must be chmod +x"<<endl;
		return 0;
	}
	const char *cmd=argv[1];

	set<pair<char,char> >se;
	for(int i=0;i<N;i++)se.insert(list[i]);

/*
Example:
18
00111001
*/

	int n;
	string dec,enc,dec2;
	cin>>n>>dec;

//encode phase.
{
	//Initialize
	int fd_in[2],fd_out[2];
	pipe(fd_in);
	pipe(fd_out);

	int pid=fork();
	if(pid==-1){
		cout<<"InternalError"<<endl;
		return 0;
	}
	if(pid==0){//child
		close(0);
		dup2(fd_in[0],0);
		close(1);
		dup2(fd_out[1],1);
		close(fd_in[0]);
		close(fd_in[1]);
		close(fd_out[0]);
		close(fd_out[1]);
		execl(cmd,cmd,NULL);
		return 0;
	}
	//parent
	close(fd_in[0]);
	close(fd_out[1]);
	FILE *fin=fdopen(fd_out[0],"r");
	__gnu_cxx::stdio_filebuf<char> bin(fin, ios_base::in);
	istream pin(&bin);

	{
		FILE *fout=fdopen(fd_in[1],"w");
		__gnu_cxx::stdio_filebuf<char> bout(fout, ios_base::out);
		ostream pout(&bout);
		pout<<"encode"<<endl;
		pout<<dec.size()<<endl;
		pout<<dec<<endl;
		fclose(fout);
	}
	getline(pin,enc);
	fclose(fin);
	cerr<<TAG<<dec.size()<<" encoded into "<<enc.size()<<"."<<endl;
	for(int i=0;i<enc.size()-1;i++){
		if(se.find(make_pair(enc[i],enc[i+1]))==se.end()){
			cerr<<TAG<<"Verdict:"<<endl;
			cout<<"WA: Encoded string is not good."<<endl;
			return 0;
		}
	}
}

//decode phase.
{
	//Initialize
	int fd_in[2],fd_out[2];
	pipe(fd_in);
	pipe(fd_out);

	int pid=fork();
	if(pid==-1){
		cout<<"InternalError"<<endl;
		return 0;
	}
	if(pid==0){//child
		close(0);
		dup2(fd_in[0],0);
		close(1);
		dup2(fd_out[1],1);
		close(fd_in[0]);
		close(fd_in[1]);
		close(fd_out[0]);
		close(fd_out[1]);
		execl(cmd,cmd,NULL);
		return 0;
	}
	//parent
	close(fd_in[0]);
	close(fd_out[1]);
	FILE *fin=fdopen(fd_out[0],"r");
	__gnu_cxx::stdio_filebuf<char> bin(fin, ios_base::in);
	istream pin(&bin);

	{
		FILE *fout=fdopen(fd_in[1],"w");
		__gnu_cxx::stdio_filebuf<char> bout(fout, ios_base::out);
		ostream pout(&bout);
		pout<<"decode"<<endl;
		pout<<enc.size()<<endl;
		pout<<enc<<endl;
		fclose(fout);
	}

	getline(pin,dec2);
	fclose(fin);
	if(dec==dec2){
		if(enc.size()<=dec.size()+10){
			cerr<<TAG<<"Verdict:"<<endl;
			cout<<"AC (Level2)"<<endl;
		}else if(enc.size()<=2*dec.size()+10){
			cerr<<TAG<<"Verdict:"<<endl;
			cout<<"AC (Level1)"<<endl;
		}else{
			cerr<<TAG<<"Verdict:"<<endl;
			cout<<"WA: Decoding was OK, but encoded string was too long."<<endl;
		}
	}else{
		cerr<<TAG<<"Verdict:"<<endl;
		cout<<"WA: Decoding was wrong."<<endl;
	}
}

	return 0;
}