#include<bits/stdc++.h>
using namespace std;

#define deb(...) logger(#__VA_ARGS__, __VA_ARGS__)
template<typename ...Args>
void logger(string vars, Args&&... values) {
    cout << vars << " = ";
    string delim = "";
    (..., (cout << delim << values, delim = ", "));
    cout<<endl;
}

template<int N>
struct AhoCorasick{
  map<int,int> to[N];
  int sl[N];
  int next_node = 1;
  const int ROOT = 0;
  vector<int> sl_tree[N]; 
  int curr_time = 0;
  int in_time[N], out_time[N], time2node[N], sz[N];
  
  void precalc(int x){
    time2node[curr_time] = x; in_time[x] = curr_time++;
    sz[x] = 1;
    for(auto y : sl_tree[x]){
      precalc(y);
      sz[x] += sz[y];
    }
    out_time[x] = curr_time;
  }
  
  bool is_in_subtree_of(int x, int y){
    return in_time[x] >= in_time[y] && out_time[x] <= out_time[y];
  }

  vector<int> add(const string &s){
    vector<int> vis_nodes;
    int curr = ROOT;
    for(auto c : s){
      vis_nodes.push_back(curr);
      if(to[curr].count(c) == 0) curr = to[curr][c] = next_node++;
      else curr = to[curr][c];
    }
    vis_nodes.push_back(curr);
    return vis_nodes;
  }

  void compute_suffix_links(){ // similar to calculation of preffix function
    const int P = -1; // placeholder 
    sl[ROOT] = P;
    queue<int> q; q.push(ROOT);
    while(q.size()){
      int x = q.front(); q.pop();
      for(auto [c, child] : to[x]){
        int child_sl = sl[x]; // trying to match a prefsufix, starts with parents suffixlink
        while(child_sl != P){
          if(to[child_sl].count(c)) break;
          else child_sl = sl[child_sl];
        }
        if(child_sl == P) sl[child] = ROOT; // couldnt match a prefsufix+c
        else sl[child] = to[child_sl][c]; // found match (prefsufix+c)
        
        q.push(child);
        sl_tree[sl[child]].push_back(child);
      }
    }
    sl[ROOT] = ROOT;
    precalc(ROOT);
  }


  vector<int> node_at_index(string &t){
    vector<int> ret(t.size());
    int curr = ROOT;
    for(int i = 0; i < t.size(); ++i){ char c = t[i];
      while(true){
        if(to[curr].count(c)){ curr = to[curr][c]; break; }
        else if(curr == ROOT) break;
        else curr = sl[curr];
      }
      ret[i] = curr;
    }
    return ret;
  }

  void print(int x){
    cout << x << " ";
    for(auto [c, y] : to[x]){
      cout << "( " << (char)c << " ";
      print(y);
      cout << " )";
    }
  }

};

const int MAXN = 4000100;
AhoCorasick<MAXN> aho_prefix, aho_suffix;
// these edges need to have ids
map<int, vector<pair<int,int>>> acc_edges, query_edges;

set<pair<int,int>> active_query_nodes_on_suffix;

void add_query_node(int x, int query_id){
  active_query_nodes_on_suffix.insert({aho_suffix.in_time[x], query_id});
}

void rmv_query_node(int x, int query_id){
  active_query_nodes_on_suffix.erase({aho_suffix.in_time[x], query_id});
}

vector<pair<int,int>> acc_edges_info;
vector<int> query_edges_info;
set<pair<int,int>> ans;

void add_ans(int acc_id, int query_id){
  auto [pat_id, pos_in_pat] = acc_edges_info[acc_id];
  int pos_query = query_edges_info[query_id];
  ans.insert({pos_query - pos_in_pat, pat_id});
}

