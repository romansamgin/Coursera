#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
using namespace std;


bool IsSubdomainReverse(const string& subdomain, const string& domain) {
    size_t pos = domain.find(subdomain);
    if (pos == 0 && 
        (domain[subdomain.size()] == '.' || subdomain.size() == domain.size())) {
        return true;    
    }
    return false;
}


vector<string> ReadDomains() {
    size_t count;
    cin >> count;
    vector<string> domains(count);
    string domain;
    for (size_t i = 0; i < count; ++i) {
        cin >> domain;
        reverse(domain.begin(), domain.end());
        domains[i] = domain;
    }
    return domains;
}


set<string> ReadBannedDomains() {
    set<string> domains;
    size_t count;
    cin >> count;
    string domain;
    for (int i = 0; i < count; ++i) {
        cin >> domain;
        reverse(domain.begin(), domain.end());
        domains.insert(domain);  
    }
    return domains;
}

int main() {
    set<string> domains_banned = ReadBannedDomains();
    vector<string> domains_to_check = ReadDomains();
    for (const string& domain : domains_to_check) {
        string prefix = domain.substr(0, domain.find('.'));
        string result = "Good\n";
        auto it_begin = domains_banned.lower_bound(prefix);
        for (auto it = it_begin; it != domains_banned.end(); it++) {
            string banned_prefix = (*it).substr(0, (*it).find('.'));
            if (banned_prefix != prefix) {
                break;
            }
            if (IsSubdomainReverse(*it, domain)) {
                result = "Bad\n";
                break;
            }
        }
        cout << result;
    }
    return 0;
}
