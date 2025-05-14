#include <bits/stdc++.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;
#define sim template < class c
#define ris return * this
#define dor > debug & operator <<
#define eni(x) sim > typename \
  enable_if<sizeof dud<c>(0) x 1, debug&>::type operator<<(c i) {
sim > struct rge { c b, e; };
sim > rge<c> range(c i, c j) { return rge<c>{i, j}; }
sim > auto dud(c* x) -> decltype(cerr << *x, 0);
sim > char dud(...);
struct debug {
#ifdef LOCAL
~debug() { cerr << endl; }
eni(!=) cerr << boolalpha << i; ris; }
eni(==) ris << range(begin(i), end(i)); }
sim, class b dor(pair < b, c > d) {
  ris << "(" << d.first << ", " << d.second << ")";
}
sim dor(rge<c> d) {
  *this << "[";
  for (auto it = d.b; it != d.e; ++it)
  *this << ", " + 2 * (it == d.b) << *it;
  ris << "]";
}
#else
sim dor(const c&) { ris; }
#endif
};
#define imie(...) " [" << #__VA_ARGS__ ": " << (__VA_ARGS__) << "] "

struct Lib {
  int id;
  int n, signup_time, per_day;
  vector<int> books, sorted_scores_prefix_sum;
  void read_lib_data(const vector<int>& score_values, int lib_id) {
    id = lib_id;
    scanf("%d%d%d", &n, &signup_time, &per_day);
    books.clear();
    sorted_scores_prefix_sum.clear();
    for(int i = 0; i < n; ++i) {
      int x;
      scanf("%d", &x);
      books.push_back(x);
      if (x >= 0 && x < score_values.size()) {
        sorted_scores_prefix_sum.push_back(score_values[x]);
      } else {
        sorted_scores_prefix_sum.push_back(0); 
      }
    }
    sort(sorted_scores_prefix_sum.rbegin(), sorted_scores_prefix_sum.rend());
    for(int i = 1; i < (int) sorted_scores_prefix_sum.size(); ++i) {
      sorted_scores_prefix_sum[i] += sorted_scores_prefix_sum[i-1];
    }
  }
};

struct FastFlow {
  using T = int;
  struct E {
    int dest;
    T orig, *lim, *rev;
  };
  int zr, uj, n = 0;
  vector<unique_ptr<T>> ts;
  vector<vector<E>> graf;
  vector<int> ptr, odl;
  void reset_flow() {
    n = 0;
    ts.clear();
    graf.clear();
    ptr.clear();
    odl.clear();
  }
  void vert(int v) {
    n = max(n, v + 1);
    graf.resize(n);
    ptr.resize(n);
    odl.resize(n);
  }
  bool iszero(T v) {
    return !v;
  }
  void bfs() {
    fill(odl.begin(), odl.end(), 0);
    vector<int> kol = {zr};
    odl[zr] = 1;
    for (int i = 0; i < (int) kol.size(); i++) {
      for (E& e : graf[kol[i]]) {
        if (!odl[e.dest] and !iszero(*e.lim)) {
          odl[e.dest] = odl[kol[i]] + 1;
          kol.push_back(e.dest);
        }
      }
    }
  }
  T dfs(int v, T lim) {
    if (v == uj) return lim;
    T ret = 0, wez;
    for (int& i = ptr[v]; i < (int) graf[v].size(); i++) {
      E& e = graf[v][i];
      if (odl[e.dest] == odl[v] + 1 and !iszero(*e.lim) and
          !iszero(wez = dfs(e.dest, min(*e.lim, lim)))) {
        ret += wez;
        *e.lim -= wez;
        *e.rev += wez;
        lim -= wez;
        if (iszero(lim)) break;
      }
    }
    return ret;
  }
  void add_edge(int u, int v, T lim, bool bi = false) {
    vert(max(u, v));
    T *a = new T(lim), *b = new T(lim * bi);
    ts.emplace_back(a);
    ts.emplace_back(b);
    graf[u].push_back(E{v, lim,      a, b});
    graf[v].push_back(E{u, lim * bi, b, a});
  }
  T dinic(int zr_, int uj_) {
    zr = zr_; uj = uj_;
    vert(max(zr, uj));
    T ret = 0;
    while (true) {
      bfs();
      fill(ptr.begin(), ptr.end(), 0);
      const T sta = dfs(zr, numeric_limits<T>::max());
      if (iszero(sta)) break;
      ret += sta;
    }
    return ret;
  }
  vector<int> cut() {
    vector<int> ret;
    bfs();
    for (int i = 0; i < n; i++)
      if (odl[i])
        ret.push_back(i);
    return ret;
  }
  map<pair<int, int>, T> get_flowing() {
    map<pair<int, int>, T> ret;
    for (int i = 0; i < n; i++)
      for (E& e : graf[i])
        if (*e.lim < e.orig)
          ret[make_pair(i, e.dest)] += e.orig - *e.lim;
    for (auto& i : ret) {
      const pair<int, int> rev{i.first.second, i.first.first};
      const T x = min(i.second, ret[rev]);
      i.second -= x;
      ret[rev] -= x;
    }
    return ret;
  }
};

