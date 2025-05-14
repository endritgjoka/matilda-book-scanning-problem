#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cmath>  
#include <string> 
#include <filesystem> 
#include <limits> 

namespace fs = std::filesystem;

using namespace std;


struct Library
{
    int libId;
    int n, t, m;
    vector<int> ids;
    double libScore;
    int canTake;
    int toTake;
    vector<int> taken;
    int takenScore = 0;
};

void swap(Library& lib1, Library& lib2)
{
    std::swap(lib1.libId, lib2.libId);
    std::swap(lib1.n, lib2.n);
    std::swap(lib1.t, lib2.t);
    std::swap(lib1.m, lib2.m);
    std::swap(lib1.ids, lib2.ids);
    std::swap(lib1.libScore, lib2.libScore);
    std::swap(lib1.canTake, lib2.canTake);
    std::swap(lib1.toTake, lib2.toTake);
    std::swap(lib1.taken, lib2.taken);
    std::swap(lib1.takenScore, lib2.takenScore);
}

const int MAX_B = 100000; 
const int MAX_L = 100000;

int b, l, d;
int score[MAX_B];
int freq[MAX_B];     
Library lib[MAX_L];  
int signedUp; 
int lastSignedUp;
int totalScore;
string outputStr;

bool isFree[MAX_B]; 

int bestScore = -1;  

void input(ifstream& inFile)
{
    inFile >> b >> l >> d;

    for (int i = 0; i < b; ++i)
    {
        inFile >> score[i];
    }

    for (int i = 0; i < l; ++i)
    {
        lib[i].libId = i;
        inFile >> lib[i].n >> lib[i].t >> lib[i].m;

        lib[i].ids.clear();
        lib[i].libScore = 0.0;
        lib[i].canTake = 0; 
        lib[i].toTake = 0;  
        lib[i].taken.clear();
        lib[i].takenScore = 0;

        for (int j = 0; j < lib[i].n; ++j)
        {
            int id;
            inFile >> id;
            if (id >= 0 && id < MAX_B) {
                 ++freq[id];
            }
            lib[i].ids.push_back(id);
        }
    }
}

void output(ofstream& outFile)
{
    outFile << outputStr;
}

void generateOutput()
{
    ostringstream ss;
    ss << signedUp << "\n";
    for (int i = 0; i < lastSignedUp; ++i)
    {
        if (lib[i].taken.empty()) continue;

        ss << lib[i].libId << " " << lib[i].taken.size() << "\n";
        for (int j = 0; j < lib[i].taken.size(); ++j)
        {
            if (j > 0) ss << " ";
            ss << lib[i].taken[j];
        }
        ss << "\n";
    }
    outputStr = ss.str();
}

int getScore(int id)
{
    if (id < 0 || id >= b) return 0; 
    return isFree[id] * score[id];
}

bool cmpScore(int id1, int id2)
{
    return getScore(id1) > getScore(id2);
}

bool cmpLibScore(const Library& lib1, const Library& lib2)
{
    return lib1.libScore > lib2.libScore;
}


void solveIter(bool toSort)
{
    for (int i = 0; i < b; ++i)
    {
        isFree[i] = true;
    }
    if (toSort) std::sort(lib, lib + l, cmpLibScore);
    
    int currDay = 0;
    lastSignedUp = 0; 
    signedUp = 0;     
    totalScore = 0;   

    while (lastSignedUp < l && currDay < d) 
    {
        Library& curr = lib[lastSignedUp++]; 
        currDay += curr.t;
        if (currDay > d)
        {
            --lastSignedUp;
            break;
        }
        
        std::sort(curr.ids.begin(), curr.ids.end(), cmpScore);
        curr.taken.clear(); 
        curr.takenScore = 0;
        curr.canTake = (d - currDay) * curr.m; 
        
        if (curr.canTake > curr.ids.size()) curr.canTake = curr.ids.size();

        for (int book_idx = 0; book_idx < curr.ids.size() && curr.taken.size() < curr.canTake ; ++book_idx)
        {
            int current_book_id = curr.ids[book_idx];
            if (isFree[current_book_id]) 
            {
                int currentBookScoreValue = score[current_book_id]; 
                if (currentBookScoreValue > 0) {
                    isFree[current_book_id] = false;
                    totalScore += currentBookScoreValue;
                    curr.takenScore += currentBookScoreValue;
                    curr.taken.push_back(current_book_id);
                }
            }
        }

        if (curr.taken.empty())
        {
            currDay -= curr.t; 
        }
        else {
            signedUp++; 
        }
    }
}

