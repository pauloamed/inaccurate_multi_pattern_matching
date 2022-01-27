#include<bits/stdc++.h>
using namespace std;

vector<int> Z(string &s){
  int n=s.length();
  vector<int> z(n);
  for (int i=1,l=0,r=0;i<n;i++){
    if(i<r){
      z[i]=min(r-i,z[i-l]);
    }
    while(i+z[i]<n && s[i+z[i]]==s[z[i]]){
      z[i]++;
    }
    if ((i+z[i])>=r){
      l=i;
      r=i+z[i];
    }
  }
  return z;
}

set<pair<int,int>> ans;
void solve(int id, string &pat, string &txt){
  string ab = pat + "#" + txt;
  string ba;
  {
    string ra = pat, rb = txt;
    reverse(ra.begin(), ra.end());
    reverse(rb.begin(), rb.end());
    ba = ra + "#" + rb;
  }

  auto z_pref = Z(ab);
  auto z_suf = Z(ba);

  int pat_size = pat.size();
  int txt_size = txt.size();

	for(int i = pat_size + 1; i < ab.size(); i++){
    if(i + pat_size - 1 == ab.size()) break;

		int start_pref = i;
    int sz_pref = z_pref[start_pref];
    
    int real_start_suf = i + pat_size - 1;
    int start_suf = real_start_suf - (pat_size + 1); 
    start_suf = txt_size - 1 - start_suf;
    start_suf += (pat_size + 1);

    int sz_suf = z_suf[start_suf];
  
		if(sz_pref + sz_suf + 1 >= pat_size) ans.insert({i - (pat_size + 1), id});
	}
}

int main(){
  string s; cin >> s;
  int n; cin >> n;
  for(int i = 0; i < n; ++i){
    string t; cin >> t;
    solve(i, t, s);
  }

  for(auto [a, b] : ans){
    cout << a << " " << b << "\n";
  }
  // cout << pos_matching(s, v).size() << endl;
}