struct Assignment {
  long long total_score_value;
  vector<vector<int>> books_chosen_for_lib;
  vector<int> final_lib_order;
};

Assignment solve_optimal_assignment(const vector<Lib> &all_libs, int D_days, const vector<int> &book_scores, const vector<int> &lib_signup_order) {
  Assignment result_assignment;
  result_assignment.total_score_value = 0;
  result_assignment.final_lib_order = lib_signup_order;
  
  FastFlow F;
  F.reset_flow();
  int B_count = book_scores.size();
  int L_count = all_libs.size();
  if (L_count == 0) { 
      return result_assignment;
  }
  int S_node = B_count + L_count + 1;
  int T_node = B_count + L_count + 2;

  vector<int> lib_ready_day(L_count, D_days + 1);
  assert ((int) lib_signup_order.size() <= L_count || lib_signup_order.empty()); 

  int current_day = 0;
  for (int lib_idx : lib_signup_order) {
    if (lib_idx < 0 || lib_idx >= L_count) continue; 
    if (current_day + all_libs[lib_idx].signup_time <= D_days) {
        lib_ready_day[lib_idx] = current_day + all_libs[lib_idx].signup_time;
        current_day += all_libs[lib_idx].signup_time;
    } else {
        lib_ready_day[lib_idx] = D_days + 1; 
        break; 
    }
  }

  for (int i = 0; i < L_count; ++i) {
    if (lib_ready_day[i] <= D_days) {
        long long books_can_scan = (long long)all_libs[i].per_day * (D_days - lib_ready_day[i]);
        if (books_can_scan > 0) {
            F.add_edge(S_node, i, min(books_can_scan, (long long)all_libs[i].books.size()));
            for (int book_idx : all_libs[i].books) {
                 if (book_idx >= 0 && book_idx < B_count) { // Safety check for book_idx
                    F.add_edge(i, L_count + book_idx, 1);
                 }
            }
        }
    }
  }

  vector<pair<int, int>> books_sorted_by_score;
  for (int i = 0; i < B_count; ++i) {
    books_sorted_by_score.push_back({book_scores[i], i});
  }
  sort (books_sorted_by_score.rbegin(), books_sorted_by_score.rend());

  long long current_total_flow = 0; 
  for (int i = 0; i < B_count; ++i) {
    int current_book_idx = books_sorted_by_score[i].second;
    if (current_book_idx < 0 || current_book_idx >= B_count) continue; 
    F.add_edge(L_count + current_book_idx, T_node, 1);
    if (i + 1 == B_count || books_sorted_by_score[i + 1].first != books_sorted_by_score[i].first) {
      debug() << imie(books_sorted_by_score[i].first);
      current_total_flow += F.dinic(S_node, T_node);
    }
  }
  
  auto flows_map = F.get_flowing();
  
  result_assignment.books_chosen_for_lib.resize(L_count);
  for (int i = 0; i < L_count; ++i) {
    if (lib_ready_day[i] <= D_days) {
        for (int book_idx : all_libs[i].books) {
            if (book_idx < 0 || book_idx >= B_count) continue; 
            if (flows_map.count({i, L_count + book_idx}) && flows_map[{i, L_count + book_idx}] > 0) {
                 result_assignment.books_chosen_for_lib[i].push_back(book_idx);
            }
        }
    }
  }

  for (int i = 0; i < B_count; ++i) {
    if (flows_map.count({L_count + i, T_node}) && flows_map[{L_count + i, T_node}] == 1) {
         result_assignment.total_score_value += book_scores[i]; 
    }
  }
  debug() << imie(result_assignment.total_score_value);
  return result_assignment;
}

