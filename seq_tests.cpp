#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <string>
#include <exception>
#include <vector>

#if 1
#include "sequence.h"
#define subseq(a, pos, n) a.subseq(pos, n)
#else
#include <string_view>
template<class T, class Traits>
using sequence = std::basic_string_view<T, Traits>;
#define subseq(a, pos, n) a.substr(pos, n)
#endif
#define remove_prefix(a, n) a.remove_prefix(n)
#define remove_suffix(a, n) a.remove_suffix(n)

struct case_insensitive_char_traits : public std::char_traits<char> {
	static char to_upper(char ch) { return std::toupper((unsigned char)ch); }

	static bool eq(char c1, char c2) { return to_upper(c1) == to_upper(c2); }

	static bool lt(char c1, char c2) { return to_upper(c1) < to_upper(c2); }

	static int compare(const char *s1, const char *s2, size_t n) {
		while (n-- != 0) {
			if (to_upper(*s1) < to_upper(*s2)) return -1;
			if (to_upper(*s1) > to_upper(*s2)) return 1;
			++s1;
			++s2;
		}
		return 0;
	}

	static const char *find(const char *s, int n, char a) {
		auto const ua(to_upper(a));
		while (n-- != 0) {
			if (to_upper(*s) == ua)
				return s;
			s++;
		}
		return nullptr;
	}
};

struct case_insensitive_assign_char_traits : public std::char_traits<char> {
	static void assign(char &r, const char &a) { r = std::tolower(a); }

	static char *assign(char *p, std::size_t count, char a) { return std::fill_n(p, count, std::tolower(a)); }

	static char_type* copy(char_type* dest, const char_type* src, std::size_t count) {
		return std::transform(src, src + count, dest, [](char c) { return std::tolower(c); });
	}
};

using int_sequence = sequence<int, std::char_traits<int>>;
using char_sequence = sequence<char, std::char_traits<char>>;
using char_sequence_ci = sequence<char, case_insensitive_char_traits>;
using char_sequence_ci_assign = sequence<char, case_insensitive_assign_char_traits>;

TEST_CASE("constructor") {
	SUBCASE("empty") {
		char_sequence s;
		char_sequence_ci s2;
		char_sequence_ci_assign s3;
		int_sequence s4;
	}

	SUBCASE("from sequence") {
		char_sequence_ci_assign s("AaB", 3);
		CHECK(s.size() == 3);
		CHECK(s == char_sequence_ci_assign("AaB", 3));
	}

	SUBCASE("from_C_str") {
		char_sequence a("abc");
		CHECK(a.size() == 3);
		//        CHECK(a == "abc");
		CHECK(a == char_sequence("abc", 3));
		CHECK(a == char_sequence("abcdff", 3));

		char_sequence_ci ci_a("ABC");
		CHECK(ci_a.size() == 3);
		//        CHECK(ci_a == "ABC");
		CHECK(ci_a == char_sequence_ci("ABCeef", 3));
	}

	SUBCASE("copy constructor") {
		char_sequence_ci_assign s("AaB", 3);
		char_sequence_ci_assign s_copy(s);
		CHECK(s_copy.size() == 3);
		CHECK(s == s_copy);
	}

	SUBCASE("int sequence") {
		int x[3] = { 1, 2, 3 };
		int_sequence s(&x[1], 2);
		CHECK(s.size() == 2);
		std::vector<int> y = { 2, 3, 4, 5 };
		int_sequence s2(y.data() + 1, 3);
		CHECK(s2.size() == 3);
	}
}

TEST_CASE("iterators") {
	SUBCASE("begin/end") {
		char_sequence s("a", 1);
		CHECK(std::distance(std::begin(s), std::end(s)) == 1);
		CHECK(*s.begin() == 'a');

		char_sequence s2;
		CHECK(s2.begin() == s2.end());
	}

	SUBCASE("rbegin/rend") {
		char_sequence s("ab", 2);
		CHECK(*std::prev(s.rend()) == 'a');
		CHECK(*s.rbegin() == 'b');
	}
}

