#include <iostream>
#include <fstream>
#include <cmath>
#include <json/json.h>
#include <json/reader.h>


Json::Value load_current_histogram(){
    Json::Value root;
    std::string filename_data = "nfb_histogram_data_real/nfb_histogram_data/nfb_histogram.json";
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

void save_data(std::string file_string, Json::Value& data){
    std::ofstream nfb_json;
    nfb_json.open(file_string);
    nfb_json << data;
    nfb_json.close();
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

std::vector<std::map<double, int>> remove_outliars_beyond_sigma(std::map<double, int> & h_map, double& _mean, double& _stdev, double _mu){
    std::map<double, int>  out_hist_map;
    std::map<double, int>  outliars_hist_map;
    std::vector<std::map<double, int> > filter_outliars_beyond_x_sigma;
    for(auto iter: h_map){
        if (std::abs(iter.first - _mean) > (_mu * _stdev))
            outliars_hist_map[iter.first] = iter.second;
        else
            out_hist_map[iter.first] = iter.second;
    }
    filter_outliars_beyond_x_sigma.push_back(out_hist_map);
    out_hist_map.clear();
    filter_outliars_beyond_x_sigma.push_back(outliars_hist_map);
    outliars_hist_map.clear();
    return filter_outliars_beyond_x_sigma;
}


int main(int argc, char *argv[]) {
	int num_histogram_values = 500;
    std::vector<std::string> files;
    std::cout << "Loading data" << "\n";
    Json::Value nfb_histogram_json = load_current_histogram();
    std::map<double, int> nfb_histogram;
    for (Json::ValueIterator it = nfb_histogram_json.begin(); it != nfb_histogram_json.end(); ++it){
        double value = atof(it.key().asString().c_str());
        nfb_histogram[value] = it->asFloat();
    }
    std::cout << "Total values: "<< nfb_histogram.size() << "\n";
    nfb_histogram_json.clear();

    double data_mean = mean_from_histogram_map(nfb_histogram);
    std::cout << "Calulating Mean: "<< data_mean << "\n";
    
    double data_stdv = calculate_stdv_from_histogram_map(nfb_histogram, data_mean);
    std::cout << "Calulating STDV: " << data_stdv << "\n";

    //trying to remove outliars beyond 100 sigma that skew distribution range
    std::vector<std::map<double, int>> nfb_histogram_filtered = remove_outliars_beyond_sigma(nfb_histogram, data_mean, data_stdv, 100);
    nfb_histogram.clear();

    std::cout << "Size filtered values: " << nfb_histogram_filtered[1].size() << std::endl;
    for (auto it: nfb_histogram_filtered[1])
        std::cout << it.first << ": " << it.second << std::endl;
        
    //clear ouliars for now to save on mem
    nfb_histogram_filtered[1].clear();
    
    std::map<double, int> nfb_f = nfb_histogram_filtered[0];
    
    double min_v = 0.0;
    double max_v = 0.0;
    double total_ = 0.0;

    for (auto it: nfb_f){
        double val = it.first;
    	if(val < min_v)
    		min_v = val;

    	if(val > max_v)
    		max_v = val;
    }

    double width = (max_v - min_v)/((double)num_histogram_values);
    std::cout << "min: " << min_v << "  max: " << max_v << std::endl;
    std::map<double,unsigned long long> h_vec;
    for(int i=0;i<num_histogram_values;++i){
    	double h_node = min_v + (i * width);
    	h_vec[h_node] = 0;
    	
    }

    for (auto it: nfb_f){
    	double value = it.first;
    	double occurance = it.second;
    	total_ += occurance;
    	for(int i=0;i<num_histogram_values;++i){
    		double h_node = min_v + (i * width);
    		if(value <= h_node){
    			h_vec[h_node] += occurance;
    			break;
    		}
    	}
    }

    nfb_f.clear();

    Json::Value out_hist_map;
    std::cout << "total_bins: " << total_ << std::endl;

    for(auto iter: h_vec)
        out_hist_map[std::to_string(iter.first)] = iter.second;

    save_data("nfb_histogram_data_real/nfb_histogram_data/nfb_histogram_binned_" + std::to_string(num_histogram_values) + ".json", out_hist_map);
	return 0;
}