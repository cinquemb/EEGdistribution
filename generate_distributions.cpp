#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <cmath>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <json/json.h>
#include <json/reader.h>

//time ./gdp > gdp.txt 2>&1 &

const int is_test = 1;

const int offset_node = 2;

const int num_leads = 128;

//factor to convert biosemi values into uv
double biosemi_microvoltage_factor = 8192;

//sampling rate
double sampling_rate = 2048;

void string_split_histogram(std::string s, char delimiter, std::map<double, unsigned long int>& h_map){
    size_t start=0;
    int count = 0;
    size_t end=s.find_first_of(delimiter);   
    while (end <= std::string::npos){
        if(offset_node+num_leads == count)
            break;

        if (end == std::string::npos)
            break;

        if( (count > 1) && (count < offset_node+num_leads)){
            double t_val = atof(s.substr(start, end-start).c_str())/biosemi_microvoltage_factor;
            if(h_map.count(t_val) > 0)
                h_map[t_val] += 1;
            else
                h_map[t_val] = 1;
        }
        start=end+1;
        ++count;
        end = s.find_first_of(delimiter, start);
    }
}

void print_vector_string(std::vector<std::string>& v){
    int vector_len = v.size();
  for(int i = vector_len - 1;i >= 0; i--) 
      std::cout << "'" << v[i] << "'" << '\n';// << std::fixed;
    std::cout << '\n';
}

void print_vector(std::vector<double>& v){
	int vector_len = v.size();
	for(int i = vector_len - 1;i >= 0; i--){
        if(i > 0)
            std::cout << v[i] << ',';
        else
            std::cout << v[i] << '\n';
    }
    std::cout << '\n';
}

template <class T1, class T2, class Pred = std::less<T2> >
struct sort_pair_second {
    bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
        Pred p;
        return p(left.second, right.second);
    }
};

std::vector<std::string> get_files_to_mine(std::string files_list){
    std::vector<std::string> data_file_paths;
    std::string line;
    std::ifstream in(files_list.c_str());
    if (!in.is_open()) return data_file_paths;

    while (std::getline(in,line)){
        if(line.size() > 1){
            boost::split(data_file_paths, line, boost::is_any_of(","), boost::token_compress_on);
            break;
        }
    }
    return data_file_paths;
}

Json::Value load_current_histogram(){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data/nfb_histogram.json";
    {
        std::ifstream json_data(filename_data);
        try{
            json_data >> root;
        }catch(...){
             Json::Value root;
        }
    }
    return root;
}

Json::Value load_current_histogram_metadata(){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data/nfb_histogram_metadata.json";
    {
        std::ifstream json_data(filename_data);
        try{
            json_data >> root;
        }catch(...){
             Json::Value root;
        }
    }
    return root;
}

void save_data(std::string file_string, std::string analysis_type, std::vector<double> data_vector){
    int start = 0;
    std::size_t pos = file_string.find(".txt");
    std::string out_file = file_string.substr(0,pos) + analysis_type + ".txt";
    std::cout << out_file << std::endl;
    std::ofstream o(out_file);
    int length_data = data_vector.size();

    o.precision(10);
    for(int k = start; k < length_data; k++){
        if(k < length_data-1)
            o << data_vector[k] << ",";
        else
            o << data_vector[k];
    }
    o.close();
}

void save_data(std::string file_string, Json::Value& data){
    std::ofstream nfb_json;
    nfb_json.open(file_string);
    nfb_json << data;
    nfb_json.close();
}

double calculate_stdv(std::vector<double>& v, double& _mean){
    int stdv_exp = 2;
    double _accum = 0.0;
    std::for_each (v.begin(), v.end(), [&](const double d) {
            _accum += std::pow((d - _mean), stdv_exp);
    });
    double _stdev = std::sqrt(_accum/ ((double)v.size()-1));
    return _stdev;
}

double calculate_stdv_from_histogram_map(std::map<double, int>& h_map, double& _mean){
    int stdv_exp = 2;
    double _accum_val = 0.0;
    double _accum_length = 0.0;
    for(auto iter: h_map){
        _accum_val += std::pow((iter.first - _mean), stdv_exp) * iter.second;
        _accum_length += iter.second;
    }
    double _stdev = std::sqrt(_accum_val/ (_accum_length-1));
    return _stdev;
}

double calculate_skewness_from_histogram_map(std::map<double, int>& h_map, double& _mean, double& _stdev){
    int skew_exp = 3;
    double _accum_val = 0.0;
    double _accum_length = 0.0;
    for(auto iter: h_map){
        _accum_val += std::pow( ((iter.first - _mean)/_stdev), skew_exp) * iter.second;
        _accum_length += iter.second;
    }
    double _skew = _accum_val/_accum_length;
    return _skew;
}

double calculate_kurtosis_from_histogram_map(std::map<double, int>& h_map, double& _mean, double& _stdev){
    int kurt_exp = 4;
    double _accum_val = 0.0;
    double _accum_length = 0.0;
    for(auto iter: h_map){
        _accum_val += std::pow( ((iter.first - _mean)/_stdev), kurt_exp) * iter.second;
        _accum_length += iter.second;
    }
    double _kurt = _accum_val/_accum_length;
    return _kurt;
}

double mean_from_histogram_map(std::map<double, int>& h_map){
    double _accum_val = 0.0;
    double _accum_length = 0.0;
    for(auto iter: h_map){
        _accum_val += iter.first * iter.second;
        _accum_length += iter.second;
    }
    double _accum = _accum_val/_accum_length;
    return _accum;
}


