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

const int is_test = 1;

const int offset_node = 2;

const int num_leads = 128;

//factor to convert biosemi values into uv
double biosemi_microvoltage_factor = 8192;

//sampling rate
double sampling_rate = 2048;

Json::Value load_histogram(std::string& data_file){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data/nfb_histogram_"+ data_file +".json";
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

Json::Value load_unnormed_histogram(std::string& data_file){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data/nfb_unnormed_histogram_"+ data_file +".json";
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

double calculate_stdv_from_histogram_map(std::map<double, unsigned long int>& h_map, double& _mean){
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

double calculate_skewness_from_histogram_map(std::map<double, unsigned long int>& h_map, double& _mean, double& _stdev){
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

double calculate_kurtosis_from_histogram_map(std::map<double, unsigned long int>& h_map, double& _mean, double& _stdev){
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

double mean_from_histogram_map(std::map<double, unsigned long int>& h_map){
    double _accum_val = 0.0;
    double _accum_length = 0.0;
    for(auto iter: h_map){
        _accum_val += iter.first * iter.second;
        _accum_length += iter.second;
    }
    double _accum = _accum_val/_accum_length;
    return _accum;
}

std::map<double, unsigned long int> norm_histogram(std::map<double, unsigned long int>& h_map, double& _mean, double& _stdv){
    std::map<double, unsigned long int> o_map;
    for(auto iter: h_map){
        double normed_val = ((iter.first - _mean)/_stdv);
        o_map[normed_val] = iter.second;
    }
    return o_map;
}

void save_data(std::string file_string, Json::Value& data){
    std::ofstream nfb_json;
    nfb_json.open(file_string);
    nfb_json << data;
    nfb_json.close();
}

int main(int argc, char *argv[]) {
	Json::Value nfb_unnormed_histogram_file;
	int file_count = 0;

	std::cout << "Loading metadata" << "\n";
	Json::Value nfb_histogram_metadata_json = load_current_histogram_metadata();
	for (Json::ValueIterator it = nfb_histogram_metadata_json.begin(); it != nfb_histogram_metadata_json.end(); ++it){
		std::string uf_name  = it.key().asString();
		std::vector<std::string> file_strings;
		boost::split(file_strings,uf_name,boost::is_any_of("/"));
		std::string f_name = file_strings[file_strings.size()-1];

		double t_mean = it->get("mean", "mean").asFloat();
		double t_stdv = it->get("stdv", "stdv").asFloat();
		double t_skew = it->get("skew", "skew").asFloat();
		double t_kurt = it->get("kurt", "kurt").asFloat();

		std::cout << "Loading histogram data for: " << f_name <<  "\n";
	    Json::Value tmp_nfb_histogram_json = load_histogram(f_name);
	    Json::Value tmp_nfb_unnormed_histogram;

	    for (Json::ValueIterator t_it = tmp_nfb_histogram_json.begin(); t_it != tmp_nfb_histogram_json.end(); ++t_it){
	        double value = (atof(t_it.key().asString().c_str()) * t_stdv) + t_mean;
	        tmp_nfb_unnormed_histogram[std::to_string(value)] = t_it->asInt();
	    }

	    ++file_count;
	    std::cout << "Done: " << f_name <<  "\n";
	    if (file_count % 100 == 0)
	    	std::cout << "Compleded " <<  file_count <<"\n";
	    save_data("nfb_histogram_data/nfb_unnormed_histogram_" + file_strings[file_strings.size()-1] + ".json", tmp_nfb_unnormed_histogram);


	    tmp_nfb_histogram_json.clear();
	    tmp_nfb_unnormed_histogram.clear();

	}

	long double _accum_val_mean = 0.0;
	double _global_mean = 0.0;
	long double _accum_val_var = 0.0;
	double _global_stdv = 0.0;
	long double _accum_val_skew = 0.0;
	double _global_skew = 0.0;
	long double _accum_val_kurt = 0.0;
	double _global_kurt = 0.0;
    unsigned long int _accum_length = 0.0;

    /* COMPUTING MEAN */

	for (Json::ValueIterator it = nfb_histogram_metadata_json.begin(); it != nfb_histogram_metadata_json.end(); ++it){
		std::string uf_name  = it.key().asString();
		std::vector<std::string> file_strings;
		boost::split(file_strings,uf_name,boost::is_any_of("/"));
		std::string f_name = file_strings[file_strings.size()-1];

		Json::Value tmp_nfb_unnormed_histogram = load_unnormed_histogram(f_name);
		for (Json::ValueIterator t_it = tmp_nfb_unnormed_histogram.begin(); t_it != tmp_nfb_unnormed_histogram.end(); ++t_it){
	        double value = (atof(t_it.key().asString().c_str()) );
	        int count = t_it->asInt();

	        _accum_val_mean += value * count;
		    _accum_length += count;
	    }
	    tmp_nfb_unnormed_histogram.clear();
	}

	_global_mean = _accum_val_mean/_accum_length;

	 /* COMPUTING STDV */

	for (Json::ValueIterator it = nfb_histogram_metadata_json.begin(); it != nfb_histogram_metadata_json.end(); ++it){
		std::string uf_name  = it.key().asString();
		std::vector<std::string> file_strings;
		boost::split(file_strings,uf_name,boost::is_any_of("/"));
		std::string f_name = file_strings[file_strings.size()-1];

		Json::Value tmp_nfb_unnormed_histogram = load_unnormed_histogram(f_name);
		for (Json::ValueIterator t_it = tmp_nfb_unnormed_histogram.begin(); t_it != tmp_nfb_unnormed_histogram.end(); ++t_it){
	        double value = (atof(t_it.key().asString().c_str()) );
	        int count = t_it->asInt();
	        _accum_val_var += std::pow((value - _global_mean), 2) * count;
	    }
	    tmp_nfb_unnormed_histogram.clear();
	}

	_global_stdv = std::sqrt(_accum_val_var/ (_accum_length-1));

	/* COMPUTING KURT and SKEW */

	for (Json::ValueIterator it = nfb_histogram_metadata_json.begin(); it != nfb_histogram_metadata_json.end(); ++it){
		std::string uf_name  = it.key().asString();
		std::vector<std::string> file_strings;
		boost::split(file_strings,uf_name,boost::is_any_of("/"));
		std::string f_name = file_strings[file_strings.size()-1];

		Json::Value tmp_nfb_unnormed_histogram = load_unnormed_histogram(f_name);
		for (Json::ValueIterator t_it = tmp_nfb_unnormed_histogram.begin(); t_it != tmp_nfb_unnormed_histogram.end(); ++t_it){
	        double value = (atof(t_it.key().asString().c_str()) );
	        int count = t_it->asInt();
	        _accum_val_skew += std::pow( ((value - _global_mean)/_global_stdv), 3) * count;
	        _accum_val_kurt += std::pow( ((value - _global_mean)/_global_stdv), 4) * count;
	    }
	    tmp_nfb_unnormed_histogram.clear();
	}

	_global_skew = _accum_val_skew/_accum_length;
	_global_kurt = _accum_val_kurt/_accum_length;



	Json::Value nfb_unnormed_histogram_metadata;
    nfb_unnormed_histogram_metadata["mean"] = _global_mean;
    nfb_unnormed_histogram_metadata["stdv"] = _global_stdv; 
    nfb_unnormed_histogram_metadata["skew"] = _global_skew; 
    nfb_unnormed_histogram_metadata["kurt"] = _global_kurt; 
    save_data("nfb_histogram_data/nfb_unnormed_histogram_metadata.json", nfb_unnormed_histogram_metadata);
	return 0;
}