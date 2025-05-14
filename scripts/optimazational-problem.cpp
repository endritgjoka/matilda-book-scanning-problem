#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const int test = 5;

const string tests[] = {
    "a_example",
    "b_read_on",
    "c_incunabula",
    "d_tough_choices",
    "e_so_many_books",
    "f_libraries_of_the_world"
};

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

void swap_libs(Library& lib1, Library& lib2)
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

const int MAX_B = 10000000;
const int MAX_L = 10000000;

int b, l, d;
int score[MAX_B];
int freq[MAX_B];
Library lib[MAX_L];
int signedUp;
int lastSignedUp;
int totalScore;
string outputStr;

void perform_input(ifstream& current_inFile)
{
    current_inFile >> b >> l >> d;
    for (int i = 0; i < b; ++i)
    {
        current_inFile >> score[i];
    }
    for (int i = 0; i < l; ++i)
    {
        lib[i].libId = i;
        current_inFile >> lib[i].n >> lib[i].t >> lib[i].m;
        for (int j = 0; j < lib[i].n; ++j)
        {
            int id;
            current_inFile >> id;
            if (id >= 0 && id < MAX_B) {
                 ++freq[id];
            }
            lib[i].ids.push_back(id);
        }
    }
}

void perform_output(ofstream& current_outFile)
{
    current_outFile << outputStr;
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

bool isFree[MAX_B];

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

int bestScore = -1;

void solveIter(bool toSort)
{
    for (int i = 0; i < b; ++i)
    {
        isFree[i] = true;
    }
    if (toSort) sort(lib, lib + l, cmpLibScore);
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
        sort(curr.ids.begin(), curr.ids.end(), cmpScore);
        curr.taken.resize(0);
        curr.takenScore = 0;
        curr.canTake = (d - currDay) * curr.m;
        if (curr.canTake > curr.ids.size()) curr.canTake = curr.ids.size();
        
        for (int i = 0; i < curr.canTake; ++i)
        {
            if (i >= curr.ids.size()) break;
            int book_id_to_check = curr.ids[i];
            int currentBookScoreValue = getScore(book_id_to_check);
            
            if (isFree[book_id_to_check]) {
                int actual_score_value = score[book_id_to_check];
                isFree[book_id_to_check] = false;
                if (actual_score_value > 0) {
                    totalScore += actual_score_value;
                    curr.takenScore += actual_score_value;
                    curr.taken.push_back(book_id_to_check);
                }
            }
        }

        if (curr.taken.empty())
        {
            currDay -= curr.t;
        }
        else signedUp++;
    }
}

void solveIterSmart(bool toSort)
{
    for (int i = 0; i < b; ++i)
    {
        isFree[i] = true;
    }
    if (toSort) sort(lib, lib + l, cmpLibScore);
    int currDay = 0;
    lastSignedUp = 0;
    signedUp = 0;
    totalScore = 0;
    
    for(int i=0; i<l; ++i) {
        lib[i].taken.clear();
        lib[i].takenScore = 0;
        lib[i].toTake = 0;
    }

    while (lastSignedUp < l && currDay < d)
    {
        Library& curr = lib[lastSignedUp++];
        currDay += curr.t;
        if (currDay > d)
        {
            --lastSignedUp;
            break;
        }
        ++signedUp;
        sort(curr.ids.begin(), curr.ids.end(), cmpScore);
        curr.toTake = 0;
        curr.canTake = (d - currDay) * curr.m;
        if (curr.canTake > curr.ids.size()) curr.canTake = curr.ids.size();
    }

    bool took = true;
    while (took)
    {
        took = false;
        for (int i = lastSignedUp - 1; i >= 0; --i)
        {
            Library& curr = lib[i];
            if (!curr.canTake) continue;

            while (curr.toTake < curr.ids.size() && 
                   (curr.ids[curr.toTake] < 0 || curr.ids[curr.toTake] >= MAX_B || !isFree[curr.ids[curr.toTake]])) {
                ++curr.toTake;
            }

            if (curr.toTake < curr.ids.size())
            {
                int book_id = curr.ids[curr.toTake];
                if (book_id >=0 && book_id < MAX_B) {
                    took = true;
                    isFree[book_id] = false;
                    totalScore += score[book_id];
                    curr.takenScore += score[book_id];
                    curr.taken.push_back(book_id);
                    --curr.canTake;
                } else {
                    curr.toTake++; 
                }
            }
            else curr.canTake = 0;
        }
    }

    int actualSignedUp = 0;
    for (int i = 0; i < lastSignedUp; ++i)
    {
        if (!lib[i].taken.empty()) {
            actualSignedUp++;
        }
    }
    signedUp = actualSignedUp;
}


