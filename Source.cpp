#include <iostream>
#include <vector>
#include <cassert>


using namespace std;


void input_max_matrix(int n, int m, vector<vector<int>>& max) {
	cout << "Max matrix: \n";
	for (int i = 0; i < n; i++) {
		max[i] = vector<int>(m);
		for (int j = 0; j < m; j++) {
			cout << "Maximum number of instances of resource " << j << " for process " << i << " : ";
			cin >> max[i][j];
			assert(max[i][j] >= 0);
		}
	}
	cout << endl;
}


void input_allocation_matrix(int n, int m, vector<vector<int>>&allocation) {
	cout << "Allocation matrix: \n";
	for (int i = 0; i < n; i++) {
		allocation[i] = vector<int>(m);
		for (int j = 0; j < m; j++) {
			cout << "Number of allocated instances of resource " << j << " for process " << i << " : ";
			cin >> allocation[i][j];
			assert(allocation[i][j] >= 0);
		}
	}
	cout << endl;
}

void input_available_matrix(int m, vector<int>& available) {
	cout << "Available matrix: \n";
	for (int i = 0; i < m; i++) {
		cout << "Number of available instances of resource " << i << " : ";
		cin >> available[i];
		assert(available[i] >= 0);
	}
	cout << endl;
}


void calculate_need_matrix(int n, int m, vector<vector<int>> max, vector<vector<int>> allocation, vector<vector<int>> &need) {
	assert(max.size() == allocation.size());
	for (int i = 0; i < n; i++) {
		need[i] = vector<int>(m);
		for (int j = 0; j < m; j++) {
			assert(max[i].size() == allocation[i].size());
			need[i][j] = max[i][j] - allocation[i][j];
		}
	}
}

void present_need_matrix(int n, int m, vector<vector<int>>need) {
	cout << "The need matrix\n";
	cout << "--------------------\n";
	cout << "Resource:    ";
	for (int j = 0; j < m; j++) {
		cout << j << "    ";
	}
	cout << endl;
	cout << "--------------------\n";
	for (int i = 0; i < n; i++) {
		cout << "P" << i << "  |        ";
		for (int j = 0; j < m; j++) {
			cout << need[i][j];
			if (j != m - 1) {
				cout << "    ";
			}
		}
		cout << endl;
	}
	cout << "--------------------\n";
}


void check_for_negs_in_need(int n, int m, vector<vector<int>>need, bool *need_mat_status) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (need[i][j] < 0) {
				*need_mat_status = false;
				break;
			}
		}
	}
	if (*need_mat_status == false) {
		cout << "Wrong Input!\nMax MUST be greater than or equal allocation.\n";
	}
}


bool less_than_or_eq(vector<int>need, vector<int>work) {
	assert(need.size() == work.size());
	bool res = true;
	for (int i = 0; i < need.size(); i++) {
		if (need[i] > work[i]) {
			res = false;
			break;
		}
	}
	return res;
}


void add(vector<int> allocation, vector<int> work, vector<int> &res) {
	assert(allocation.size() == work.size());
	for (int i = 0; i < work.size(); i++) {
		res[i] = work[i] + allocation[i];
	}
}


// safety test
bool is_safe(vector<vector<int>> need, vector<vector<int>> allocation,vector<int> work, vector<int> &safe_order) {
	int n = need.size();
	bool safe = true;
	vector<bool> finish(n, false);
	while (true) {
		bool exit = true;
		for (int i = 0; i < n; i++) {
			if (less_than_or_eq(need[i], work) && finish[i] == false) {
				add(allocation[i], work, work);
				finish[i] = true;
				safe_order.push_back(i);
				exit = false;
			}
		}
		if (exit == true) {
			break;
		}
	}
	for (int i = 0; i < n; i++) {
		if (finish[i] == false) {
			safe = false;
			break;
		}
	}
	
	return safe;
}


void answer_safe_enquiry(vector<vector<int>>need, vector<vector<int>>allocation, vector<int> available, vector<int>safe_order) {
	if (is_safe(need, allocation, available, safe_order)) {
		cout << "Yes, Safe state <";
		for (int i = 0; i < safe_order.size(); i++) {
			cout << "P" << safe_order[i];
			if (i != safe_order.size() - 1) {
				cout << ",";
			}
			else {
				cout << ">\n";
			}
		}
	}
	else {
		cout << "No, It is not safe.\n";
	}
}


void input_request(int n, int m, int* p_req_num, vector<int>& request) {
	cout << "\nWhat is the number of the process do you want to create request for?\n";
	cin >> *p_req_num;
	assert(*p_req_num >= 0 && *p_req_num < n);
	cout << endl;
	for (int i = 0; i < m; i++) {
		cout << "Number of instances requested from resource " << i << " : ";
		cin >> request[i];
		assert(request[i] >= 0);
	}
	cout << endl;
}

void sub(vector<int> avail_need, vector<int> request, vector<int>& res) {
	assert(avail_need.size() == request.size());
	for (int i = 0; i < request.size(); i++) {
		res[i] = avail_need[i] - request[i];
	}
}


