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

const int offset_node = 2;

const int num_leads = 128;

//factor to convert biosemi values into uv
double biosemi_microvoltage_factor = 8192;

//sampling rate
double sampling_rate = 2048;

//bin size
int bin_size = 1000;

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

void save_data(std::string file_string, std::string analysis_type, std::vector<double> data_vector){
    int start = 0;

    std::vector<std::string> file_strings;
    boost::split(file_strings,file_string,boost::is_any_of("/"));
    std::string f_name = file_strings[file_strings.size()-1];

    std::size_t pos = f_name.find(".txt");
    std::string out_file = "nfb_running_moments/" + f_name.substr(0,pos) + analysis_type + ".txt";
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

Json::Value load_unnormed_histogram_metadata(){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data/nfb_unnormed_histogram_metadata.json";
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

void string_split_histogram(std::string s, char delimiter, std::map<int, std::map<std::string, long double>>& col_sum_map, double& _mean, double& _stdv){
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
            if(col_sum_map.count(count) > 0){
            	col_sum_map[count] = {
            		{"_accum_mean", t_val},
            		{"_accum_stdv", std::pow((t_val - _mean), 2)},
            		{"_accum_skew", std::pow( ((t_val - _mean)/_stdv), 3)},
            		{"_accum_kurt", std::pow( ((t_val - _mean)/_stdv), 4)}
            	};
            }
            else{
            	col_sum_map[count]["_accum_mean"] += t_val;
            	col_sum_map[count]["_accum_stdv"] += std::pow((t_val - _mean), 2);
            	col_sum_map[count]["_accum_skew"] += std::pow( ((t_val - _mean)/_stdv), 3);
            	col_sum_map[count]["_accum_kurt"] += std::pow( ((t_val - _mean)/_stdv), 4);
            }
        }
        start=end+1;
        ++count;
        end = s.find_first_of(delimiter, start);
    }
}

std::map<int, std::map<std::string, std::vector<double>>> mine_file(std::string& f_name, double& _overall_mean, double& _overall_stdv, double& _overall_skew, double& _overall_kurt){
    std::map<int, std::map<std::string, long double>> column_moment_sum_maps;
    std::map<int, std::map<std::string, std::vector<double> >> column_running_moments;

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
                string_split_histogram(row_string, ' ', column_moment_sum_maps, _overall_mean, _overall_stdv);
                row_string.clear();
                if( (count % bin_size) == 0){
                	for(auto it_col: column_moment_sum_maps){
                		double t_mean = it_col.second["_accum_mean"]/(double)bin_size;
                		double t_stdv = std::sqrt(it_col.second["_accum_stdv"]/ (bin_size-1));
                		double t_skew = it_col.second["_accum_skew"]/(double)bin_size;
                		double t_kurt = it_col.second["_accum_kurt"]/(double)bin_size;
                        int column = it_col.first;
                		if(column_running_moments.count(column) > 0){
			            	column_running_moments[column]["mean"].push_back(t_mean);
                            column_running_moments[column]["stdv"].push_back(t_stdv);
                            column_running_moments[column]["skew"].push_back(t_skew);
                            column_running_moments[column]["kurt"].push_back(t_kurt);
			            }
			            else{
                            column_running_moments[column] = {
                                {"mean", {t_mean}},
                                {"stdv", {t_stdv}},
                                {"skew", {t_skew}},
                                {"kurt", {t_kurt}}
                            };
			            }
                	}
                	column_moment_sum_maps.clear();
                }
            }else{
                row_string += c[k];
            }
        }
        i++;
    }
    cleanAndSave:
        fclose(dataFile);

    return column_running_moments;
}

void compute_running_moments_json_data(std::string& file, double& _overall_mean, double& _overall_stdv, double& _overall_skew, double& _overall_kurt){
    std::cout << "Starting Mining: " << file << "\n";
    std::map<int, std::map<std::string, std::vector<double>>> column_running_moments = mine_file(file, _overall_mean, _overall_stdv, _overall_skew, _overall_kurt);

    for(auto iter_chans: column_running_moments){
        for(auto iter_chan_moments: iter_chans.second){
            save_data(file, + "bin_size_" + std::to_string(bin_size) + "_channel_" + std::to_string(iter_chans.first) + "_" + iter_chan_moments.first, iter_chan_moments.second);
        }
        iter_chans.second.clear();     
    }
    std::cout << "Done: " << file << "\n\n";
}

int main(int argc, char *argv[]){
    std::vector<std::string> files;
    Json::Value nfb_unnormed_histogram_metadata = load_unnormed_histogram_metadata();

    double _global_mean = nfb_unnormed_histogram_metadata["mean"].asFloat();
    double _global_stdv = nfb_unnormed_histogram_metadata["stdv"].asFloat();
    double _global_skew = nfb_unnormed_histogram_metadata["skew"].asFloat(); 
    double _global_kurt = nfb_unnormed_histogram_metadata["kurt"].asFloat();
    
    std::string mined_file_list = "nfb-mined_file_list.txt";
    files = get_files_to_mine(mined_file_list);
    std::cout << "Files to mine: " << files.size() << "\n\n";

    for(int i=0;i<files.size();i++){
        compute_running_moments_json_data(files[i], _global_mean, _global_stdv, _global_skew, _global_kurt);
    }

	return 0;
}