#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;
using namespace std;

// functions
void inputFirstFollow(string fname);
set <string> computeFirst(string nt);
void computeFollow();
int makeParseTable();
void parseString(string &startSymbol);

// data structures used for parse table , follow , first , productions etc
unordered_map <string, vector<string>> prods;
unordered_map <string, set<string>> first;
unordered_map <string, set<string>> follow;
map <pair<string,string> , int> validPairs;
map <pair<string,string> , string> parseTable;

char lambda = '^';
vector <string> lambdaProds;
string startSymbol;
set <char> nonTerminals;
set <char> terminals;
vector <string> productionStrings;

int main(int argc, char *argv[])
{
	string fname = argv[1];
    inputFirstFollow(fname);
    int flag = makeParseTable();
    if(flag){
    	parseString(startSymbol);
    }	
    return 0;
}

//computing follow
void computeFollow(){
	int tests = 5;
    while (tests--)
    {
    	for(auto p: prods){
    		for (auto x : p.second)
        	{
		        for (int i = 0; i < (int)x.size(); i++)
		        {
		            string xi = "";
		            xi += x[i];
		            if (terminals.find(x[i]) == terminals.end() && x[i] != lambda)
		            {
		                if (i == (int)x.size() - 1)
		                {
		                     for (auto y : follow[p.first])
		                     {
		                         	string nextfollow = "";
		                         	nextfollow += y;
		                            follow[xi].insert(nextfollow);
		                     }
		                }
		                else
		                {
		                    string nextxi = "";
		                    nextxi += x[i + 1];
		                    if (terminals.find(x[i + 1]) != terminals.end())
		                    {
		                        follow[xi].insert(nextxi);
		                        continue;
		                    }
		                    for (auto y : first[nextxi])
		                    {
		                        string nextfollow = "";
		                        nextfollow += y;
		                        if (nextfollow == "^")
		                        {
		                            for (auto yy : follow[nextxi])
		                            {
		                                string spt = "";
		                                spt += yy;
		                                follow[xi].insert(spt);
		                            }
		                        }
		                        else{
		                        	follow[xi].insert(nextfollow);
		                        }
		                    }
		                }
		            }
		        }
        	}
    	}
    }
}

//compute first
set <string> computeFirst(string nt){
	if(first[nt].size()!=0)
		return first[nt];
		
	for(auto x : prods[nt]){
		if(x.size()==1 && x[0]==lambda){
			first[nt].insert("^");
		} 
		
		else if(terminals.find(x[0])!=terminals.end()){
			string c = "";
			c+=x[0];
			first[nt].insert(c);
		}
		
		else{
			int i=0;
			while(terminals.find(x[i])==terminals.end() && i<x.size()){
				string c="";
				c += x[i];
				set <string> st = computeFirst(c);
				if(st.find("^")!=st.end()){
					for(auto as : st){
						if(as != "^")
							first[nt].insert(as);
					}
					i++;
					continue;
				} else{
					for(auto as : st){
						first[nt].insert(as);
					}
					break;
				}
			}
			if(i==x.size())
				first[nt].insert("^");	
			else if(terminals.find(x[i]) != terminals.end()){
				string c="";
				c+=x[i];
				first[nt].insert(c);
			}
		}
	}
	return first[nt];
}

//taking input, computing first and follow
void inputFirstFollow(string fname)
{
    ifstream fin;
    string line;
    int n_prod;
    int n_nonterm;
    int n_term;
    fin.open(fname);
    
    //get productions
    getline(fin, line);
    n_prod = stoi(line);
    for (int i = 0; i < n_prod; i++)
    {
        getline(fin, line);
        trim(line);
        productionStrings.push_back(line);
    }
    
    //get non-terminals
    getline(fin, line);
    n_nonterm = stoi(line);
    for (int i = 0; i < n_nonterm; i++)
    {
        getline(fin, line);
        char char_array[line.length()];
        strcpy(char_array, line.c_str());
        nonTerminals.insert(char_array[0]);
    }
    
    //get terminals
    getline(fin, line);
    n_term = stoi(line);
    for (int i = 0; i < n_term; i++)
    {
        getline(fin, line);
        char char_array[line.length()];
        strcpy(char_array, line.c_str());
        terminals.insert(char_array[0]);
    }
    
    //get start symbol
    getline(fin, line);
    startSymbol = line;
    
    //inserting $ in follow of start symbol
    follow[startSymbol].insert("$");
    
    //splitting production into left and right
    for (auto prod : productionStrings)
    {
        string left = "";
        string right = "";
        vector <string> rprods;
        int i;
        for (i = 0; prod[i] != '-'; i++)
        {
            left += prod[i];
        }
        i += 2;
        for (; i < (int)prod.size(); i++)
        {
            if (prod[i] == '|')
            {
                rprods.push_back(right);
                right = "";
            }
            else
            {
                right += prod[i];
            }
        }
        rprods.push_back(right);
        prods[left] = rprods;
        for (auto x : rprods)
        {
            for (auto y : x)
            {
                string temp = "";
                temp += y;
                if (y == lambda)
                {
                    lambdaProds.push_back(left);
                }
            }
        }
    }

    //computing first of each terminal
    for (auto x : prods)
    {
        computeFirst(x.first);
    }
    
    //computing follow
    computeFollow();
   	cout << "----------------------------------------------------------" << endl;
    cout << "Grammar is:" << endl;
    for(auto x : productionStrings){
    	cout << x << endl;
    }
    
    cout << "\nNon-terminal\t"
         << " First\t"
         << "\t Follow\t"
         << "\n";
    cout << "----------------------------------------------------------"<<endl;
    for (auto x : prods)
    {
        cout << x.first << "\t\t ";
        for (auto y : first[x.first])
        {
            for (auto z : y)
                cout << z << " ";
        }
        cout << "\t\t ";
        for (auto y : follow[x.first])
            for (auto z : y)
                cout << z << " ";
        cout << "\n";
    }
}