void answer_request_inquiry(int p_req_num, vector<int>request, vector<vector<int>>need, vector<vector<int>>& need_req, vector<int>available, vector<int>& available_req, vector<vector<int>>allocation, vector<vector<int>>& allocation_req, vector<int>&safe_order_req) {
	if (!(less_than_or_eq(request, need[p_req_num]))) {
		cout << "No, The request can't be granted! P" << p_req_num << " has exceeded its maximum claim\n";
	}
	else if (!(less_than_or_eq(request, available))) {
		cout << "No, The request can't be granted! P" << p_req_num << " must wait since resources are not available\n";
	}
	else {
		need_req = need;
		allocation_req = allocation;
		sub(available, request, available_req);
		add(allocation[p_req_num], request, allocation_req[p_req_num]);
		sub(need[p_req_num], request, need_req[p_req_num]);
		if (is_safe(need_req, allocation_req, available_req, safe_order_req)) {
			cout << "Yes, The request can be granted with safe state. Safe state <";
			cout << "P" << p_req_num << "req,";
			for (int i = 0; i < safe_order_req.size(); i++) {
				cout << "P" << safe_order_req[i];
				if (i != safe_order_req.size() - 1) {
					cout << ",";
				}
				else {
					cout << ">\n";
				}
			}
		}
		else {
			cout << "No, The request can't be granted! The request results in an unsafe state\n";
		}
	}
}


int main() {
	string exit;
	cout << "For positive answer type: yes\nFor negative answer type: no\n\n";

	while (1) {
		// getting number of processes and resources
		int n;
		int m;
		string check_flag;
		bool need_mat_status = true;
		string inquiry[2] = { "","" };
		int p_req_num;
		string exit_reqs;

		cout << "Enter Number of Processes: ";
		cin >> n;
		assert(n > 0);
		cout << "Enter Number of Resources: ";
		cin >> m;
		assert(m > 0);

		// data structures neeeded
		vector<int> available(m);
		vector<vector<int>> max(n);
		vector<vector<int>> allocation(n);
		vector<vector<int>> need(n);
		vector<int> safe_order;
		vector<int> request(m);
		vector<int> available_req(m);
		vector<vector<int>> allocation_req(n);
		vector<vector<int>> need_req(n);
		vector<int> safe_order_req;
		cout << endl;

		// getting max and allocation from user
		input_max_matrix(n, m, max);
		input_allocation_matrix(n, m, allocation);

		// calculating need matrix
		calculate_need_matrix(n, m, max, allocation, need);


		// checking that no negative values in need matrix
		check_for_negs_in_need(n, m, need, &need_mat_status);

		assert(need_mat_status == true);

		// getting available from user
		input_available_matrix(m, available);

		// getting safe state inquiry from user
		cout << "Do you want to check if the system in the safe state?\n";
		cin >> inquiry[0];
		assert(inquiry[0] == "yes" || inquiry[0] == "no");

		// getting immediate request inquireis from user
		cout << "\nDo you want to check a certain request?\n";
		cin >> check_flag;
		assert(check_flag == "yes" || check_flag == "no");
		cout << endl;
		if (check_flag == "yes") {
			input_request(n, m, &p_req_num, request);
		}
		
	
		// presenting need matrix
		cout << "\n";
		present_need_matrix(n, m, need);

		// getting safe state
		if (inquiry[0] == "yes") {
			cout << "\nIs the system at safe state or not?\n";
			cout << "------------------------------------------\n";
			answer_safe_enquiry(need, allocation, available, safe_order);
		}

		// if the request can be granted or not
		if (check_flag == "yes") {
			cout << "\nCan the request be granted or not?\n";
			cout << "------------------------------------------\n";
			answer_request_inquiry(p_req_num, request, need, need_req, available, available_req, allocation, allocation_req, safe_order_req);

		}

		cout << "\n";

		// looping for request
		while (1) {
			cout << "\nDo you want to try another request? \"Note: the next request doesn't depend on the result of the previous request\"\n";
			cin >> exit_reqs;
			assert(exit_reqs == "yes" || exit_reqs == "no");
			cout << endl;
			if (exit_reqs == "no") {
				break;
			}
			for (int i = 0; i < safe_order_req.size(); i++) {
				safe_order_req.pop_back();
			}
			// getting input
			input_request(n, m, &p_req_num, request);
			// getting result
			cout << "\nCan the request be granted or not?\n";
			cout << "------------------------------------------\n";
			answer_request_inquiry(p_req_num, request, need, need_req, available, available_req, allocation, allocation_req, safe_order_req);
		}







		//-------------------------------------------------------------------------------------
		cout << "\nDo you want to restart the program?\n";
		cin >> exit;
		assert(exit == "yes" || exit == "no");
		cout << endl;
		if (exit == "no") {
			break;
		}
		system("cls");
		cout << "For positive answer type: yes\nFor negative answer type: no\n\n";
	}
	
	


	cout << "\n\n";
	system("pause");
	return 0;
}