void solveIterSmart(bool toSort)
{
    for (int i = 0; i < b; ++i)
    {
        isFree[i] = true;
    }
    if (toSort) std::sort(lib, lib + l, cmpLibScore);
    
    int currDay = 0;
    lastSignedUp = 0; 
    totalScore = 0;   
    for (int i = 0; i < l; ++i) { 
        Library& current_lib_to_signup = lib[lastSignedUp]; 

        int time_after_this_signup = currDay + current_lib_to_signup.t;
        if (time_after_this_signup > d) {
            break; 
        }
        currDay = time_after_this_signup; 
        std::sort(current_lib_to_signup.ids.begin(), current_lib_to_signup.ids.end(), cmpScore);
        current_lib_to_signup.taken.clear();
        current_lib_to_signup.takenScore = 0;
        current_lib_to_signup.toTake = 0;
        current_lib_to_signup.canTake = (d - currDay) * current_lib_to_signup.m;
        if (current_lib_to_signup.canTake > current_lib_to_signup.ids.size()) {
             current_lib_to_signup.canTake = current_lib_to_signup.ids.size();
        }
        lastSignedUp++;
    }

    bool took_a_book_this_round = true;
    while (took_a_book_this_round)
    {
        took_a_book_this_round = false;
        for (int i = 0; i < lastSignedUp; ++i)
        {
            Library& current_scanning_lib = lib[i];
            if (current_scanning_lib.canTake == 0) continue;

            while (current_scanning_lib.toTake < current_scanning_lib.ids.size() &&
                   (current_scanning_lib.ids[current_scanning_lib.toTake] < 0 || 
                    current_scanning_lib.ids[current_scanning_lib.toTake] >= b || 
                   !isFree[current_scanning_lib.ids[current_scanning_lib.toTake]]))
            {
                current_scanning_lib.toTake++;
            }

            if (current_scanning_lib.toTake < current_scanning_lib.ids.size()) 
            {
                took_a_book_this_round = true;
                int book_id = current_scanning_lib.ids[current_scanning_lib.toTake];
                
                isFree[book_id] = false;
                totalScore += score[book_id];
                current_scanning_lib.takenScore += score[book_id];
                current_scanning_lib.taken.push_back(book_id);
                
                current_scanning_lib.canTake--; 
                current_scanning_lib.toTake++; 
            }
            else
            {
                current_scanning_lib.canTake = 0; 
            }
        }
    }
    
    signedUp = 0;
    for (int i = 0; i < lastSignedUp; ++i)
    {
        if (!lib[i].taken.empty()) {
            signedUp++;
        }
    }
}


double customScore(int id)
{
    if (id < 0 || id >= b) return 0.0;
    if (freq[id] == 0) return static_cast<double>(score[id]);
    return score[id] * 1.0 / freq[id];
}

void evalLib(Library& curr, int days_remaining)
{
    curr.libScore = 0;
    for (size_t j = 0; j < curr.ids.size(); ++j)
    {
        curr.libScore += customScore(curr.ids[j]);
    }

    if (curr.t == 0) { 
        curr.libScore = (curr.libScore > 0) ? std::numeric_limits<double>::max() : 0.0;
    } else {
        curr.libScore /= curr.t;
    }
}

void customSort()
{
    for (int i = 0; i < b; ++i)
    {
        freq[i] = 1; 
    }

    int current_day_sim = 0; 
    for (int i = 0; i < l && current_day_sim < d + 10; ++i) 
    {
        double max_eval_score = -1.0;
        int best_lib_idx = -1;

        for (int j = i; j < l; ++j) 
        {
            evalLib(lib[j], d - current_day_sim); 
            if (lib[j].libScore > max_eval_score)
            {
                max_eval_score = lib[j].libScore;
                best_lib_idx = j;
            }
        }

        if (best_lib_idx != -1) { 
            swap(lib[i], lib[best_lib_idx]); 
            
            for (int book_id : lib[i].ids)
            {
                if (book_id >= 0 && book_id < b) { 
                     ++freq[book_id]; 
                }
            }
            current_day_sim += lib[i].t; 
        } else {
            break; 
        }
    }
}

const int REPS = 10;

void solve() 
{
    customSort(); 

    bool first_iter_in_solve = true;
    int reps_countdown = REPS;

    while (reps_countdown-- > 0)
    {
        solveIterSmart(!first_iter_in_solve); 

        if (totalScore > bestScore)
        {
            cerr << "New best score: " << totalScore << ", Libraries: " << signedUp << endl;
            bestScore = totalScore;
            generateOutput(); 
            reps_countdown = REPS; 
        }

        for (int k = 0; k < lastSignedUp; ++k) 
        {
            lib[k].libScore = lib[k].takenScore; 
            lib[k].takenScore = 0; 
        }
        first_iter_in_solve = false;
    }
}

void bigSolve()
{
    for (int i = 0; i < 50; ++i) 
    {
        solve(); 
    }
}

int main()
{
    srand(-5);

    const std::string input_folder_path = "input"; 
    const std::string output_folder_name = "output-optimizational-problem";
    fs::path output_dir(output_folder_name);

    try {
        if (!fs::exists(output_dir)) {
            if (fs::create_directories(output_dir)) {
                cout << "Created output directory: " << fs::absolute(output_dir) << endl;
            } else {
                cerr << "Failed to create output directory: " << fs::absolute(output_dir) << endl;
                return 1;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error with output directory '" << output_folder_name << "': " << e.what() << endl;
        return 1;
    }

    if (!fs::exists(input_folder_path) || !fs::is_directory(input_folder_path)) {
        cerr << "Error: Input directory '" << input_folder_path << "' not found or is not a directory." << endl;
        cerr << "Please create an 'input' directory (e.g., in the same location as the executable) and place your .txt files there." << endl;
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(input_folder_path)) {
        if (entry.is_regular_file()) {
            fs::path input_file_path = entry.path();
            std::string filename_str = input_file_path.filename().string();
            
            if (input_file_path.extension() != ".txt") {
                continue;
            }

            cout << "Processing file: " << filename_str << " ..." << endl;

            fs::path output_file_path = output_dir / ("sol_" + filename_str);

            ifstream current_inFile(input_file_path);
            if (!current_inFile.is_open()) {
                cerr << "Error opening input file: " << input_file_path << endl;
                continue; 
            }

            ofstream current_outFile(output_file_path);
            if (!current_outFile.is_open()) {
                cerr << "Error opening output file: " << output_file_path << endl;
                current_inFile.close();
                continue;
            }

            bestScore = -1; 
            outputStr.clear(); 
            std::fill(freq, freq + MAX_B, 0); 

            input(current_inFile); 
            bigSolve();            
            output(current_outFile); 

            current_inFile.close();
            current_outFile.close();

            cout << "Finished processing " << filename_str << ". Output: " << output_file_path << endl;
        }
    }

    cout << "All files processed." << endl;
    return 0;
}