//making parse table
int makeParseTable()
{
    cout << "\n\n";
    for (auto x : prods)
    {
        for (auto y : x.second)
        {
            vector<pair<string, string>> v;
            string sss = "";
            sss += y[0];
            if (y.size() == 1 && y[0] == lambda)
            {
                for (auto x2 : follow[x.first])
                {
                    v.push_back({x.first, x2});
                }
            }
            else if (terminals.find(y[0]) != terminals.end())
            {
                v.push_back({x.first, sss});
            }
            else
            {
            	bool flag=false;
                for (int i = 0; i < (int)y.size(); i++)
                {
                    string s = "";
                    s += y[i];
                    flag = false;
                    for (auto xx : first[s])
                    {
                        if (xx[0] == lambda)
                        {
                            flag = true;
                        }
                        else
                        {
                            v.push_back({x.first, xx});
                        }
                    }
                    if (!flag)
                        break;
                }
                if(flag){
                	for(auto z: follow[x.first]){
                		v.push_back({x.first, z});
                	}
                }
            }
            string lef = "";
            lef += x.first + "->" + y;
            string pp = y;
            for (auto xy : v)
            {
            	validPairs[xy]++;
   		if(validPairs[xy] > 1){
   			cout << "Grammar is not LL(1)" << endl;
   			return 0;
   		}
   		parseTable[xy] = lef;
            }
        }
    }
    cout << "Parse table will be: \n";
    cout << "------------------------\n";
    for(auto x: parseTable){
    		string s = "";
    		s += "{ " + x.first.first + "," + x.first.second + " }";
    		cout << " " << s << "   |   " << x.second << endl;
    	}
    cout << "------------------------\n";
    cout << "\n";
    return 1;
}

//parsing the string
void parseString(string &startSymbol)
{
	cout << "Enter the string to parse"
         << "\n";
    string input;
    cin >> input;
    input += "$";
    
    stack<char> st;
    st.push('$');
    st.push(startSymbol[0]);
    
    cout << "\nStack\t\t"
         << "Input\t\t"
         << "\tActions\n";
    
    //int i = 0;
    string inputCopy = input;
    stack <char> stCopy;
    while(st.top()!='$' || !st.empty()){
    	stCopy = st;
    	string s1="";
    	while(!stCopy.empty()){
    		s1 += stCopy.top();
    		stCopy.pop();
    	}
    	reverse(s1.begin(), s1.end());
    	cout << s1 << "\t\t";
    	cout << inputCopy << "\t\t\t";
    	
    	string s2="";
    	string s3="";
    	s2+= inputCopy[0];
    	s3+= st.top();
    	pair <string, string> x;
    	x = make_pair(s3,s2);
    	
    	if(s2==s3 && s2!="$"){
    		cout << "Pop " << s2 << endl;
    		st.pop();
    		inputCopy = inputCopy.substr(1);
    	}
    	else if(s2==s3 && s2=="$"){
    		cout << "Accepted\n" << endl;
    		return;
    	}
    	else if(parseTable.find(x) != parseTable.end()){
    		string temp = parseTable[x];
    		cout << "Push " << temp << endl;
    		int pos = temp.find(">");
    		temp = temp.substr(pos+1);
    		reverse(temp.begin(), temp.end());
    		
    		st.pop();
    		if(temp!="^"){
				for(auto t : temp){
					st.push(t);
				}
    		}
    	}
    	else {
    		cout << "Rejected\n" << endl;
    		return;
    	}
    }
} 