void sack_solve(int x, bool keep){ // maybe worry about parent
  if(aho_prefix.sl_tree[x].size() > 0){
    int heavy_child = aho_prefix.sl_tree[x].front();
    for(auto y : aho_prefix.sl_tree[x]){
      if(aho_prefix.sz[y] > aho_prefix.sz[heavy_child]) heavy_child = y;
    }
 
    // vising light and then heavy childs of x
    for(auto y : aho_prefix.sl_tree[x]){
      if(y != heavy_child) sack_solve(y, false);
    }
    sack_solve(heavy_child, true);
 
    for(auto y : aho_prefix.sl_tree[x]){
      if(y == heavy_child) continue;
      for(int t = aho_prefix.in_time[y]; t < aho_prefix.out_time[y]; ++t){
        int z = aho_prefix.time2node[t];
        // add z
        for(auto [node, query_id] : query_edges[z]){
          add_query_node(node, query_id);
        }
      }
    }
  }
 
  for(auto [node, query_id] : query_edges[x]){
    add_query_node(node, query_id);
  }
 
  // solve queries
  for(auto [acc_node, acc_id] : acc_edges[x]){ // tot amortized: sum(string_sizes)
    // acc_node is in suffix trie
    // active_query_nodes_on_suffix are nodes in suffix trie
    // we want to know if there is x in active_query_nodes_on_suffix s.t. x is in the subtree of acc_node
    int acc_node_in = aho_suffix.in_time[acc_node];
    int acc_node_out = aho_suffix.out_time[acc_node];

    auto it_in = active_query_nodes_on_suffix.lower_bound({acc_node_in, -1});
    auto it_out = active_query_nodes_on_suffix.upper_bound({acc_node_out, -1});

    for(auto it = it_in; it != it_out; ++it){
      add_ans(acc_id, it->second);
      // if dont delete after adding, its quadratic, because the problem nature is so
    }

  }
 
  if(!keep){
    for(int t = aho_prefix.in_time[x]; t < aho_prefix.out_time[x]; ++t){
      int z = aho_prefix.time2node[t];
      for(auto [node, query_id] : query_edges[z]){
        rmv_query_node(node, query_id);
      }
    }
  }
}

void pos_matching(string &s, vector<string> &patterns){
  for(int pat_id = 0; pat_id < patterns.size(); pat_id++){
    string &t = patterns[pat_id];
    auto pos_a = aho_prefix.add(t);

    auto rt = t;
    reverse(rt.begin(), rt.end()); 
    auto pos_b = aho_suffix.add(rt);

    // for(auto x : pos_a) cout << x << " "; cout << endl;
    // for(auto x : pos_b) cout << x << " "; cout << endl;

    // aho_prefix.print(0);cout << endl;
    // aho_suffix.print(0);cout << endl;

    int m = t.size();
    for(int i = 0; i < m; ++i){
      int j = m - i - 1; if(j < 0) break;

      // cout << pos_a[i] << " " << pos_b[j] << endl;

      acc_edges[pos_a[i]].push_back({pos_b[j], acc_edges_info.size()});
      acc_edges_info.push_back({pat_id, i}); // i-th letter of pat_id pattern
    }
  }

  aho_prefix.compute_suffix_links();
  aho_suffix.compute_suffix_links();


  s = "#" + s + "#";  
  string rs = s;  
  reverse(rs.begin(), rs.end());

  auto preffix_trie = aho_prefix.node_at_index(s);
  auto suffix_trie = aho_suffix.node_at_index(rs);

  int n = s.size();

  reverse(suffix_trie.begin(), suffix_trie.end());

  // for(int i = 0; i < n; ++i){
  //   cout << preffix_trie[i] << " " << suffix_trie[i] << endl;
  // }

  for(int i = 0; i + 2 < n; ++i){
    query_edges[preffix_trie[i]].push_back({suffix_trie[i + 2], query_edges_info.size()});
    // deb(preffix_trie[i], suffix_trie[i + 2]);
    query_edges_info.push_back(i);
  }

  sack_solve(aho_prefix.ROOT, false);
}

int main(){
  string s; cin >> s;
  int n; cin >> n;
  vector<string> v(n);
  for(auto &x : v) cin >> x;

  pos_matching(s, v);
  for(auto [a, b] : ans){
    cout << a << " " << b << "\n";
  }
}