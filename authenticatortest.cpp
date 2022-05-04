/*
 * Copyright (c) 2015 Tim Ruffing <tim.ruffing@mmci.uni-saarland.de>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <gtest/gtest.h>
#include "../chameleonhash.h"
#include "../authenticator.h"
#include <ctime>
#include <random>
#include <array>
#include <iomanip>
#include <cstdio>

using namespace std;

class AuthenticatorTest{
public:
    static const ChameleonHash::pk_t pk;
    static const ChameleonHash::sk_t sk;
    static const ChameleonHash::W w;

    // some deterministic test messages
    ChameleonHash::mesg_t m1, m2;
    Authenticator::ct_t ct;
    
};

const ChameleonHash::pk_t AuthenticatorTest::pk = {
    0x03, 0x17, 0x0a, 0x37, 0x72, 0x41, 0xd9, 0x4a,
    0x5f, 0x4c, 0x85, 0xf1, 0x99, 0xc7, 0x96, 0xc5,
    0xa9, 0xf9, 0xf5, 0x6b, 0x3e, 0x2d, 0x6a, 0xea,
    0x18, 0xbe, 0x91, 0x88, 0xd1, 0x31, 0x31, 0x76,
    0x9c
};

// the corresponding secret exponent in big-endian
const ChameleonHash::sk_t AuthenticatorTest::sk = {
    0xb2, 0x19, 0x77, 0xc8, 0xca, 0x1c, 0xbb, 0x55,
    0xf0, 0xa3, 0xef, 0xfd, 0x99, 0x66, 0xe3, 0xd5,
    0xc9, 0x58, 0x86, 0x88, 0xfa, 0x02, 0xbf, 0x7a,
    0x0d, 0x2a, 0xf7, 0xb6, 0x36, 0x6f, 0x1e, 0x8f
};

const ChameleonHash::W AuthenticatorTest::w = {
    0xb2, 0x19, 0x77, 0xc8, 0xca, 0x1c, 0xbb, 0x55,
    0xf0, 0xa3, 0xef, 0xfd, 0x99, 0x66, 0xe3, 0xd5,
    0xc9, 0x58, 0x86, 0x88, 0xfa, 0x02, 0xbf, 0x7a,
    0x0d, 0x2a, 0xf7, 0xb6, 0x36, 0x6f, 0x1e, 0x8f
};
int strToInt(char *s){
	int x = 0;
	for(int i = 0; s[i]!=0; i++){
		x = x*10+s[i]-'0';
	}
	return x;
}

int hexToDec(char c){
	if('a' <= c && c <= 'f') return c-'a'+10;
	return c-'0';
}
//int main(int argc, char **argv){
//	AuthenticatorTest authTest;
//	Authenticator acca(authTest.sk, authTest.w, 0);
//	Authenticator::token_t t1, t2;
//	
//	//获取ct 
//	cout << "=======ct========" << endl;
//	for(int i = 0; i < 8; i++){
//		int x = strToInt(argv[i+1]);
//		authTest.ct[i] = (unsigned char)x;
//		printf("0x%x ",x); 
//	} 
//	printf("\n");
//	cout << "=======m1========" << endl;
//	//获取m 
//	for(int i = 0; argv[9][i]!=0; i++){
//		authTest.m1.push_back((unsigned char)argv[9][i]);
//	} 
//	for(int i = 0; argv[9][i]!=0; i++) printf("%c",authTest.m1[i]); 
//	printf("\n");
//	cout << "=======m2========" << endl;
//	//获取m 
//	for(int i = 0; argv[10][i]!=0; i++){
//		authTest.m2.push_back((unsigned char)argv[10][i]);
//	} 
//	for(int i = 0; argv[10][i]!=0; i++) printf("%c",authTest.m2[i]); 
//	printf("\n");
//	
//	int n = strToInt(argv[11]);
//	cout << "=======n========" << endl;
//	printf("%d\n",n); 
//
//
//	acca.authenticate(t1, authTest.ct, authTest.m1, 0);
//	
//	acca.authenticate(t2, authTest.ct, authTest.m2, n);
//	
//	acca.extract(t1, t2, authTest.ct, authTest.m1, authTest.m2, 0, n);
//	
//	EXPECT_EQ(authTest.sk, acca.getDsk());
//	
//	return 0;
//}
//int main(int argc, char **argv){
//	AuthenticatorTest authTest;
//	Authenticator acca(authTest.sk, authTest.w, 0);
//	Authenticator::token_t t, t1;
//	clock_t start, end;
//	//获取ct 
//	cout << "=======ct========" << endl;
//	for(int i = 0; i < 8; i++){
//		int x = strToInt(argv[i+1]);
//		authTest.ct[i] = (unsigned char)x;
//		printf("0x%x ",x); 
//	} 
//	printf("\n");
//	cout << "=======m========" << endl;
//	//获取m 
//	for(int i = 0; argv[9][i]!=0; i++){
//		authTest.m1.push_back((unsigned char)argv[9][i]);
//	} 
//	for(int i = 0; argv[9][i]!=0; i++) printf("%c",authTest.m1[i]); 
//	printf("\n");
//	int n = strToInt(argv[10]);
//	cout << "=======n========" << endl;
//	printf("%d\n",n); 
//	
//	int cnt = 0;
//	for(int i = 0; i < Authenticator::DEPTH; i++){
//		for(int j = 0; j < 33; j++){
//			t1.chs[i][j] = hexToDec(argv[11][cnt])*16+hexToDec(argv[11][cnt+1]);
//			cnt+=2;
//		}
//	}
//	for(int i = 0; i < Authenticator::DEPTH; i++){
//		for(int j = 0; j < 32; j++){
//			t1.rs[i][j] = hexToDec(argv[11][cnt])*16+hexToDec(argv[11][cnt+1]);
//			cnt+=2;
//		}
//	}
//	
//	Authenticator::dpk_t dpk = acca.getDpk();
//	ChameleonHash ch(authTest.sk, authTest.w, n);
//	dpk.chpk = ch.getPk(true);
//	Authenticator accaV(dpk, authTest.w);
//
//	EXPECT_TRUE(accaV.verify(t1, authTest.ct, authTest.m1, n));
//	if(accaV.verify(t1, authTest.ct, authTest.m1, n)) cout << "true" << endl;
//	else cout << "false" << endl;
//	return 0;
//} 


int main(int argc, char **argv){
	AuthenticatorTest authTest;
	Authenticator acca(authTest.sk, authTest.w, 0);
	Authenticator::token_t t, t1;
	clock_t start, end;
	//获取ct 
	cout << "=======ct========" << endl;
	for(int i = 0; i < 8; i++){
		int x = strToInt(argv[i+1]);
		authTest.ct[i] = (unsigned char)x;
		printf("0x%x ",x); 
	} 
	printf("\n");
	cout << "=======m========" << endl;
	//获取m 
	for(int i = 0; argv[9][i]!=0; i++){
		authTest.m1.push_back((unsigned char)argv[9][i]);
	} 
	for(int i = 0; argv[9][i]!=0; i++) printf("%c",authTest.m1[i]); 
	printf("\n");
	int n = strToInt(argv[10]);
	cout << "=======n========" << endl;
	printf("%d\n",n); 
	acca.authenticate(t, authTest.ct, authTest.m1, n);
	for(int i = 0; i < Authenticator::DEPTH; i++){
		for(int j = 0; j < 33; j++){
			printf("%02x",t.chs[i][j]);
		}
	}
	for(int i = 0; i < Authenticator::DEPTH; i++){
		for(int j = 0; j < 32; j++){
			printf("%02x",t.rs[i][j]);
		}
	}
	printf("\n");
	return 0;
} 

//int main(int argc, char **argv){
//	AuthenticatorTest authTest;
//	Authenticator acca(authTest.sk, authTest.w, 0);
//	Authenticator::token_t t1, t2;
//	
//	//获取ct 
//	cout << "=======ct========" << endl;
//	for(int i = 0; i < 8; i++){
//		int x = strToInt(argv[i+1]);
//		authTest.ct[i] = (unsigned char)x;
//		printf("0x%x ",x); 
//	} 
//	printf("\n");
//	cout << "=======m1========" << endl;
//	//获取m 
//	for(int i = 0; argv[9][i]!=0; i++){
//		authTest.m1.push_back((unsigned char)argv[9][i]);
//	} 
//	for(int i = 0; argv[9][i]!=0; i++) printf("%c",authTest.m1[i]); 
//	printf("\n");
//	cout << "=======m2========" << endl;
//	//获取m 
//	for(int i = 0; argv[10][i]!=0; i++){
//		authTest.m2.push_back((unsigned char)argv[10][i]);
//	} 
//	for(int i = 0; argv[10][i]!=0; i++) printf("%c",authTest.m2[i]); 
//	printf("\n");
//	
//	int n = strToInt(argv[11]);
//	cout << "=======n========" << endl;
//	printf("%d\n",n); 
//
//	clock_t start, end;
//	int total = 0;
//	start = clock();
//
//	acca.authenticate(t1, authTest.ct, authTest.m1, 0);
//	for (int i = 1; i <= n; i++) {
//		acca.authenticate(t2, authTest.ct, authTest.m2, i);
//		start = clock();
//		acca.extract(t1, t2, authTest.ct, authTest.m1, authTest.m2, 0, i);
//		end = clock();
//		total += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//		EXPECT_EQ(authTest.sk, acca.getDsk());
//	}
//	end = clock();
//	printf("extract totile time=%d ms\n", total);
//	return 0;
//}
//int main(int argc, char **argv){
//	AuthenticatorTest authTest;
//	Authenticator acca(authTest.sk, authTest.w, 0);
//	Authenticator::token_t t;
//	clock_t start, end;
//	int total1 = 0, total2 = 0, total3 = 0;
//	//获取ct 
//	cout << "=======ct========" << endl;
//	for(int i = 0; i < 8; i++){
//		int x = strToInt(argv[i+1]);
//		authTest.ct[i] = (unsigned char)x;
//		printf("0x%x ",x); 
//	} 
//	printf("\n");
//	cout << "=======m1========" << endl;
//	//获取m 
//	for(int i = 0; argv[9][i]!=0; i++){
//		authTest.m1.push_back((unsigned char)argv[9][i]);
//	} 
//	for(int i = 0; argv[9][i]!=0; i++) printf("%c",authTest.m1[i]); 
//	printf("\n");
//	int n = strToInt(argv[10]);
//	cout << "=======n========" << endl;
//	printf("%d\n",n); 
//	start = clock();
//	for (int i = 1; i <= n; i++) {
//		Authenticator::dpk_t dpk = acca.getDpk();
//		ChameleonHash ch(authTest.sk, authTest.w, i);
//		dpk.chpk = ch.getPk(true);
//		Authenticator accaV(dpk, authTest.w);
//	}
//	end = clock();
//	total3 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//
//	for (int i = 1; i <= n; i++) {
//		start = clock();
//		acca.authenticate(t, authTest.ct, authTest.m1, i);
//		end = clock();
//
//		total1 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//
//		Authenticator::dpk_t dpk = acca.getDpk();
//		ChameleonHash ch(authTest.sk, authTest.w, i);
//		dpk.chpk = ch.getPk(true);
//		Authenticator accaV(dpk, authTest.w);
//
//		start = clock();
//		EXPECT_TRUE(accaV.verify(t, authTest.ct, authTest.m1, i));
//		end = clock();
//
//		total2 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//	}
//	end = clock();
//	printf("authenticate totile time=%d ms\n", total1);
//	printf("verify totile time=%d ms\n", total2);
//	printf("getKey totile time=%d ms\n", total3);
//	return 0;
//} 
//TEST_F(AuthenticatorTest, TestSingleDataTime) {
//	Authenticator acca(sk, w, 0);
//	Authenticator::token_t t;
//	clock_t start, end;
//	int total1 = 0, total2 = 0, total3 = 0;
//	
//	start = clock();
//	for (int i = 1; i <= 100; i++) {
//		Authenticator::dpk_t dpk = acca.getDpk();
//		ChameleonHash ch(sk, w, i);
//		dpk.chpk = ch.getPk(true);
//		Authenticator accaV(dpk, w);
//	}
//	end = clock();
//	total3 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//
//	for (int i = 1; i <= 100; i++) {
//		start = clock();
//		acca.authenticate(t, ct, m1, i);
//		end = clock();
//
//		total1 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//
//		Authenticator::dpk_t dpk = acca.getDpk();
//		ChameleonHash ch(sk, w, i);
//		dpk.chpk = ch.getPk(true);
//		Authenticator accaV(dpk, w);
//
//		start = clock();
//		EXPECT_TRUE(accaV.verify(t, ct, m1, i));
//		end = clock();
//
//		total2 += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//	}
//	end = clock();
//	printf("authenticate totile time=%d ms\n", total1);
//	printf("verify totile time=%d ms\n", total2);
//	printf("getKey totile time=%d ms\n", total3);
//}
//
//
//TEST_F(AuthenticatorTest, TestAuthenticatorExtractSimpleTime) {
//	Authenticator acca(sk, w, 0);
//	Authenticator::token_t t1, t2;
//
//
//	clock_t start, end;
//	int total = 0;
//	start = clock();
//
//	acca.authenticate(t1, ct, m1, 0);
//	for (int i = 1; i <= 100; i++) {
//		acca.authenticate(t2, ct, m2, i);
//		start = clock();
//		acca.extract(t1, t2, ct, m1, m2, 0, i);
//		end = clock();
//		total += (int)((float)(end - start) * 1000 / CLOCKS_PER_SEC);
//		EXPECT_EQ(sk, acca.getDsk());
//	}
//	end = clock();
//	printf("extract totile time=%d ms\n", total);
//}
//
/*
64 4 255 23 95 169 23 35 abcd 50
64 4 255 23 95 169 23 35 abcd accb 50
*/

