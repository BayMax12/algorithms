#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <ctime>

using namespace std;

/* find a pair (c,d) s.t. ac + bd = gcd(a,b)
 * Dependencies: none */
pair<long long, long long> extended_gcd(long long a, long long b) {
	if (b == 0) return make_pair(1, 0);
	pair<long long, long long> t = extended_gcd(b, a % b);
	return make_pair(t.second, t.first - t.second * (a / b));
}

/* Find x in [0,m) s.t. ax ≡ gcd(a, m) (mod m)
 * Dependencies: extended_gcd(a, b) */
long long modinverse(long long a, long long m) {
	return (extended_gcd(a, m).first % m + m) % m;
}

struct ntfft
{
	// 7 * 2^26 + 1
	static const int ourmod = 469762049;
	static const int default_prim = 3;
	int n, w;
	ntfft(int _n)
	{
		--_n;
		for (int i = 1; i < 32 && (_n & (_n+1)); i += i) _n = (_n | (_n>>i));
		n = ++_n;
		// default_prim^(7*2^26) = 1
		// (default_prim^(7*2^26/n))^n = 1
		int powcnt = (7<<26) / n;
		w = 1;
		for (long long curp = default_prim; powcnt; ) {
			if (powcnt & 1) w = w*curp%ourmod;
			powcnt >>= 1;
			curp = curp*curp%ourmod;
		}
	}
	void fft(int *p, const int n, const int s, int *res, const int w)
	{
		if (n == 4) {
			auto A = (p[0] + p[s*2]);
			auto B = (p[0] - p[s*2]);
			auto C = (p[s] + p[s*3]);
			auto D = (p[s] - p[s*3]);
			res[2] = (A-C)%ourmod;
			res[0] = (A+C)%ourmod;
			res[3] = (B - (long long)w*D)%ourmod;
			res[1] = (B + (long long)w*D)%ourmod;
			return;
		}
		if (n == 2) {
			res[0] = (p[0] + p[s]) % ourmod;
			res[1] = (p[0] - p[s]) % ourmod;
			return;
		}
		if (n == 1) { *res = *p; return; }
		fft(p, n>>1, s*2, res, w*(long long)w % ourmod);
		fft(p+s, n>>1, s*2, res + (n>>1), w*(long long)w % ourmod);
		long long w_power = 1;
		for (int i = 0; i < (n>>1); i++) {
			auto ofs = w_power * res[i+(n>>1)];
			res[i+(n>>1)] = (res[i] - ofs) % ourmod;
			res[i] = (res[i] + ofs) % ourmod;
			w_power = (w_power * w) % ourmod;
		}
	}
	vector<int> fft(vector<int> &p) {
		vector<int> res(n);
		p.resize(n);
		fft(p.data(), n, 1, res.data(),w);
		return res;
	}
	vector<int> ifft(vector<int> &p)
	{
		vector<int> res(n);
		p.resize(n);
		fft(p.data(), n, 1, res.data(), modinverse(w, ourmod));
		long long in = modinverse(n, ourmod);
		for (auto &v : res)
		{
			v = v * in % ourmod + ourmod;
			if (v >= ourmod) v -= ourmod;
		}
		return res;
	}
};

