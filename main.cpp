#include "bits/stdc++.h"

using namespace std;

template<typename T>
using priorityQ_min = priority_queue<T, vector<T>, greater<T> >;

using ll = long long;
using ull = unsigned long long;
using ulong = unsigned long;
using uint = unsigned int;
using dbl = double;

constexpr int INF = 1e9 + 7;
constexpr ll MINF = 1e18;
const dbl PI = acos(-1);

constexpr int K = 64;

int getCode(char c) {
    if (c == 'A') {
        return 0;
    }
    if (c == 'C') {
        return 1;
    }
    if (c == 'G') {
        return 2;
    }
    return 3;
}

int getOcc(char ch, int i, string &bwt, vector<vector<int> > &mas) {
    int res = mas[i / K][getCode(ch)];
    for (int j = i / K * K; j < i; ++j) {
        if (bwt[j] == ch) {
            res++;
        }
    }
    return res;
}

string revComp(string r) {
    reverse(r.begin(), r.end());
    for (char &c: r) {
        if (c == 'A') {
            c = 'T';
        } else if (c == 'T') {
            c = 'A';
        } else if (c == 'C') {
            c = 'G';
        } else if (c == 'G') {
            c = 'C';
        }
    }
    return r;
}

vector<int> getSuffArr(const string &s) {
    int n = s.size();
    vector<int> p(n), c(n), pn(n), cn(n), cnt(max(256, n));

    for (int i = 0; i < n; ++i) {
        cnt[s[i]]++;
    }

    for (int i = 1; i < 256; ++i) {
        cnt[i] += cnt[i - 1];
    }
    for (int i = 0; i < n; ++i) {
        p[--cnt[s[i]]] = i;
    }

    c[p[0]] = 0;
    int classes = 1;

    for (int i = 1; i < n; ++i) {
        if (s[p[i]] != s[p[i - 1]]) {
            classes++;
        }
        c[p[i]] = classes - 1;
    }

    for (int h = 0; (1 << h) < n && classes < n; ++h) {
        for (int i = 0; i < n; ++i) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0) {
                pn[i] += n;
            }
        }

        for (int i = 0; i < classes; ++i) {
            cnt[i] = 0;
        }
        for (int i = 0; i < n; ++i) {
            cnt[c[pn[i]]]++;
        }
        for (int i = 1; i < classes; ++i) {
            cnt[i] += cnt[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            p[--cnt[c[pn[i]]]] = pn[i];
        }

        classes = 1;
        cn[p[0]] = 0;
        for (int i = 1; i < n; ++i) {
            int cur = (p[i] + (1 << h)) % n;
            int prev = (p[i - 1] + (1 << h)) % n;

            if (c[p[i]] != c[p[i - 1]] || c[cur] != c[prev]) {
                classes++;
            }
            cn[p[i]] = classes - 1;
        }

        swap(c, cn);
    }
    return p;
}

pair<int, int> findExact(const string &r, vector<int> &C, string &bwt, vector<vector<int> > &mas, int m) {
    int lo = 0, hi = m + 1;
    for (int j = int(r.size()) - 1; j >= 0 && lo < hi; --j) {
        int c = getCode(r[j]);
        lo = C[c] + getOcc(r[j], lo, bwt, mas);
        hi = C[c] + getOcc(r[j], hi, bwt, mas);
    }
    return {lo, hi};
}

signed main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream in(argv[1]);
    string s, line;
    while (getline(in, line)) {
        if (line[0] == '>') {
            continue;
        }
        s += line;
    }
    int m = s.size();

    s += '$';
    vector<int> p = getSuffArr(s);
    string bwt(m + 1, ' ');
    for (int i = 0; i <= m; ++i) {
        bwt[i] = s[(p[i] + m) % (m + 1)];
    }

    vector<vector<int> > mas((m + 1) / K + 2, vector<int>(4));
    vector<int> cur(4);
    for (int i = 0; i <= m; ++i) {
        if (bwt[i] != '$') {
            cur[getCode(bwt[i])]++;
        }
        if (i % K == K - 1) {
            mas[i / K + 1] = cur;
        }
    }

    vector<int> C(4);
    C[0] = 1;
    for (int c = 1; c < 4; ++c) {
        C[c] = C[c - 1] + cur[c - 1];
    }

    ll total = 0, uniq = 0, multi = 0;
    vector<int> vec(m + 1);
    for (int i = 0; getline(cin, line); ++i) {
        if (i % 4 != 1) {
            continue;
        }
        total++;
        if (line.find('N') != string::npos) {
            continue;
        }

        auto [lo1, hi1] = findExact(line, C, bwt, mas, m);
        auto [lo2, hi2] = findExact(revComp(line), C, bwt, mas, m);
        int cnt = (hi1 - lo1) + (hi2 - lo2);
        if (cnt == 0) {
            continue;
        }
        if (cnt > 1) {
            multi++;
            continue;
        }

        uniq++;
        int pos;
        if (hi1 - lo1 == 1) {
            pos = p[lo1];
        } else {
            pos = p[lo2];
        }
        vec[pos]++;
        vec[pos + line.size()]--;
    }

    ll mapped = uniq + multi, covered = 0, depth = 0;
    for (int i = 0, sum = 0; i < m; ++i) {
        sum += vec[i];
        depth += sum;
        if (sum > 0) {
            covered++;
        }
    }

    cout << fixed << setprecision(2);
    cout << "Total reads: " << total << "\n";
    cout << "Mapped: " << mapped << "/" << total << " (" << 100.0 * mapped / total << "%)\n";
    cout << "Unmapped: " << total - mapped << "/" << total << " (" << 100.0 * (total - mapped) / total << "%)\n";
    cout << "Unique: " << uniq << "/" << total << " (" << 100.0 * uniq / total << "%)\n";
    cout << "Multi: " << multi << "/" << total << " (" << 100.0 * multi / total << "%)\n";
    cout << "Mean depth: " << dbl(depth) / m << "x\n";
    cout << "Covered: " << covered << "/" << m << " (" << 100.0 * covered / m << "%)\n";
}
