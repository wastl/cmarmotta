//
// Created by wastl on 18.04.15.
//

#include "gtest.h"
#include "model/rdf_model.h"

namespace marmotta {

    TEST(URITest, Construct) {
        rdf::URI uri1("http://www.example.com/U1");
        rdf::URI uri2(std::string("http://www.example.com/U2"));

        ASSERT_EQ(uri1.getUri(), "http://www.example.com/U1");
        ASSERT_EQ(uri2.getUri(), "http://www.example.com/U2");
    }

    TEST(URITest, Equality) {
        rdf::URI uri1("http://www.example.com/U1");
        rdf::URI uri2("http://www.example.com/U1");
        rdf::URI uri3("http://www.example.com/U3");

        ASSERT_EQ(uri1, uri2);
        ASSERT_NE(uri1, uri3);
    }

    TEST(BNodeTest, Construct) {
        rdf::BNode bNode1("n1");
        rdf::BNode bNode2(std::string("n2"));

        ASSERT_EQ(bNode1.getId(), "n1");
        ASSERT_EQ(bNode2.getId(), "n2");
    }

    TEST(BNodeTest, Equality) {
        rdf::BNode bNode1("n1");
        rdf::BNode bNode2("n1");
        rdf::BNode bNode3("n3");

        ASSERT_EQ(bNode1, bNode2);
        ASSERT_NE(bNode1, bNode3);
    }

    TEST(StringLiteralTest, Construct) {
        rdf::StringLiteral l1("Hello, World!");
        rdf::StringLiteral l2("Hello, World!", "en");
        rdf::StringLiteral l3(std::string("Hello, World!"));

        ASSERT_EQ(l1.getContent(), "Hello, World!");
        ASSERT_EQ(l1.getLanguage(), "");

        ASSERT_EQ(l2.getContent(), "Hello, World!");
        ASSERT_EQ(l2.getLanguage(), "en");

        ASSERT_EQ(l3.getContent(), "Hello, World!");
        ASSERT_EQ(l3.getLanguage(), "");
    }

    TEST(StringLiteralTest, Equality) {
        rdf::StringLiteral l1("Hello, World!");
        rdf::StringLiteral l2("Hello, World!");
        rdf::StringLiteral l3("Hello, World!", "en");
        rdf::StringLiteral l4("The quick brown fox jumps over the lazy dog.");

        ASSERT_EQ(l1, l2);
        ASSERT_NE(l1, l3);
        ASSERT_NE(l1, l4);
    }

    TEST(StatementTest, Construct) {
        rdf::Statement s(rdf::URI("http://www.example.com/S1"), rdf::URI("http://www.example.com/P1"), "Hello World!");

        ASSERT_EQ(s.getSubject(), "http://www.example.com/S1");
        ASSERT_EQ(s.getPredicate(), "http://www.example.com/P1");
        ASSERT_EQ(s.getObject(), "Hello World!");
    }
}