TEST_CASE("element access") {
	SUBCASE("operator[]") {
		char_sequence_ci_assign s("ABCDI", 5);
		CHECK(s[0] == 'A');
		CHECK(s[4] == 'I');
	}

	SUBCASE("int sequence") {
		int x[3] = { 1, 2, 3 };
		int_sequence s(&x[1], 2);
		CHECK(s.at(0) == 2);
		std::vector<int> y = { 2, 3, 4, 5 };
		int_sequence s2(y.data() + 1, 3);
		CHECK(s2.at(0) == 3);
		CHECK_THROWS_AS(s2.at(-1), std::out_of_range);
	}

	SUBCASE("at") {
		char_sequence_ci_assign s("ABCDI", 5);
		CHECK(s.at(0) == 'A');
		CHECK(s.at(4) == 'I');
		CHECK_THROWS_AS(s.at(5), std::out_of_range);
	}

	SUBCASE("front/back") {
		char_sequence_ci s("ABCDI", 5);
		CHECK(s.front() == 'A');
		CHECK(s.back() == 'I');
	}
}

TEST_CASE("capacity") {
	SUBCASE("empty") {
		char_sequence_ci_assign s("ABCDI", 5);
		CHECK(!s.empty());

		char_sequence_ci_assign s2;
		CHECK(s2.empty());
	}

	SUBCASE("size") {
		char_sequence_ci_assign s("ABCDI", 5);
		CHECK(s.size() == 5);

		char_sequence_ci_assign s2("", 0);
		CHECK(s2.size() == 0);
	}
}

TEST_CASE("modifiers") {
	SUBCASE("remove_prefix") {
		char_sequence_ci_assign s("ABCDI", 5);
		remove_prefix(s, 2);
		CHECK(s == char_sequence_ci_assign("CDI", 3));

		remove_prefix(s, 3);
		CHECK(s.empty());
	}

	SUBCASE("remove_suffix") {
		char_sequence_ci_assign s("ABCDI", 5);
		remove_suffix(s, 2);
		CHECK(s == char_sequence_ci_assign("ABC", 3));

		remove_suffix(s, 3);
		CHECK(s.empty());
	}

	SUBCASE("swap") {
		char_sequence_ci_assign a("AAAAAAAAA", 9);
		char_sequence_ci_assign b("B", 1);
		char_sequence_ci_assign a_copy(a);
		char_sequence_ci_assign b_copy(b);

		CHECK_NOTHROW(a_copy.swap(b_copy));
		CHECK(a_copy == b);
		CHECK(b_copy == a);

		CHECK_NOTHROW(a_copy.swap(b_copy));
		CHECK(a_copy == a);
		CHECK(b_copy == b);
	}
}

TEST_CASE("operations") {
	SUBCASE("copy") {
		char_sequence_ci_assign a("AbAb");
		std::string s(4, '0');
		a.copy(&s[0], 4);

		CHECK(s == "abab");
	}

	SUBCASE("compare_char_seq") {
		char_sequence cs1("aba");
		char_sequence cs1_u("AbA");
		char_sequence cs2("abc");

		CHECK(cs1.compare(cs1_u) != 0);
		CHECK(cs1.compare(cs1) == 0);
		CHECK(cs1.compare(cs2) < 0);
		CHECK(cs2.compare(cs1) > 0);
	}

	SUBCASE("compare_char_seq_ci") {
		char_sequence_ci cs1("aba");
		char_sequence_ci cs1_u("AbA");

		CHECK(cs1_u.front() == 'A');
		CHECK(cs1.front() == 'a');
		CHECK(cs1.compare(cs1_u) == 0);
	}

	SUBCASE("subseq") {
		char_sequence cs1("abab");
		CHECK(subseq(cs1, 1, 2) == char_sequence("ba"));

		CHECK(subseq(subseq(cs1, 2, 3), 0, 1) == char_sequence("a"));
	}

	SUBCASE("find") {
		char_sequence_ci cs1("Abbbbbb");
		char_sequence_ci cs2("ab");

		CHECK(cs1.find(cs2) == 0);
	}


	SUBCASE("rfind") {
		char_sequence_ci cs1("AbAB");
		char_sequence_ci cs2("ab");
		char_sequence_ci cs3("de");

		CHECK(cs1.rfind(cs2) == 2);
		CHECK(cs1.rfind(cs1) == 0);
		CHECK(cs1.rfind(cs3) == char_sequence::npos);
	}
}