std::vector<double> normalize_data(std::vector<double>& v, double& _mean, double& stdv){
    std::vector<double> rdata;
    for(int i=0;i< rdata.size();++i){
        try{
            double dval = (v[i] - _mean)/stdv;
            rdata.push_back(dval);
        }catch(...){
            std::cout << v[i] << std::endl;
            exit(0);
        }
        
    }
    return rdata;
}

std::map<double, unsigned long int> update_histogram(std::map<double, unsigned long int>& h_map, double& _mean, double& _stdv){
    std::map<double, unsigned long int> o_map;
    for(auto iter: h_map){
        double normed_val = ((iter.first - _mean)/_stdv);
        if(o_map.count(normed_val) > 0)
            o_map[normed_val] += iter.second;
        else
            o_map[normed_val] = iter.second;
    }
    return o_map;
}

std::map<double, int> mine_file(std::string& f_name){
    std::vector<unsigned long int> new_line_indexes;
    std::vector<std::string> data_vector_string;
    std::map<double, unsigned long int> raw_nfb_histogram_map;

    char *filename = (char *)f_name.c_str();
    struct stat stbuff;
    stat(filename, &stbuff);
    unsigned long int max = stbuff.st_size;

    unsigned long int i = 0;
    int count = 0;
    unsigned long int buffSize = 3000000;
    char c[buffSize];
    size_t val;

    FILE *dataFile = fopen(filename, "r");
    std::string row_string = "";

    while(1){
        unsigned long int real_i = i*buffSize;
        fseek(dataFile, real_i, SEEK_SET);
        val = fread(&c, 1, buffSize,dataFile);
        

        for(unsigned long int k=0;k<buffSize;k++){
            unsigned long int mod_i = k + real_i;

            if(mod_i == max)
                goto cleanAndSave;

            if(c[k] == '\n'){
                count++;
                data_vector_string.clear();
                string_split_histogram(row_string, ' ', raw_nfb_histogram_map);
                row_string.clear();
            }else{
                row_string += c[k];
            }
        }
        i++;
    }

    cleanAndSave:
        fclose(dataFile);

    return raw_nfb_histogram_map;
}

void build_distribution_json_data(std::string& file, Json::Value& nfb_histogram, Json::Value& nfb_histogram_metadata){
    std::cout << "Starting Mining: " << file << "\n\n";
    std::map<double, int> raw_nfb_histogram_map = mine_file(file);
    std::cout << "Calulating Mean " << "\n";
    double data_mean = mean_from_histogram_map(raw_nfb_histogram_map);
    std::cout << "Calulating STDV " << "\n";
    double data_stdv = calculate_stdv_from_histogram_map(raw_nfb_histogram_map, data_mean);
    std::cout << "Calulating Skew " << "\n";
    double data_skew = calculate_skewness_from_histogram_map(raw_nfb_histogram_map, data_mean, data_stdv);
    std::cout << "Calulating Kurtosis " << "\n";
    double data_kurt = calculate_kurtosis_from_histogram_map(raw_nfb_histogram_map, data_mean, data_stdv);
    
    std::cout << "Normalizing Data  & Update Histogram " << "\n";
    std::map<double, int> normed_hist_map = update_histogram(raw_nfb_histogram_map, data_mean, data_stdv);
    
    std::cout.precision(10);

    /* loop over twice to save on memmory alloc */
    std::cout << "Saving Data "<<'\n';
    Json::Value nfb_histogram_file;
    for(auto iter: normed_hist_map)
        nfb_histogram_file[std::to_string(iter.first)] = iter.second;
    std::vector<std::string> file_strings;
    boost::split(file_strings,file,boost::is_any_of("/"));
    save_data("nfb_histogram_data/nfb_histogram_" + file_strings[file_strings.size()-1] + ".json", nfb_histogram_file);
    nfb_histogram_file.clear();

    for(auto iter: normed_hist_map){
        if(nfb_histogram.get(std::to_string(iter.first),false) == false)
            nfb_histogram[std::to_string(iter.first)] = iter.second;
        else
            nfb_histogram[std::to_string(iter.first)] = nfb_histogram[std::to_string(iter.first)].asInt() + iter.second;
    }
    normed_hist_map.clear();
    save_data("nfb_histogram_data/nfb_histogram.json", nfb_histogram);

    nfb_histogram_metadata[file]["mean"] = data_mean;
    nfb_histogram_metadata[file]["stdv"] = data_stdv; 
    nfb_histogram_metadata[file]["skew"] = data_skew; 
    nfb_histogram_metadata[file]["kurt"] = data_kurt; 
    save_data("nfb_histogram_data/nfb_histogram_metadata.json", nfb_histogram_metadata);

    std::cout << "Done: " << file << "\n\n";
}

int main(int argc, char *argv[]) {
    std::vector<std::string> files;
    Json::Value nfb_histogram = load_current_histogram();
    Json::Value nfb_histogram_metadata = load_current_histogram_metadata();
    
    std::string mined_file_list = "nfb-mined_file_list.txt";
    files = get_files_to_mine(mined_file_list);
    std::cout << "Files to mine: " << files.size() << "\n\n";
    print_vector_string(files);

    if(is_test > 0)
        std::cout << "~*~*~*~*~**~*~*~**~*~*~*~**~ Testing Mode ~*~*~*~*~**~*~*~**~*~*~*~**~" << "\n\n";

    for(int i=0;i<files.size();i++){
        build_distribution_json_data(files[i], nfb_histogram, nfb_histogram_metadata);
    }

	return 0;
}