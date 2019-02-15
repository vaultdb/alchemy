//create methods for doing conversions, printing, and comparing

int size() {
	return lengths.size();
}

int data_length() {
	int result = 0;
	for (unsigned int i=0; i<lengths.size(); i++) {
		result += lengths[i];
	}	
	return result;
}

bool operator < (const Row& r) const
{
	string col = data.substr(0, lengths[0]);
	string r_col = r.data.substr(0, lengths[0]);

        return col < r_col;
}

bool *to_bool() {
	int l = data_length();
	bool *result = new bool[l];
	const char *tmp = data.c_str();
	
	for (int i=0; i<l; i++) 
		result[i] = (tmp[i] == '0') ? false : true;

	return result;
}

string to_string() {
	string result = "";
	int ptr = 0;
	for (int i=0; i<size(); i++) {
		string cur_val = data.substr(ptr, ptr + lengths[i]);
		
		//reverse for endian-ness
		std::reverse(cur_val.begin(), cur_val.end());
		
		//convert different types
		if (lengths[i] == 64) {
			cur_val = std::to_string(std::stoll(cur_val, nullptr, 2));
		} else if (lengths[i] == 1) {
			cur_val = cur_val;
		} else {
			cur_val = binstr_to_str(cur_val);
		}

		result = (i == 0) ? result + cur_val : result + ", " + cur_val;  
		ptr += lengths[i];
	}

	return result;
}