double customScore_eval(int id)
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
        curr.libScore += customScore_eval(curr.ids[j]);
    }

    if (test == 5)
    {
        if (curr.t == 0) curr.libScore = (curr.libScore > 0) ? 1e18 : 0;
        else curr.libScore /= pow(curr.t, 1);
        curr.libScore *= (rand() % 101) / 100.0 * 0.4 + 0.85;
    }
    else if (test == 4)
    {
        if (curr.t == 0) curr.libScore = (curr.libScore > 0) ? 1e18 : 0;
        else curr.libScore /= pow(curr.t, 1.75);
        if (curr.m != 0) curr.libScore *= pow(curr.m, 1);
        else if (curr.libScore > 0) curr.libScore = 0;
        curr.libScore *= (rand() % 101) / 100.0 * 0.3 + 0.85;
    }
    else
    {
        if (curr.t == 0) curr.libScore = (curr.libScore > 0) ? 1e18 : 0;
        else curr.libScore /= curr.t;
    }
}

void customSort()
{
    for (int i = 0; i < b; ++i)
    {
        if (i < MAX_B) freq[i] = 1;
        else break; 
    }

    int currDay = 0;
    for (int i = 0; i < std::min(l, b) && currDay < d + 10; ++i)
    {
        double maxS = -1.0;
        int maxJ = -1;
        for (int j = i; j < l; ++j)
        {
            evalLib(lib[j], d - currDay);
            if (lib[j].libScore > maxS)
            {
                maxS = lib[j].libScore;
                maxJ = j;
            }
        }
        
        if (maxJ != -1) {
            swap_libs(lib[i], lib[maxJ]);
            for (int id_val : lib[i].ids)
            {
                if (id_val >=0 && id_val < MAX_B) ++freq[id_val];
            }
            currDay += lib[i].t;
        } else {
            break;
        }
    }
}

const int REPS = 10;

void solve()
{
    int reps = REPS;
    customSort();
    bool first = true;
    while (reps-- > 0)
    {
        solveIterSmart(!first);
        if (totalScore > bestScore)
        {
            cerr << "File: [CurrentFile] - New best score: " << totalScore << ", Libraries: " << signedUp << endl;
            bestScore = totalScore;
            generateOutput();
            reps = REPS;
        }
        for (int k = 0; k < lastSignedUp; ++k)
        {
            lib[k].libScore = static_cast<double>(lib[k].takenScore);
        }
        first = false;
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

    const string input_dir_path_str = "input";
    const string output_dir_path_str = "output-optimizational-problem";
    fs::path output_dir(output_dir_path_str);

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
        cerr << "Error with output directory '" << output_dir_path_str << "': " << e.what() << endl;
        return 1;
    }
    
    fs::path input_dir(input_dir_path_str);
    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        cerr << "Error: Input directory '" << input_dir_path_str << "' not found or is not a directory." << endl;
        cerr << "Please create an 'input' directory in the same location as the executable and place your .txt files there." << endl;
        return 1;
    }

    cout << "Processing files from: " << fs::absolute(input_dir) << endl;
    cout << "Outputting solutions to: " << fs::absolute(output_dir) << endl;


    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file()) {
            fs::path input_file_path = entry.path();
            if (input_file_path.extension() == ".txt") {
                string current_filename_str = input_file_path.filename().string();
                cout << "\nProcessing file: " << current_filename_str << " ..." << endl;

                string output_filename_stem = input_file_path.stem().string();
                fs::path output_file_path = output_dir / ("sol_" + output_filename_stem + ".txt");

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
                
                for (int k = 0; k < MAX_L; ++k) {
                    lib[k].ids.clear();
                    lib[k].taken.clear();
                    lib[k].takenScore = 0;
                    lib[k].libScore = 0.0;
                    lib[k].canTake = 0;
                    lib[k].toTake = 0;
                }
                std::fill(freq, freq + MAX_B, 0);


                perform_input(current_inFile);
                
                cerr << "Starting solve for: " << current_filename_str << endl;

                bigSolve();
                perform_output(current_outFile);

                current_inFile.close();
                current_outFile.close();

                cout << "Finished processing " << current_filename_str << ". Output: " << output_file_path << endl;
                 if (bestScore != -1) {
                    cout << "Best score for " << current_filename_str << ": " << bestScore << endl;
                }
            }
        }
    }

    cout << "\nAll .txt files processed." << endl;
    return 0;
}