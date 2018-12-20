#define BOOST_TEST_MODULE CitationGraphTests


//#include <boost/test/excecution_monitor.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "citation_graph.h"
#include "dag.h"
#include "citation_graph.h"
#include "Publication.h"

class PublicationExample {
public:
	typedef typename std::string id_type;
	PublicationExample(id_type const &_id) : id(_id) {
	}
	id_type get_id() const noexcept {
		return id;
	}
private:
	id_type id;
};


BOOST_AUTO_TEST_SUITE(ProvidedTests);

	BOOST_AUTO_TEST_CASE(example) {
		CitationGraph<PublicationExample> gen("Goto Considered Harmful");
		PublicationExample::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
		BOOST_ASSERT(gen.exists(id1));
		BOOST_ASSERT(gen.get_parents(id1).size() == 0);
		gen.create("A", id1);
		gen.create("B", id1);
		BOOST_ASSERT(gen.get_children(id1).size() == 2);
		gen.create("C", "A");
		gen.add_citation("C", "B");
		BOOST_ASSERT(gen.get_parents("C").size() == 2);
		BOOST_ASSERT(gen.get_children("A").size() == 1);
		std::vector<PublicationExample::id_type> parents;
		parents.push_back("A");
		parents.push_back("B");
		gen.create("D", parents);
		BOOST_ASSERT(gen.get_parents("D").size() == parents.size());
		BOOST_ASSERT(gen.get_children("A").size() == 2);
		BOOST_ASSERT("D" == gen["D"].get_id());
		gen.remove("A");
		BOOST_ASSERT(!gen.exists("A"));
		BOOST_ASSERT(gen.exists("B"));
		BOOST_ASSERT(gen.exists("C"));
		BOOST_ASSERT(gen.exists("D"));
		gen.remove("B");
		BOOST_ASSERT(!gen.exists("A"));
		BOOST_ASSERT(!gen.exists("B"));
		BOOST_ASSERT(!gen.exists("C"));
		BOOST_ASSERT(!gen.exists("D"));
		try {
			gen["E"];
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
		try {
			gen.create("E", "Goto Considered Harmful");
			gen.create("E", "Goto Considered Harmful");
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
		try {
			gen.remove("Goto Considered Harmful");
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
	}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(SimpleOperations);

	BOOST_AUTO_TEST_CASE(Cycle_removing) {
		CitationGraph<PublicationExample> gen("X");
		PublicationExample::id_type const id1 = gen.get_root_id();
		BOOST_ASSERT(gen.exists(id1));
		BOOST_ASSERT(gen.get_parents(id1).size() == 0);
		gen.create("A", id1);
		gen.create("B", id1);
		BOOST_ASSERT(gen.get_children(id1).size() == 2);
		gen.create("C", "A");
		gen.add_citation("C", "B");
		BOOST_ASSERT(gen.get_parents("C").size() == 2);
		BOOST_ASSERT(gen.get_children("A").size() == 1);
		std::vector<PublicationExample::id_type> parents;
		parents.push_back("A");
		parents.push_back("B");
		gen.create("D", parents);
		gen.add_citation("C", "D");
		BOOST_ASSERT(gen.get_parents("C").size() == 3);
		BOOST_ASSERT(gen.get_parents("D").size() == parents.size());
		BOOST_ASSERT(gen.get_children("A").size() == 2);
		BOOST_ASSERT("D" == gen["D"].get_id());
		gen.remove("A");
		BOOST_ASSERT(gen.get_parents("C").size() == 2);
		BOOST_ASSERT(!gen.exists("A"));
		BOOST_ASSERT(gen.exists("B"));
		BOOST_ASSERT(gen.exists("C"));
		BOOST_ASSERT(gen.exists("D"));
		gen.remove("B");
		BOOST_ASSERT(!gen.exists("A"));
		BOOST_ASSERT(!gen.exists("B"));
		BOOST_ASSERT(!gen.exists("C"));
		BOOST_ASSERT(!gen.exists("D"));

	}

	BOOST_AUTO_TEST_CASE(UnconectedGraph) {
		CitationGraph<PublicationExample> gen("X");
		PublicationExample::id_type const root = gen.get_root_id();
		gen.create("A", root);
		gen.create("C", "A");
		gen.create("D", "C");
		gen.create("E", "D");

		BOOST_ASSERT(gen.get_parents("A").size() == 1);
		BOOST_ASSERT(gen.get_parents("C").size() == 1);
		BOOST_ASSERT(gen.get_parents("D").size() == 1);
		BOOST_ASSERT(gen.get_parents("E").size() == 1);

		BOOST_ASSERT(gen.get_children("A").size() == 1);
		BOOST_ASSERT(gen.get_children("C").size() == 1);
		BOOST_ASSERT(gen.get_children("D").size() == 1);
		BOOST_ASSERT(gen.get_children("E").size() == 0);

		gen.remove("C");

		BOOST_ASSERT(gen.get_children("A").size() == 0);
		BOOST_ASSERT(gen.get_children("X").size() == 1);

		try {
			gen["C"];
		} catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}

		try {
			gen["D"];
		} catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}

		try {
			gen["E"];
		} catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}




	}

	BOOST_AUTO_TEST_CASE(UnconectedGraph_two_sources) {
		CitationGraph<PublicationExample> gen("X");
		PublicationExample::id_type const root = gen.get_root_id();
		gen.create("A", root);
		gen.create("C", "A");
		gen.create("D", "C");
		gen.create("E", "D");
		gen.create("B", root);
		gen.add_citation("E", "B");

		BOOST_ASSERT(gen.get_parents("A").size() == 1);
		BOOST_ASSERT(gen.get_parents("B").size() == 1);
		BOOST_ASSERT(gen.get_parents("C").size() == 1);
		BOOST_ASSERT(gen.get_parents("D").size() == 1);
		BOOST_ASSERT(gen.get_parents("E").size() == 2);

		BOOST_ASSERT(gen.get_children("A").size() == 1);
		BOOST_ASSERT(gen.get_children("B").size() == 1);
		BOOST_ASSERT(gen.get_children("C").size() == 1);
		BOOST_ASSERT(gen.get_children("D").size() == 1);
		BOOST_ASSERT(gen.get_children("E").size() == 0);

		gen.remove("C");

		BOOST_ASSERT(gen.get_children("X").size() == 2);
		BOOST_ASSERT(gen.get_children("A").size() == 0);
		BOOST_ASSERT(gen.get_children("B").size() == 1);
		BOOST_ASSERT(gen.get_children("E").size() == 0);


		BOOST_ASSERT(gen.get_parents("X").size() == 0);
		BOOST_ASSERT(gen.get_parents("A").size() == 1);
		BOOST_ASSERT(gen.get_parents("B").size() == 1);
		BOOST_ASSERT(gen.get_parents("E").size() == 1);


		try {
			gen["C"];
		} catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}

		try {
			gen["D"];
		} catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	BOOST_AUTO_TEST_CASE(create) {
		CitationGraph<PublicationExample> gen("X");
		PublicationExample::id_type const root = gen.get_root_id();

		std::vector<PublicationExample::id_type> parents_F;
		std::vector<PublicationExample::id_type> parents_E;

		parents_F.emplace_back("B");
		parents_F.emplace_back("D");

		parents_E.emplace_back("B");
		parents_E.emplace_back("C");

		gen.create("A", root);
		gen.create("B", root);
		gen.create("C", root);
		gen.create("D", "A");

		gen.create("F", parents_F);
		gen.create("E", parents_E);



		BOOST_ASSERT(gen.get_parents("A").size() == 1);
		BOOST_ASSERT(gen.get_parents("B").size() == 1);
		BOOST_ASSERT(gen.get_parents("C").size() == 1);
		BOOST_ASSERT(gen.get_parents("D").size() == 1);
		BOOST_ASSERT(gen.get_parents("E").size() == 2);
		BOOST_ASSERT(gen.get_parents("F").size() == 2);

		BOOST_ASSERT(gen.get_children("A").size() == 1);
		BOOST_ASSERT(gen.get_children("B").size() == 2);
		BOOST_ASSERT(gen.get_children("C").size() == 1);
		BOOST_ASSERT(gen.get_children("D").size() == 1);
		BOOST_ASSERT(gen.get_children("E").size() == 0);
		BOOST_ASSERT(gen.get_children("F").size() == 0);
	}

	BOOST_AUTO_TEST_CASE(segfault) {
		CitationGraph<PublicationExample> gen("X");
		std::vector<PublicationExample::id_type> parents_E;
		parents_E.emplace_back("A");
		parents_E.emplace_back("B");


		gen.create("A", "X");
		gen.create("B", "X");
		gen.create("C", parents_E);


		BOOST_ASSERT(gen.get_parents("A").size() == 1);
		BOOST_ASSERT(gen.get_parents("B").size() == 1);
		BOOST_ASSERT(gen.get_parents("C").size() == 2);
		BOOST_ASSERT(gen.get_parents("X").size() == 0);

		BOOST_ASSERT(gen.get_children("X").size() == 2);
		BOOST_ASSERT(gen.get_children("A").size() == 1);
		BOOST_ASSERT(gen.get_children("B").size() == 1);
		BOOST_ASSERT(gen.get_children("C").size() == 0);

		gen.remove("A");
		gen.remove("B");
	}



BOOST_AUTO_TEST_SUITE_END()