/*
64 4 255 23 95 169 23 35 abcd 50 02a100f7692de3f8c96f8d0862b8e60a3fde0f71c62f717ab1e1e6faba95b19aff02378c37baf7773df52e12090b632be8b235e3997e1f7803a8bb90fc546fd68dee034d9043f011af8ac996aa5dd467f42eafad94eaa64d9281c98091c6d4afd0f36e022843ce8e626bf892b83da7326dafbc1bf02b2d181dd20b4d1fb476858c59a925038b3e949d82f478a5ef1165d39347f59db2dd23585533e4e54bf7432a6fcc8ff603d51198b466f1bf4cb82b2cd82a8e784675cc943e5ea4a9dce5ac441fa3b69cc9028170ec6b06da4fcc4f222204c31c063da1cd1b68d12d09681cb1ba23175be9d602da9df0df47efcc9520bc5f5f773284f7fd8f1587974c64365d8a9d852a353234022e660b9a2ccf50413a8964f849e11c11447a2db0b5aa2185fc8901fe27e9462a03e245079b604673b336e0cfc3723280807f2478acc44c61eee644c96890cb37f80322612f753a5683899f9249102affd78b073a4c3ad5925eb2726e7893adfad52b02d0b9d10c48061606f638493b3510f3e2cc73a19a0e4afef0fe16909b3b45395f03cf206f812f8c51b9e9e2c0a30c397fe2d885455424e2e4bd087aa91d8ed94a260358836b2b8de736d6fb060dad57f70e62a65467dcd5f3a3720ea300c67592697c039b2e11a75385b682e21418e786501132db4feac3ad9e1c0308262f600df0bca402b4b2cddb0aafd6db810dc387aa9ab4f6694dde210442115cfb1092f4ed99a5770290c4637552b9e38b96ff642751c892c8ec62b2314fa1c4e7e57b363e98d3750e02db512032330e455df8247ce2dad6d5df8cb777203ee51b3c698e4885452795ec031590ca93bbf29b12be1d4d6ed916a80ec95bde13b13e735b9c046892113ea2c502d98be8a93908064c35dd5abc012da91cd8e0947c9acd45eccd4f83044a635b3e03a1fff2070b9e0186fef4f33fdb29466eab78dff443e0cb4ac6fc48b59e0fb719023bcc0013f1d1559ff116e14ba3962d26ac22b5ca4dbc6eb103c5c87968e8248d025413fe9e1f5fc23a73dec9fb48dc08768a563b46aad308a9e51e76e55838011202c101cf9c8a15d1aaf5eb1db3b1e59c616fc6102f9e9ffca9f0d793c65af79bc80284031efae4221e496313fa26445935cfa7de6f42711e5bc85f3c2c2c952ff2af02834eb8e29acc1cf2172032ecc0967296e41fd1d162aca84930cfe14392b39624039e0b895cf49cc97da95695c5e0338a3290034b42e630e6d4d4053320bdcf66ed033854c8bee6e6d44531de76d6f1bf355b23c8aebf2ce73beb5e0a991bcc5354ba03aeeea41cd9a8e4ae4d524dda0e2884eed08366c39c53524c3ae706d538ab83210298939b992c72884894dd7c5ce790fbdab7a752b9dfec50f2cfec4292071c1e3303c8959e619de6875f644821c6d412573eb7c9db4714d6248f7b5bdab6a81b5af80307c03463f17991c43038d27032453c7d6e4dab114c85e38205bdc895393f6bfa03a69a43496fb3a8cfaea58bf66dac758bf491a45f7bf97b9185dd7da0479035fd0323ca614dc0c710a7226815034ae349aafaf4642308de5ab7c9d4cb0fb978581c0341342b411a6fabc02a4cc617483248cbbe6e85d80e602f1ba55dfbae265f1066027d73108ba9a90ebb6b28d890367671fdc07ec8588568f0898c70830b8c8765bd0237182495c9ced2bc3891164341fca7decaf06abff886a0c353b9f159746550cb02be58be801a3cbfa2af91e6807b2e04fe8b185974d442ef7186c0e25a01b30e5003a99bb41c119e9c161fc7d55cc5773f1e785deaef56222dd0ad760990cdc9d25d033e9f92950c993fc06e06f682a80439ac6f1ea90d590b2aa9078777071706e81e03843efe4075278a6bf3dd6c32a62f44b0549121ca92b894ac8b6d9635a80548fe02d3e4234a7d1fef7297472a29339d8a6435b394c40e9b285f952a3793ead082e902fa9912b3eb086d59882b2976fc4320d2d1052bab737e3a021eaf0af604c344bb027250cee058669883fd6e6856b6a6494368d97c561d3af8a1d5e45885080703eb024014807845fd7751bbeee51efd5379f3f86e23a3189a0e18ac03239ed665a1e3038a2139144c97ae62e136057d293629aac2c55ac7ce22cc300704c181f9ce724f020f4eb0ea96426ece0e24152143d876611ddd24559fef0137e035d9f90e762e78036be41c57601c3240923d358ea5862a90283229f5d7b3a9b5621a565087f0ba1903d50be010c09b977d061e3bed95bfe3e9a2e30e77e0a6feda87c3abd8ae647fb7028d8c9d36ee5a09df8105337cc2e41b1b85455418072ca32df4c912f3f407205402bf86dc8f8e3814ce9b89f5ab9cdd8e9c9d77d5db9921871fdfa959fb64e3b26c0383b67d601f49dca143af0c2e2bccc272af0fcfa0eaaa0a99310b99d61c2f883d03f3f05c290e2717bdec6809537df344fa2ae447b2cc643b0a6b703af71f70428e0379ea16197852075ef66ffef9117cd7af0c174c5e5bd6c2c6e015b8a2828dde97021ccf11e228277209de5f54d72ba5cbc2244e7356d3f49fc513a47b35f0a9661e024fb14f2353e5cac3793fa095ee9e3288fc2ae0b068a6d9d962ea972c76baf8ce021a134c4f3316fa19a8cb01d3bd631d5ce77547cdef634152c6f18431b351d88c02d82b6e2990b8a0ddd3bb67636fefbcf4bedf3e616a25b85e7c62d626f64fd51603738ac28084943e101b369fbebc36628fd4ad0863229070f1556798c0a658bbb802f5502878a6510b9431aa04c807d9befecde0d51c2659ca97d4fc51f501cf711402f2988a8d48954d4f4ab7c4264b6c91321cac7789647a558e1de02c99e6a7a88d03f020fad7c6004e1f25c06692f7950a2a1067151a5238937fc58c23f3156cb1840344ae00f85aa51a898d485e5aba593c796ac01f6ba82c3f8f0cf3bebb2566579b03a98a8241a63b3b4b58e1d636a2e3af8f184d371ee65f3aa7de2713bd5b21ea10643e4ec44f0f6dc52e6f9e96cad26de743d65bec70f2d22caf70b231ae9ce4c5d1d9bbcfd09e6f5ca50a4844a24ccb575eb1d878880541bae06017f14bfdfe5e8661999c6006184df4b954c5b8764171a7aa40b5787adc0564f02e3ab70b44d2171ad62d0ac340a4ed396c5384c30753871e191d90da36deea65f7b14430fe21bca0246d8fe3a0447162c2d61ee854c0d5d5bee9be2c212c7beed85bb854b8ca58a85aeef0d4d9ee7a13906a8ade1bad075299a35fb00316c273fc001c5e1f5e074d92109cd542851458b2f4deebd24a74a518ac9ed441f529eb73d4e1c0e5adfe63d374169a11fc55a10935cf205789dca14561565ffacf48e8f3fa6f242734cd2fc02a9d11cb346e8d9cd1e40ac2525dd79c5ac9194e9860be0d1f730ec402a08b50de4c6de5c6d4e3af387559aa76d4fb4615bb76cbb4451bc22b23bc6c84dbbd2bdb5028d0b502170f710c9e7f22bac8c099e8f7e959fc326222d6bdb002537c734c3bafaf216a0a4a52ba59490a61b785c5aad462cb3aab713fe13cf841fc7f9da4b459a5f2ad782cbd83af6c04b54e380e0a18c7aefad7b296069620ad03d9fc65bb8a08b5e132d2e67a358a6dbec5c0da99896e8a4b4de818a723c5e5585efb6ce0404a854dbf849e808c92b6a1c9f8b8a377a86ea98e9ae45eb41eef439bb0bf4f8d583d1d38853ef2224ac304701042eae742f9d63c58b8f1a70d7cfd9303f398a66365c54e5cc6654fe3bde4beaabc3954eddd18ad711ae79bdaac3d42913a9f8f7f31689fb3b27445c0d471ffee701af630897c2b367d1660540dacf5686c1f1af0dd6eb4122bfd57665151b9f0ab619763a71dde6bf03cc210015a31d617eed3830a3ae211f0da4d749f2296cb0620a0bc5dca5d6615d2734f94afadefd92fdcc4f1ca23c25e3341cf58146beded930861a643466c470d8990251d60fe1ab45222d5144ecb6709aab41d2cc9386d9730403a268090c54ceb1010924c279d0ec2c9717cd690ec33f7aa31ac94d0e7effd82a92c44ce54a879cd941c46defecaf82f0cc578dd955cda16e4338a76f4e6b517053b93a6342aa6d0bc7271f1f8833685ca7001830b618e2ee8c5a608deb8de52701df8d1672ca59d6570185273037690cbc89acfb6bd3f612f24afa31ee3898b0caaeceb7244b79ea5e4fcd50deb494819f631bf4733ceeca7b7e1147d979d60d4bab8657f19716955dbca83f99b3a32527b77c1fc9aad3c66dd7e995ce059b76d964957324c76cccd8d7271d57982962b98e40a8a69ba957359e2727083eb43e8966a0a5b77b06c919a85860a436ce8999c6b733bb97c45362d76fd89bf5731db6124770d78cb86a746b66c969ad579901ae5d84b4f73f5ba2fc8ba7ae121c9f416288cff1d3b7e5f438a660713e68342aebf046ae90604495505ff1eccdbc446ac4ed00cd725efe1f72770fde6d01aced00ca099352b2dba3026713c3be39881a8caa54fdf1f4055f423d2050d7a75ba0d891a7a56174617c1cbb4e53a30568d885fcaa39109361e7a802ab76e27173fd7cc6c3e3ce4489a0b4087e73a816eb5d9be73fffcc7cb771f875e729791fa303faf353cbc327ac6f0629a5a0ad4b5fa19ecc52059b57e1e20df01fe6ab41b949c7ee94f510d84d6daaf4e7ce0d5ce14223f48975a288738f926ee74fff2df8ce847466de863e103c36a2230ec8248bd2e7d79a768adb24b5e237d73cda58e045dda0b8570434efcf003e114cfc4fefcd2b240abec8828df6db7605f460afc6f314f0ef455363dbb672a462d2dfd873f7c5bb397070ef74841553d9eabea12cc319b1c5dd24aa2add9d983e4cc1490147960e5efd767e5d190f21d93b66071ece8691d9623288cde31d9bc97401b20f1de6e2e3f194d2fb0ce3a930436750f82c996f89e1ad75d37c45373e778282461c66b0d544e8d392ec4ff95c9e24c2b4b920cf020554aecbaa4839a904e009b7f671ef6de8ce84e2d7478d99c501f807f28792aa6278bc96277a6bdc1eac34852f963bf81684c0fa93d36871f55cd9be7071d1681032c6775a26868f553d90227764367b83685e406ff1866a118289c1176ad14a2ab7d1665892dae7cccf4e0440ace14db1b7a66dddd40ae33e795c03ad37864b09723d0eca6523bbde4d3bb1e677225032fc73d05ee647f31320cbc8a6e351eecd913eadcdf241f955c7679ec8501e1b71bb3424d23899bbdaedbdb12e09209d4b8b9b5c14a4281a26d6c8906560e4edc17f31bdef99e07c7059fdfc912b2cfabe32217233bce50da5e64a963e9424dd8dc14834f36a7f853ec074a848abdc24868e8f16a73056a05d915769742466a256e3723ba56cd91d0b0b3c2b80dd6491c1b0924567253a3238290cd8a73e98383840080b37be1739345dd3958c4630c434d10ab77cdb8fb8beed020dc56eefa644a5f614ab002261e436db52984f7b1836be5dddacbd26d45a4b047e6420f60f43e0298cafab152470149e2fe3628bfa8cc57d6398b9dfa633d1db591d5dd83af7d6c36fda48bb1cac42dd2fd61278165c98e768837a24fee6ae7aca9a0169a4355392c5d22c6aaa416529a3515388658e11f7fdea3b6a0ce0fb102005817f7adc409b9e37d68a20ec42ee7fdb4fb0ffe9749b61f80e94d8c9d8515d990b49ad67482dff01d3862d7d48496b8e278b66b660cb440195bfab1ed108428a106dbc9dad0cfdf15ab23d595581ba5cf6e8e75e39a222666cbf808969d4a41b0f298f3a37e543ecf2506f569499f0b7a963b0781ff66ae4a175cf95cb64a47595ce987026c57759235b35bba18bd8cffbeb0055f164833f877f41a62105ce1d75a9b86ad3b210b0de87c05d840864c5b5453afe97d5dc7efd2aa8690710aff71219f9de8c75aba
*/