void process_file(const string& input_filename, const string& output_filename) {

    FILE* inputFile = freopen(input_filename.c_str(), "r", stdin);
    if (inputFile == nullptr) {
        cerr << "Error opening input file: " << input_filename << endl;
        return;
    }
    FILE* outputFile = freopen(output_filename.c_str(), "w", stdout);
    if (outputFile == nullptr) {
        cerr << "Error opening output file: " << output_filename << endl;
        fclose(inputFile); 
        return;
    }
    
    cerr << "Processing file: " << input_filename << endl;

    int B_val, L_val, D_val;
    scanf("%d%d%d", &B_val, &L_val, &D_val);
    vector<int> book_score_values(B_val);
    for(int i = 0; i < B_val; ++i) {
        scanf("%d", &book_score_values[i]);
    }
    vector<Lib> libraries_data(L_val);
    for(int il = 0; il < L_val; ++il) {
        libraries_data[il].read_lib_data(book_score_values, il);
    }

    vector<int> initial_lib_order;
    int num_signed_up_libs_in_input_order = 0;
    
    int scanf_ret = scanf("%d", &num_signed_up_libs_in_input_order);

    if (scanf_ret == 1 && num_signed_up_libs_in_input_order > 0 && num_signed_up_libs_in_input_order <= L_val) { 
      for (int rr = 0; rr < num_signed_up_libs_in_input_order; ++rr) {
            int lib_idx, num_books_for_lib;
            if (scanf("%d %d", &lib_idx, &num_books_for_lib) == 2) {
                int temp_book_idx;
                for(int k=0; k < num_books_for_lib; ++k) {
                    if (scanf("%d", &temp_book_idx) != 1) { 
                        num_signed_up_libs_in_input_order = -1; 
                        break;
                    }
                }
                if (num_signed_up_libs_in_input_order == -1) break; 
                initial_lib_order.push_back(lib_idx);
            } else { 
                num_signed_up_libs_in_input_order = -1; 
                break;
            }
        }
        if (num_signed_up_libs_in_input_order == -1) initial_lib_order.clear();

    } else {
        if (!(scanf_ret == 1 && num_signed_up_libs_in_input_order > 0 && num_signed_up_libs_in_input_order <=L_val)) {
             initial_lib_order.clear();
        }
    }
    
    if (initial_lib_order.empty() && L_val > 0) {
        cerr << "INFO: No valid prior library order read/found for " << input_filename 
             << ". Using default order (0, 1, ..., L-1)." << endl;
        for (int i = 0; i < L_val; ++i) {
            initial_lib_order.push_back(i);
        }
    }
    
    Assignment final_assignment = solve_optimal_assignment(libraries_data, D_val, book_score_values, initial_lib_order);

    int actual_signed_up_count = 0;
    for(int lib_idx : final_assignment.final_lib_order) {
        if (lib_idx < 0 || lib_idx >= L_val) continue;
        if(!final_assignment.books_chosen_for_lib[lib_idx].empty()){
            actual_signed_up_count++;
        }
    }
    printf("%d\n", actual_signed_up_count);

    for (int lib_idx_in_order : final_assignment.final_lib_order) {
        if (lib_idx_in_order < 0 || lib_idx_in_order >= L_val) continue;
        if (!final_assignment.books_chosen_for_lib[lib_idx_in_order].empty()) {
            printf("%d %d\n", lib_idx_in_order, (int) final_assignment.books_chosen_for_lib[lib_idx_in_order].size());
            sort(final_assignment.books_chosen_for_lib[lib_idx_in_order].begin(), final_assignment.books_chosen_for_lib[lib_idx_in_order].end());
            for (int book_idx_val : final_assignment.books_chosen_for_lib[lib_idx_in_order]) {
                printf("%d ", book_idx_val);
            }
            printf("\n");
        }
    }
    cerr << "Score for " << input_filename << ": " << final_assignment.total_score_value << endl;
    fclose(inputFile); 
    fclose(outputFile);
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    const string input_dir_name = "input";
    const string output_dir_name = "output_maxflow_solutions_v2";

    if (!fs::exists(output_dir_name)) {
        if (!fs::create_directories(output_dir_name)) {
            cerr << "Failed to create output directory: " << output_dir_name << endl;
            return 1;
        }
    }

    if (!fs::exists(input_dir_name) || !fs::is_directory(input_dir_name)) {
        cerr << "Input directory '" << input_dir_name << "' not found." << endl;
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(input_dir_name)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            fs::path input_path = entry.path();
            string input_filename_stem = input_path.stem().string();
            string output_filename = "sol_" + input_filename_stem + ".txt";
            fs::path output_path = fs::path(output_dir_name) / output_filename;
            
            process_file(input_path.string(), output_path.string());
        }
    }
    
    return 0;
}