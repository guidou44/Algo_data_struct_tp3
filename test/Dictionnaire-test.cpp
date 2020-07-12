#include "gtest/gtest.h"
#include "Dictionnaire.h"
#include <algorithm>

using namespace TP3;
using namespace std;

TEST(Dictionnaire, givenNoWord_whenDeleteWord_thenItThrowsProper) {
    TP3::Dictionnaire dictSubject = Dictionnaire();

    ASSERT_THROW(dictSubject.supprimeMot("mot"), logic_error);
}

TEST(Dictionnaire, givenWordNotThere_whenDeleteWord_thenItThrowsProper) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    dictSubject.ajouteMot("mot", "word");

    ASSERT_THROW(dictSubject.supprimeMot("test"), logic_error);
}

TEST(Dictionnaire, givenWordThere_whenDeleteWordThantItDeletesProper) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    dictSubject.ajouteMot("mot", "word");
    dictSubject.ajouteMot("test", "test");
    dictSubject.ajouteMot("test", "essaie");
    dictSubject.ajouteMot("bird", "oiseau");
    dictSubject.ajouteMot("bird", "aigle");
    dictSubject.ajouteMot("truck", "camion");

    dictSubject.supprimeMot("bird");

    ASSERT_FALSE(dictSubject.appartient("bird"));
}

TEST(Dictionnaire, givenIdenticalWords_whenGetSimilitude_itReturnsMaxSimilitude) {
    TP3::Dictionnaire dictSubject = Dictionnaire();

    double similitude = dictSubject.similitude("oiseau", "oiseau");

    ASSERT_EQ(1, similitude);
}

TEST(Dictionnaire, givenCompletelyDifferentWords_whenGetSimilitude_itReturnsMinSimilitude) {
    TP3::Dictionnaire dictSubject = Dictionnaire();

    double similitude = dictSubject.similitude("oiseau", "avion");

    ASSERT_EQ(0, similitude);
}

TEST(Dictionnaire, givenSimilarWords_whenGetSimilitude_itReturnsPlausibleSimilitude) {
    TP3::Dictionnaire dictSubject = Dictionnaire();

    double similitude = dictSubject.similitude("oiseau", "oiseauxxx");

    ASSERT_TRUE(0 < similitude);
    ASSERT_TRUE(similitude < 1);
}

TEST(Dictionnaire, givenWordNotThere_whenAddWord_thenItAddsWordProperly) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    ASSERT_FALSE(dictSubject.appartient("mot"));
    ASSERT_TRUE(dictSubject.estVide());

    dictSubject.ajouteMot("mot", "word");

    ASSERT_TRUE(dictSubject.appartient("mot"));
    ASSERT_FALSE(dictSubject.estVide());
}

TEST(Dictionnaire, givenWordWithSingleTraduction_whenTraductWord_thenItReturnsOnlyOneTraduction) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    dictSubject.ajouteMot("mot", "word");
    dictSubject.ajouteMot("test", "test");
    dictSubject.ajouteMot("test", "essaie");
    dictSubject.ajouteMot("bird", "oiseau");
    dictSubject.ajouteMot("bird", "aigle");
    dictSubject.ajouteMot("truck", "camion");

    vector<string> traductions = dictSubject.traduit("truck");

    ASSERT_EQ(1, traductions.size());
    ASSERT_EQ("camion", traductions[0]);
}

TEST(Dictionnaire, givenWordWithManyTraductions_whenTraductWord_thenItReturnsMultipleTraductions) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    dictSubject.ajouteMot("mot", "word");
    dictSubject.ajouteMot("test", "test");
    dictSubject.ajouteMot("test", "essaie");
    dictSubject.ajouteMot("bird", "oiseau");
    dictSubject.ajouteMot("bird", "aigle");
    dictSubject.ajouteMot("truck", "camion");

    vector<string> traductions = dictSubject.traduit("bird");

    ASSERT_EQ(2, traductions.size());
    ASSERT_TRUE(find(traductions.begin(), traductions.end(), "oiseau") != traductions.end());
    ASSERT_TRUE(find(traductions.begin(), traductions.end(), "aigle") != traductions.end());
}

TEST(Dictionnaire, givenSimilarWords_whenGetSuggestionItReturnsAllSimilarWordsInTree) {
    TP3::Dictionnaire dictSubject = Dictionnaire();
    dictSubject.ajouteMot("bird", "aigle");
    dictSubject.ajouteMot("beard", "barbe");
    dictSubject.ajouteMot("bread", "pain");
    dictSubject.ajouteMot("bed", "lit");

    vector<string> suggestions = dictSubject.suggereCorrections("berad");

    ASSERT_EQ(4, suggestions.size());
    ASSERT_TRUE(find(suggestions.begin(), suggestions.end(), "bird") != suggestions.end());
    ASSERT_TRUE(find(suggestions.begin(), suggestions.end(), "beard") != suggestions.end());
    ASSERT_TRUE(find(suggestions.begin(), suggestions.end(), "bread") != suggestions.end());
    ASSERT_TRUE(find(suggestions.begin(), suggestions.end(), "bed") != suggestions.end());
}

