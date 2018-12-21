//
// Created by roman on 18.12.18.
//

#include "TrigramsContainer.h"

void TrigramsContainer::insert(trigram t) {
    trigrams.push_back(t);
}

void TrigramsContainer::makeAccessible() {
    std::sort(trigrams.begin(), trigrams.end()/*, [](const trigram a, const trigram b){ return a < b;}*/);
//    QVector<trigram> new_trigrams;
//    auto it = trigrams.begin();
//    if (trigrams.end() != trigrams.begin()) {
//        while (it + 1 != trigrams.end()) {
//            if (*it != *(it + 1))
//                new_trigrams.push_back(*it);
//            it++;
//        }
//        new_trigrams.push_back(*trigrams.begin());
//    }
//    trigrams.swap(new_trigrams);
}

bool TrigramsContainer::contains(trigram t) {
    return std::binary_search(trigrams.begin(), trigrams.end(), t);
}

QVector<trigram>::iterator TrigramsContainer::begin() {
    return trigrams.begin();
}

QVector<trigram>::iterator TrigramsContainer::end() {
    return trigrams.end();
}

int TrigramsContainer::size() {
    return trigrams.size();
}

void TrigramsContainer::clear() {
    trigrams.clear();
}
